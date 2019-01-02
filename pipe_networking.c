#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
void server_handshake(int from_client, int *to_client, char *msg) {
    // connect to private FIFO
    *to_client = open(msg, O_WRONLY);
    // send initial message
    write(*to_client, msg, HANDSHAKE_BUFFER_SIZE);
    // get response from client
    read(from_client, msg, HANDSHAKE_BUFFER_SIZE);
    printf("[SUBSERVER %d] CLIENT MESSAGE:[%s]\n", getpid(), msg);
    // return file descriptor to well known pipe
    printf("[SUBSERVER %d] HANDSHAKE COMPLETE\n", getpid());
}

/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
    // create private FIFO
    char pipe_name[HANDSHAKE_BUFFER_SIZE];
    sprintf(pipe_name, "%d", getpid());
    int private = mkfifo(pipe_name, 0644);
    if (private == - 1) {
        printf("ERROR: %d --> %s\n", errno, strerror(errno));
        return 1;
    }
    printf("[CLIENT] CREATED PRIVATE PIPE\n");
    // set to_server to wwll known pipe
    *to_server = open("main", O_WRONLY);
    if (*to_server == - 1) {
        printf("ERROR: %d --> %s\n", errno, strerror(errno));
        remove(pipe_name);
        return 1;
    }
    // send private FIFO name to server
    int w = write(*to_server, pipe_name, HANDSHAKE_BUFFER_SIZE);
    printf("[CLIENT] SENT PRIVATE PIPE NAME TO SERVER\n");
    // wait for response
    int receive = open(pipe_name, O_RDONLY);
    // receive server message
    char msg[HANDSHAKE_BUFFER_SIZE];
    int r = read(receive, msg, HANDSHAKE_BUFFER_SIZE);
    printf("[CLIENT] SERVER MESSAGE: %s\n", msg);
    // remove private FIFO
    remove(pipe_name);
    printf("[CLIENT] REMOVED PRIVATE PIPE\n");
    // send response to server
    w = write(*to_server, ACK, HANDSHAKE_BUFFER_SIZE);
    // return file descriptor to private FIFO
    return receive;
}
