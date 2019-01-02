#include "pipe_networking.h"


int main() {
    signal(SIGINT, sighandler);
    
    while (1) {
        // create well known pipe
        int mk_wkp = mkfifo("main", 0644);
        if (mk_wkp == - 1) {
            printf("ERROR: %d --> %s\n", errno, strerror(errno));
            return 1;
        }
        else printf("[SERVER] CREATED WELL KNOWN PIPE\n");
        // wait
        int wkp = open("main", O_RDONLY);
        // receive message
        char msg[HANDSHAKE_BUFFER_SIZE];
        read(wkp, msg, HANDSHAKE_BUFFER_SIZE);
        printf("[SERVER] CLIENT MESSAGE: %s\n", msg);
        // fork subserver
        int f = fork();

        if (f) { // parent
            // remove well known pipe
            remove("main");
            close(wkp);
            printf("[SERVER] REMOVED WELL KNOWN PIPE\n");
        }
        else { // subserver
            int to_client;
            int from_client;

            from_client = wkp;
            server_handshake( from_client, &to_client, msg );
            char data[BUFFER_SIZE];
            // get data from client
            while(read(from_client, data, BUFFER_SIZE)) {
                printf("[SUBSERVER %d] CLIENT MESSAGE: %s", getpid(), data);
                // process data
                int i = 0;
    			while (data[i]) {
                    if (i%2)
                        data[i] = toupper(data[i]); // convert to uppercase
        			i++;
				}
                // reply to client
                int w = write(to_client, data, BUFFER_SIZE);
            }
            close(to_client);
            close(from_client);
            printf("[SUBSERVER %d] SESSION COMPLETE\n", getpid());
            return 0;
        }
    }
    return 0;
}

static void sighandler(int signo) {
    if (signo == SIGINT) {
        remove("main");
        printf("wkp removed\n");
        exit(0);
    }
}
