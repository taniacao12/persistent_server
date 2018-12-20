#include "pipe_networking.h"

/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
    // create well known pipe
    int wkp = mkfifo("main", 0644);
    if (wkp == - 1) // errno message
        printf("-------------------------\nERROR: %d --> %s\n-------------------------\n", errno, strerror(errno));
    else printf("CREATED WELL KNOWN PIPE\n");
    // wait
    int receive = open("main", O_RDONLY);
    // receive message
    char msg[HANDSHAKE_BUFFER_SIZE];
    int r = read(receive, msg, HANDSHAKE_BUFFER_SIZE);
    printf("RECIEVED MESSAGE FROM CLIENT\nCLIENT MESSAGE: %s\n", msg);
    // remove well known pipe
    remove("main");
    printf("REMOVED WELL KNOWN PIPE\n");
    // connect to private FIFO
    *to_client = open(msg, O_WRONLY);
    // send initial message
    int w = write(*to_client, ACK, HANDSHAKE_BUFFER_SIZE);
    // get response from client
    r = read(receive, msg, HANDSHAKE_BUFFER_SIZE);
    printf("RECIEVED MESSAGE FROM CLIENT\nCLIENT MESSAGE: %s\n", msg);
    // return fd to wkp
    printf("3 WAY HANDSHAKE COMPLETE\n");
    return receive;
}

/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
    // create private fifo
    char pipe_name[HANDSHAKE_BUFFER_SIZE];
    sprintf(pipe_name, "%d", getpid());
    int private = mkfifo(pipe_name, 0644);
    printf("CREATED PRIVATE PIPE\n");
    // set to_server to well known pipe
    *to_server = open("main", O_WRONLY);
    if (*to_server == - 1) // errno message
        printf("-------------------------\nERROR: %d --> %s\n-------------------------\n", errno, strerror(errno));
    // send private FIFO to server
    int w = write(*to_server, pipe_name, HANDSHAKE_BUFFER_SIZE);
    if (w == - 1) // errno message
        printf("-------------------------\nERROR: %d --> %s\n-------------------------\n", errno, strerror(errno));
    else printf("SENT PRIVATE PIPE TO SERVER\n");
    // wait for response
    int receive = open(pipe_name, O_RDONLY);
    // receive server message
    char msg[HANDSHAKE_BUFFER_SIZE];
    int r = read(receive, msg, HANDSHAKE_BUFFER_SIZE);
    printf("RECIEVED MESSAGE FROM SERVER\SERVER MESSAGE: %s\n", msg);
    // remove private FIFO
    remove(pipe_name);
    printf("REMOVED PRIVATE PIPE\n");
    // send response to server
    w = write(*to_server, ACK, HANDSHAKE_BUFFER_SIZE);
    // return fd to private fifo
    return receive;
}