#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// threading lib
#include <pthread.h>

// TLS encryption
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// server lsitening sockets configs
int server_fd, client_socket;
struct sockaddr_in address;
int addrlen = sizeof(address);
char buffer[BUFFER_SIZE];

void *handleClient(void *clientInputSocket){

    int sock = *((int *)clientInputSocket);  // get the actual socket
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // once client sends message
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        printf("Client: %s\n", buffer);

        // Send message to client
        //printf("You: ");
        //fgets(buffer, BUFFER_SIZE, stdin);
        //buffer[strcspn(buffer, "\n")] = '\0'; // remove newline
        //send(sock, buffer, strlen(buffer), 0);
    }
}

int main() {
    
    // intializing the OpenSSL lib (TLS)
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    const SSL_METHOD *method = TLS_server_method(); // Server intialization for TLS
    SSL_CTX *ctx = SSL_CTX_new(method); // TLS configuration that is sepcial to this server, used for decryptina nd encrypting
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }



    // creates the socket for server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("SOCKET FAILED:SERVER");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind server port to address to listen or client port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("CONNECTING SOCKET TO ADDRESS FAILED");
        exit(EXIT_FAILURE);
    }

    while(1){
        // starts listening for any user client
        if (listen(server_fd, 3) < 0) { // n = number of clients can queue to connect to server
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        printf("SERVER STARTED LISTENING\n");

        // Accept client
        if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        } else {
            // creates malloc for own memeory space for client_socket
            int *pclient = malloc(sizeof(int));
            *pclient = client_socket;

            // set client socket off to a threading
            pthread_t thread;
            pthread_create(&thread, NULL, handleClient, pclient);
            pthread_detach(thread);
            printf("Client connected and running a thread!\n");
        }
    }

    close(server_fd);
    return 0;
}