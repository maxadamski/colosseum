#!/bin/bash
if ! [ $SUDO_USER ]; then
    echo >&2 "This script must be run with sudo!"
    exit 1
fi
home=$(eval echo "~$SUDO_USER")

config_user_ns=$(zgrep CONFIG_USER_NS /proc/config.gz | cut -d= -f2)
if ! [[ "$config_user_ns" == "y" ]]; then
	echo "Your kernel was not compiled with user namespace support"
	exit 1
fi

if ! [ -d "$home/.config/lxc" ]; then
    echo "Making $home/.config/lxc"
    mkdir "$home/.config/lxc"
    chown $SUDO_USER "$home/.config/lxc"
    chgrp $SUDO_USER "$home/.config/lxc"
fi
echo "Making $home/.config/lxc/default.conf"
cat >"$home/.config/lxc/default.conf" <<EOF
lxc.net.0.type = empty
lxc.idmap = u 0 100000 65536
lxc.idmap = g 0 100000 65536
EOF
chown $SUDO_USER "$home/.config/lxc/default.conf"
chgrp $SUDO_USER "$home/.config/lxc/default.conf"

echo "Copying lxc-player to /usr/share/lxc/templates/lxc-player"
cp template/lxc-player /usr/share/lxc/templates/lxc-player
chown $SUDO_USER "/usr/share/lxc/templates/lxc-player"
chgrp $SUDO_USER "/usr/share/lxc/templates/lxc-player"

if ! grep "$SUDO_USER:100000:65536" /etc/subuid >/dev/null; then
    echo "Allocating uid range"
    echo "$SUDO_USER:100000:65536" >> /etc/subuid
fi

if ! grep "$SUDO_USER:100000:65536" /etc/subgid >/dev/null; then
    echo "Allocating gid range"
    echo "$SUDO_USER:100000:65536" >> /etc/subgid
fi

sysctl_out=$(sysctl kernel.unprivileged_userns_clone)
if [ "$sysctl_out" != "kernel.unprivileged_userns_clone = 1" ]; then
    echo "Setting kernel.unprivileged_userns_clone = 1"
    sysctl kernel.unprivileged_userns_clone=1
    echo "kernel.unprivileged_userns_clone=1" >> /etc/sysctl.conf
fi

uidmap_out=$(ls -l /bin/newuidmap)
if ! [[ "$uidmap_out" == "-rwsr-xr-x"* ]]; then
    echo "Setting SUID for newuidmap"
    chmod 4755 /bin/newuidmap
fi
gidmap_out=$(ls -l /bin/newgidmap)
if ! [[ "$gidmap_out" == "-rwsr-xr-x"* ]]; then
    echo "Setting SUID for newgidmap"
    chmod 4755 /bin/newgidmap
fi
echo "Done!"
