#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <criu/criu.h>

int main(){
    criu_init_opts();

    const char * server_name = "localhost";
    const int server_port = 8080;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    inet_pton(AF_INET, server_name, &server_addr.sin_addr);

    server_addr.sin_port = htons(server_port);

    int sock;
    if ( ( sock = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
        printf("Could not create socket.\n");
        return 1;
    }

    if ( connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ) {
        printf("Could not connect to server.\n");
        return 1;
    }

    int dir = open("/tmp/criu/client/", O_DIRECTORY);
    criu_set_tcp_established(true);
    criu_set_images_dir_fd(dir);
    criu_set_shell_job(true);
    criu_set_log_file("client.log");
    criu_set_log_level(4);

    criu_dump();

    printf("...restored.\n");

    char buffer[4096] = "This is some data to be send to the server.\n";

    printf("Sending data...");
    send(sock, buffer, strlen(buffer), 0);
    printf(" ... data sent.\n");

    printf("Recving data...");
    int n = recv(sock, buffer, 4096, 0);
    printf(" ... data recved\n");
    if ( n < 0 ) {
        printf("Could not read data from socket.\n");
            return 1;
    } else if ( n == 0 ) {
        printf("EOF.\n");
    } else {
        write(STDOUT_FILENO, buffer, n);
    }
    close(sock);
    return 0;
}
