//
// Created by oskhy on 12/21/2025.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // intializing the OpenSSL lib (TLS)
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) { ERR_print_errors_fp(stderr); exit(EXIT_FAILURE); }

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // creates the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from human readable string to machine readable bytes
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) { // change cp base on server ip address (18.218.236.166 for AWS EC2 public IP)
        printf("Invalid address / Address not supported\n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    printf("Connect to server\n");

    // Wrap socket with SSL
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    printf("TLS handshake successful with %s\n", SSL_get_cipher(ssl));

    
    // authenitcates user with server
    char name[50];
    printf("Enter a name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0; // removes \n from name
    char login_prefix[] = "LOGIN#";
    char temp[strlen(name) + strlen(login_prefix) + 1]; // plus 1 is for the null terminator
    strcpy(temp, login_prefix);
    strcat(temp, name);
    SSL_write(ssl, temp, strlen(temp));
    SSL_write(ssl, name, strlen(name));

    while (1) {
        // Send message to server
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin); //gets input from user keyboard
        buffer[strcspn(buffer, "\n")] = '\0'; // remove newline
        SSL_write(ssl, buffer, strlen(buffer));

        // Receive message from server
        //memset(buffer, 0, BUFFER_SIZE);
        //int valread = read(sock, buffer, BUFFER_SIZE);
        //if (valread <= 0) {
        //    printf("Server disconnected.\n");
        //    break;
        //}
        //printf("Server: %s\n", buffer);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    return 0;
}