#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <criu/criu.h>
#include <fcntl.h>

int main(){
	criu_init_opts();
	char str[200];

	// if token exists
	if( access("/tmp/criu/token", F_OK) == 0 ){
		// restore in place
		int dir = open("/tmp/criu/afl/", O_DIRECTORY);
		criu_set_images_dir_fd(dir);
		criu_set_shell_job(true);
		criu_set_log_file("fuzz.log");
		criu_set_log_level(4);

		criu_restore_child();
	}else{
		FILE* t = fopen("/tmp/criu/token", "w");
		fclose(t);

		// cp w/o dump
		int dir = open("/tmp/criu/afl/", O_DIRECTORY);
		criu_set_images_dir_fd(dir);
		criu_set_shell_job(true);
		criu_set_log_file("dump.log");
		criu_set_leave_running(true);
		criu_set_log_level(4);

		criu_dump();
		// do stuff
		int l = read(STDIN_FILENO, str, 200);
		//int fifo = open("/tmp/criu/out", 0);
		//write(fifo, str, l);
		//close(fifo);
		if(l >= 3 && str[0] == 'F' && str[1] == 'U' && str[2] == 'Z' && str[3] == 'Z'){
			system("touch /tmp/criu/FAILED");
			free(str);
		}
		return 0;
	}
}
