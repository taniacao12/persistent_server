#include "pipe_networking.h"

static void sighandler(int signo) {
    if (signo == SIGINT) {
        remove("main");
        printf("\nREMOVED WELL KNOWN PIPE\n");
        exit(0);
    }
}

int main() {
    signal(SIGINT, sighandler);

    int to_client;
    int from_client;

    while (1) {
        from_client = server_handshake( &to_client );
        char data[BUFFER_SIZE];
        // get data from client
        while(read(from_client, data, BUFFER_SIZE)) {
            printf("CLIENT MESSAGE: %s", data);
            // reply to client
            int w = write(to_client, data, BUFFER_SIZE);
        }
        // when client exits, reset
        close(to_client);
        close(from_client);
    }
}