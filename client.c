#include "pipe_networking.h"


int main() {
    int to_server;
    int from_server;

    from_server = client_handshake( &to_server );
    char data[BUFFER_SIZE];
    while (1) {
        // get input from user
        printf("-------------------------\nENTER MESSAGE: ");
        fgets(data, BUFFER_SIZE, stdin);
        // send message to server
        int w = write(to_server, data, BUFFER_SIZE);
        // get reponse from server
        int r = read(from_server, data, BUFFER_SIZE);
        // display response
        printf("SERVER MESSAGE: %s", data);
    }
}
