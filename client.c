#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

void *receive_handler(void *arg) {
    int server_fd = *((int *) arg);
    char buffer[BUFFER_SIZE];
    int valread;

    while ((valread = read(server_fd, buffer, BUFFER_SIZE)) > 0) {
        printf("%s\n", buffer);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    int server_fd;
    struct sockaddr_in address;
    char username[20];

    printf("Enter your username: ");
    scanf("%s", username);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));

    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }

    if (connect(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, receive_handler, (void *) &server_fd) < 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }

    char message[BUFFER_SIZE];
    while (fgets(message, BUFFER_SIZE, stdin)) {
        char formatted_message[BUFFER_SIZE];

        sprintf(formatted_message, "%s: %s", username, message);
        send(server_fd, formatted_message, strlen(formatted_message), 0);
    }

    close(server_fd);
    return 0;
}
