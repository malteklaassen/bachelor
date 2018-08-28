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

#include <criu/criu.h>

int SERVER_PORT = 8081;

int main(){
    criu_init_opts();

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sock;
    if ( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Could not create listen socket.\n");
        return 1;
    }

    if ( (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0 ) {
        printf("Could not bind listen socket.\n");
        return 1;
    }

    //while ( 1 ) {
        int dir = open("/tmp/criu/server/", O_DIRECTORY);
        criu_set_tcp_established(true);
        criu_set_images_dir_fd(dir);
        criu_set_shell_job(true);
        criu_set_log_file("server.log");
        criu_set_log_level(4);

        criu_dump();

        printf("...restored.\n");
        

        char buffer[4096];
        int n;

        printf("Recving data...\n");
	struct sockaddr_in client;
	unsigned int slen = sizeof(client);
        n = recvfrom(sock, buffer, 4096, 0, (struct sockaddr*)&client, &slen);
        printf(" ...data recved\n");

        if ( n < 0 ) {
            printf("Could not read data from socket.\n");
            return 1;
        } else if ( n == 0 ) {
            printf("EOF.\n");
        } else {
            memset(buffer, 0, 4096);
            snprintf(buffer, 4096, "%ld", time(NULL));
            printf("Sending data...\n");
            sendto(sock, buffer, n, 0, (struct sockaddr*)&client, sizeof(client));
            printf(" ...data sent.\n");
        }


    //}

    return 0;
}
