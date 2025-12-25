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

#include <signal.h>


#define PORT 8080
#define BUFFER_SIZE 1024

// server lsitening sockets configs
int server_fd, client_socket;
struct sockaddr_in address;
int addrlen = sizeof(address);
char buffer[BUFFER_SIZE];

typedef struct
{
    int client_socket;
    SSL_CTX *ctx;
} threadArgs;


void *handleClient(void *args){
    threadArgs * targs = (threadArgs*)args;

    // Unpacks the data given by args
    int sock = targs->client_socket;  // get the actual socket
    char buffer[BUFFER_SIZE]; // where the message data is stored in

    SSL *ssl = SSL_new(targs->ctx);
    SSL_set_fd(ssl, sock); // Wrap socket with SSL

    // check if SSL is indeed wrapped to sockets
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(sock);
        free(targs); // free memory allocated for thread args
        return NULL;
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // once client sends message
        int valread =  SSL_read(ssl, buffer, BUFFER_SIZE);
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
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    return NULL;
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    // intializing the OpenSSL lib (TLS)
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    const SSL_METHOD *method = TLS_server_method(); // Server intialization for TLS
    SSL_CTX *ctx = SSL_CTX_new(method); // TLS configuration that is sepcial to this server, used for decryptina nd encrypting
    
    // debugging check
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Load certificate and private key
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
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

    // starts listening for any user client
    if (listen(server_fd, 3) < 0) { // n = number of clients can queue to connect to server
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("SERVER STARTED LISTENING\n");
    while(1){

        // Accept client
        if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
        } else {

            // setting up thread args (client socket, SSL)
            threadArgs *args = malloc(sizeof(threadArgs));
            args->client_socket = client_socket;
            args->ctx = ctx;

            // set client socket off to a threading
            pthread_t thread;
            pthread_create(&thread, NULL, handleClient, args);
            pthread_detach(thread);
            printf("Client connected and running a thread!\n");
        }
    }
    SSL_CTX_free(ctx);
    EVP_cleanup();
    close(server_fd);
    return 0;
}