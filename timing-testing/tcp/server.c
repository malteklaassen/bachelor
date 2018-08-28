#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <criu/criu.h>

int SERVER_PORT;

int main(int argc, char* argv[]){
    criu_init_opts();

    SERVER_PORT = atoi(argv[1]);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listen_sock;
    if ( (listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Server: Could not create listen socket.\n");
        return 1;
    }

    if ( (bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0 ) {
        printf("Server: Could not bind listen socket.\n");
        return 1;
    }

    int wait_size = 1;

    if ( listen(listen_sock, wait_size) < 0 ) {
        printf("Server: Could not open socket for listening.\n");
        return 1;
    }

    struct sockaddr_in client_addr;
    unsigned client_addr_len = 0;

    //while ( 1 ) {
        int sock;
        if ( ( sock = accept(listen_sock, (struct sockaddr*) &client_addr, &client_addr_len)) < 0 ) {
            printf("Server: Could not open accept socket.\n");
            return 1;
        }

        printf("Server: Client connected with addr %s.\n", inet_ntoa(client_addr.sin_addr));
        
        int dir = open("/tmp/criu/server/", O_DIRECTORY);
        criu_set_tcp_established(true);
        criu_set_images_dir_fd(dir);
        criu_set_shell_job(true);
        criu_set_log_file("server.log");
        //criu_set_log_level(4);

        criu_dump();

        printf("Server: ...restored.\n");
        

        char buffer[4096];
        int n;

        printf("Server: Recving data...\n");
        n = recv(sock, buffer, 4096, 0);
        printf("Server:  ...data recved\n");

        if ( n < 0 ) {
            printf("Server: Could not read data from socket.\n");
            return 1;
        } else if ( n == 0 ) {
            printf("Server: EOF.\n");
        } else {
            memset(buffer, 0, 4096);
            snprintf(buffer, 4096, "%ld", time(NULL));
            printf("Server: Sending data...\n");
            send(sock, buffer, n, 0);
            printf("Server:  ...data sent.\n");
        }

        close(sock);

    //}

    close(listen_sock);
    return 0;
}
