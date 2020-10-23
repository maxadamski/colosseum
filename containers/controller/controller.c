// TODO(piotr): make this directory configurable
#define LXC_HOME "/home/piotr/.local/share/lxc/"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <lxc/lxccontainer.h>

typedef struct lxc_container lxc_container;

#define log(fmt, ...) {\
    fprintf(stderr, "["__FILE__":%d] " fmt "\n", __LINE__, __VA_ARGS__);\
}

lxc_container * make_player(char *name) {
    lxc_container *c;
    int ret = 1;

    c = lxc_container_new(name, NULL);
    if (!c) {
        log("Failed to setup lxc_container struct for \"%s\"", name);
        return NULL;
    }

    if (c->is_defined(c)) {
        log("Container \"%s\" already exists", name);
        return NULL;
    }

    if (!c->createl(c, "player", NULL, NULL, LXC_CREATE_QUIET, NULL)) {
        log("Failed to create container rootfs for \"%s\"", name);
        return NULL;
    }

    if (!c->startl(c, 1, "player", NULL)) {
        log("Failed to start the container for \"%s\"", name);
        return NULL;
    }

    return c;
}

int main() {
    lxc_container *p1 = make_player("p1");
    if (!p1) {
        return 1;
    }

    int p1_in  = open(LXC_HOME "p1/rootfs/fifo_in", O_WRONLY);
    int p1_out = open(LXC_HOME "p1/rootfs/fifo_out", O_RDONLY);

    char msg[] = "ala ma kota";
    printf("sending message \"%s\"\n", msg);
    write(p1_in, msg, sizeof(msg));
    char buf[128] = {};
    read(p1_out, buf, sizeof(buf));
    printf("received \"%s\"\n", buf);

    //p1->stop(p1);
    //p1->destroy(p1);

    return 0;
}
