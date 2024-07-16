#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>


//maximum application buffer (the data we would receive from the NIC we would store it in an array in a reserved memoey location with the following size in bytes)
#define APP_MAX_BUFFER 1024
#define PORT 8080

int main(){
    
    /*
    when the webserver starts to listen

    it would have a file descriptor, the file would contain
    the socket info

    info:
        port i'm listensing to
        ipv4 or ipv6
        addresses bind to

    which would give me a number ==> this is the server_df , 

    we will have a client_fd once there is an established connection to the socket

    we have only one server and we can have many client_fd
    */

    int server_fd,client_fd;

    struct sockaddr_in address;
    int address_len = sizeof(address);

    // this is where copy data from the os receive kernal   , this is for every connection 
    char buffer[APP_MAX_BUFFER] = {0};

    if((server_fd = socket(AF_INET,SOCK_STREAM,0)) == 0){
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; // ipv4
    address.sin_addr.s_addr = INADDR_ANY; // like listen 0.0.0.0 (all interafces wifi ethernet etc)
    address.sin_port = htons(PORT);

    if(bind(server_fd ,(struct sockaddr *)&address,sizeof(address)) < 0){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // until now we are not listening , until now we have a file descriptor points to the socket


    // now we will create send , accept , receive queues
    // when the kernal accept the connection it put it in the accept queue

    // [con1 , con2 , ... , 10]
    // now if the client(the backend app) doesn't accept the connection it would remain here and the kernal would not accept new connections to this queue to be accepted by the app
    if(listen(server_fd,10)<0){
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("waiting for a connection \n");


        // we are creating client_df which is == to a connection

        // think about it as a plug where you are plugging many clint_fd to the same server_fd
        // if the accept queue is empty we are stuck here
        if ((client_fd = accept(server_fd,(struct sockaddr *)&address,(socklen_t*)&address_len)) < 0){
        perror("accept failed");
        exit(EXIT_FAILURE);}



        // now we are reading what is in the connections
        // from the os receive buffer to the app buffer
        // here where we read the http 
        read(client_fd,buffer,APP_MAX_BUFFER);
        printf("%s\n" , buffer);

        //here where we check is it a get request or post

        // we would assume its valid and send back 200
        // we will right to the os send buffer
        char *http_response = "HTTP/1.1 200 OK\n"
                            "Contebt-Type: text/plain\n"
                            "Content-Length: 13\n\n"
                            "Hello world!\n";

        //send queue (os) , --> nic --->send
        write(client_fd , http_response,strlen(http_response));


        close(client_fd);
    }
    

    return 0;
}