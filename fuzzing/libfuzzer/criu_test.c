#include <criu/criu.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size){
    criu_init_opts();

    if( access( "/tmp/criu/fuzz/token", F_OK) != 0 ){
        int dir = open("/tmp/criu/fuzz/", O_DIRECTORY);

        FILE* f = fopen("/tmp/criu/fuzz/token", "w");
        fclose(f);

        int input = open("/tmp/criu/fuzz/input", O_CREAT | O_WRONLY );
        FILE* sinput = fopen("/tmp/criu/fuzz/sinput", "w");
        write(input, Data, Size);
        fprintf(sinput, "%d\n", Size);
        fclose(sinput);
        close(input);

        criu_set_images_dir_fd(dir);
        criu_set_shell_job(true);
        criu_set_log_file("client.log");
        criu_set_log_level(4);
        criu_set_leave_running(true);

        criu_dump();

        size_t size; 
        uint8_t *data;

        input = open("/tmp/criu/fuzz/input", O_RDONLY);
        sinput = fopen("/tmp/criu/fuzz/sinput", "r");
        fscanf(sinput, "%d\n", &size);
        printf("%d\n", size);
        data = malloc(size);
        read(input, data, size);
        fclose(sinput);
        close(input);
    
        printf("Hello World\n");
        write(STDOUT_FILENO, data, size);
//        printf("%s\n", Data);
        return 0;
    }else{
        int dir = open("/tmp/criu/fuzz/", O_DIRECTORY);

       int input = open("/tmp/criu/fuzz/input", O_CREAT | O_WRONLY);
       FILE* sinput = fopen("/tmp/criu/fuzz/sinput", "w");
       write(input, Data, Size);
       fprintf(sinput, "%d\n", Size);
       fclose(sinput);
       close(input);

        criu_set_images_dir_fd(dir);
        criu_set_shell_job(true);
        criu_set_log_file("client.log");
        criu_set_log_level(4);

        criu_restore_child();


    return 0;
    }
}
