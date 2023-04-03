#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int clients[MAX_CLIENTS] = {0};
int num_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_handler(void *arg) {
    int client_fd = *((int *) arg);
    char buffer[BUFFER_SIZE];
    int valread;

    while ((valread = read(client_fd, buffer, BUFFER_SIZE)) > 0) {
		printf("%s", buffer);
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < num_clients; i++) {
            if (clients[i] != client_fd) {
                send(clients[i], buffer, valread, 0);
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_clients; i++) {
        if (clients[i] == client_fd) {
            clients[i] = 0;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(client_fd);
    return NULL;
}

int main(int argc, char *argv[]) {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    	perror("socket failed");
    	exit(EXIT_FAILURE);
	}

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if ((client_fd = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&mutex);
        if (num_clients < MAX_CLIENTS) {
            clients[num_clients] = client_fd;
            num_clients++;
            pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, client_handler, (void *) &client_fd) < 0) {
                perror("pthread_create failed");
                exit(EXIT_FAILURE);
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    return 0;
}
