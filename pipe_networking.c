#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
    // create well known pipe
    int wkp = mkfifo("main", 0644);
    if (wkp == - 1) {
        printf("error %d: %s\n", errno, strerror(errno));
        return 1;
    }
    else printf("CREATED WELL KNOWN PIPE\n");
    // wait
    int read_end = open("main", O_RDONLY);
    // receive message
    char msg[HANDSHAKE_BUFFER_SIZE];
    int r = read(read_end, msg, HANDSHAKE_BUFFER_SIZE);
    printf("CLIENT MESSAGE: %s\n", msg);
    // remove well known pipe
    remove("main");
    printf("REMOVED WELL KNOWN PIPE\n");
    // connect to private fifo
    *to_client = open(msg, O_WRONLY);
    // send initial message
    int w = write(*to_client, msg, HANDSHAKE_BUFFER_SIZE);
    // get response from client
    r = read(read_end, msg, HANDSHAKE_BUFFER_SIZE);
    printf("CLIENT MESSAGE: %s\n", msg);
    // return fd to well known pipe
    printf("COMPLETED HANDSHAKE\n");
    printf("-------------------------\n");
    return read_end;
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
        printf("error %d: %s\n", errno, strerror(errno));
        return 1;
    }
    printf("CREATED PRIVATE PIPE\n");
    // set to_server to well known pipe
    *to_server = open("main", O_WRONLY);
    if (*to_server == - 1) {
        printf("error %d: %s\n", errno, strerror(errno));
        remove(pipe_name);
        return 1;
    }
    // sends private FIFO name to server
    int w = write(*to_server, pipe_name, HANDSHAKE_BUFFER_SIZE);
    printf("SENT PRIVATE PIPE NAME\n");
    // wait for response
    int read_end = open(pipe_name, O_RDONLY);
    // receive server message
    char msg[HANDSHAKE_BUFFER_SIZE];
    int r = read(read_end, msg, HANDSHAKE_BUFFER_SIZE);
    printf("SERVER MESSAGE: %s\n", msg);
    // remove private FIFO
    remove(pipe_name);
    printf("REMOVED PRIVATE PIPE\n");
    // send response to server
    w = write(*to_server, ACK, HANDSHAKE_BUFFER_SIZE);
    // return fd to private fifo
    return read_end;
}
