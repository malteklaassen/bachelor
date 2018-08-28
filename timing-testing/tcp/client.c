#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <criu/criu.h>

int SERVER_PORT;

int main(int argc, char* argv[]){
    criu_init_opts();

    const char * server_name = "localhost";
    SERVER_PORT = atoi(argv[1]);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    inet_pton(AF_INET, server_name, &server_addr.sin_addr);

    server_addr.sin_port = htons(SERVER_PORT);

    int sock;
    if ( ( sock = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
        printf("Client: Could not create socket.\n");
        return 1;
    }

    if ( connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ) {
        printf("Client: Could not connect to server.\n");
        return 1;
    }

    struct sockaddr_in local_address;
    unsigned int addr_size = sizeof(local_address);
    getsockname(sock, (struct sockaddr*)&local_address, &addr_size);
    FILE* clientportfp = fopen("/tmp/criu/clientport", "w");
    fprintf(clientportfp, "%i", ntohs(local_address.sin_port));
    fclose(clientportfp);

    FILE* serverportfp = fopen("/tmp/criu/serverport", "w");
    fprintf(serverportfp, "%i", ntohs(server_addr.sin_port));
    fclose(serverportfp);




    int dir = open("/tmp/criu/client/", O_DIRECTORY);
    criu_set_tcp_established(true);
    criu_set_images_dir_fd(dir);
    criu_set_shell_job(true);
    criu_set_log_file("client.log");
//    criu_set_log_level(4);

    criu_dump();

    printf("Client: ...restored.\n");

    char buffer[4096] = "This is some data to be send to the server.\n";

    printf("Client: Sending data...\n");
    send(sock, buffer, strlen(buffer), 0);
    printf("Client: ... data sent.\n");

    printf("Client: Recving data...\n");
    int n = recv(sock, buffer, 4096, 0);
    printf("Client: ... data recved\n");
    if ( n < 0 ) {
        printf("Client: Could not read data from socket.\n");
            return 1;
    } else if ( n == 0 ) {
        printf("Client: EOF.\n");
    } else {
        write(STDOUT_FILENO, buffer, n);
	printf("\n");
    }
    close(sock);
    return 0;
}
