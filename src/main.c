#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../include/config.h"
#include "../include/server.h"
#include "../include/log.h"

int main(int argc, char **argv) {
    int port = DEFAULT_PORT;

    if (argc > 1) {
        port = atoi(argv[1]);
    }
    if (port<=0 || port>65535) {
        puts("Invalid port");
        return 1;
    }
    log(IMPORTANT, "\n%s v%s is listening on port %d", APP_NAME, VERSION, port);
    return listen_and_serve((u_int16_t)port);
}