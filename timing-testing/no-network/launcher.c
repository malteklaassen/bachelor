#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// First argument: memory to be used
// Second argument: number of iterations

int main(int argc, char* argv[]){
    //fork, start & dump the processes
    int clientpid;

    clientpid = fork();
    if ( clientpid == 0 ){
        char clientcall[100];
        sprintf(clientcall, "./client %s", argv[1]);
        system(clientcall);
        exit(0);
    }

    // wait for the processes to get saved
    waitpid(clientpid, NULL, 0);

    int iterations = atoi(argv[2]);
    // restore $ITERATIONS times
    for(int i = 0; i < iterations; i++){
        printf("Iteration: %i\n", i);
        clientpid = 0;

        clientpid = fork();
        if ( clientpid == 0 ){
            //restore the client
            system("criu restore --shell-job --images-dir /tmp/criu/client");
            exit(0);
        }

        // wait for them to terminate
        waitpid(clientpid, NULL, 0);
    }
}
