#!/usr/bin/bash
lxc-destroy 1
mkdir /tmp/colsuptest
pushd /tmp/colsuptest
rm -f build run program player.7z

cat <<EOF >build
#!/usr/bin/bash
echo Building...
cat <<DONE >program
echo Run success!
DONE
EOF

cat <<EOF >run
#!/usr/bin/bash
echo Running...
bash program
EOF

7z a player.7z build run

popd

curl -X PUT "http://localhost:8000/player/1?env_id=2&automake=false" -F data=@/tmp/colsuptest/player.7z
echo
