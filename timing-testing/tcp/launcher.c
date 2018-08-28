#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    //fork, start & dump the processes
    int serverpid, clientpid;

    serverpid = fork();
    if ( serverpid == 0 ){
        char servercall[100];
        sprintf(servercall, "./server %s", argv[1]);
        system(servercall);
        exit(0);
    }

    clientpid = fork();
    if ( clientpid == 0 ){
        char clientcall[100];
        sprintf(clientcall, "./client %s", argv[1]);
        system(clientcall);
        exit(0);
    }

    // wait for the processes to get saved
    waitpid(serverpid, NULL, 0);
    waitpid(clientpid, NULL, 0);


    // determine the server- & clientport for the netfilter rules (they are written to a file by the server and client before the dump)
    int clientport, serverport;

    FILE* portfp = fopen("/tmp/criu/clientport", "r");
    fscanf(portfp, "%i", &clientport);
    fclose(portfp);

    portfp = fopen("/tmp/criu/serverport", "r");
    fscanf(portfp, "%i", &serverport);
    fclose(portfp);

    // create the netfilter rules & iptables calls

    char iptables1[200], iptables2[200], iptables3[200], iptables4[200];
    sprintf(iptables1, "iptables -A OUTPUT -s 127.0.0.1/32 -d 127.0.0.1/32 -p tcp -m mark ! --mark 0xc114 -m tcp --sport %i --dport %i -j DROP", clientport, serverport);
    sprintf(iptables2, "iptables -A OUTPUT -s 127.0.0.1/32 -d 127.0.0.1/32 -p tcp -m mark ! --mark 0xc114 -m tcp --sport %i --dport %i -j DROP", serverport, clientport);
    sprintf(iptables3, "iptables -A INPUT -s 127.0.0.1/32 -d 127.0.0.1/32 -p tcp -m mark ! --mark 0xc114 -m tcp --sport %i --dport %i -j DROP", clientport, serverport);
    sprintf(iptables4, "iptables -A INPUT -s 127.0.0.1/32 -d 127.0.0.1/32 -p tcp -m mark ! --mark 0xc114 -m tcp --sport %i --dport %i -j DROP", serverport, clientport);


    // restore $ITERATIONS times
    for(int i = 0; i < 200; i++){
        printf("Iteration: %i\n", i);
        serverpid = 0, clientpid = 0;
        
        serverpid = fork();
        if ( serverpid == 0 ){
            //restore the server
            system("criu restore --tcp-established --shell-job --images-dir /tmp/criu/server");
            exit(0);
        }

        clientpid = fork();
        if ( clientpid == 0 ){
            //restore the client
            system("criu restore --tcp-established --shell-job --images-dir /tmp/criu/client");
            exit(0);
        }

        // wait for them to terminate
        waitpid(serverpid, NULL, 0);
        waitpid(clientpid, NULL, 0);
        
        //add the netfilter rules
        system(iptables1);
        system(iptables2);
        system(iptables3);
        system(iptables4);
    }
}
