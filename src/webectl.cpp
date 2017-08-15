extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
}

#include <cstdio>
#include "utils.h"

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_un name;
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    IF_NEGATIVE_EXIT(sock);
    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, "/tmp/webed.sock");
    IF_NEGATIVE_EXIT(sendto(sock, argv[0], strlen(argv[0]), 0, (struct sockaddr *)&name, sizeof(struct sockaddr_un)))
    close(sock);
    return 0;
}
