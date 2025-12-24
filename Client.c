//
// Created by oskhy on 12/21/2025.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
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
    if (inet_pton(AF_INET, "18.218.219.40", &serv_addr.sin_addr) <= 0) { // change cp base on server ip address
        printf("Invalid address / Address not supported\n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    printf("Connect to server at\n");

    while (1) {
        // Send message to server
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin); //gets input from user keyboard
        buffer[strcspn(buffer, "\n")] = '\0'; // remove newline
        send(sock, buffer, strlen(buffer), 0);

        // Receive message from server
        //memset(buffer, 0, BUFFER_SIZE);
        //int valread = read(sock, buffer, BUFFER_SIZE);
        //if (valread <= 0) {
        //    printf("Server disconnected.\n");
        //    break;
        //}
        //printf("Server: %s\n", buffer);
    }

    close(sock);
    return 0;
}