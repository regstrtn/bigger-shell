#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>


/******************************************************************
 * Filename: Background Process test file
 * Created by: Mohammad Luqman
 *
 *
 * ****************************************************************/

int main(int argc, char **argv){
	pid_t pid;
	int n;
	int status;
	char *lastarg = argv[argc-1];
	int bg = 0;
	if(strcmp(lastarg, "&")==0) bg = 1;
	if((pid=fork())==0) {
		if(bg) setpgid(0, 0);	
		scanf("%d", &n);
		execvp("gedit", argv);
		exit(0);
	}
	else {
		if(bg) waitpid(pid, &status, WNOHANG);
		else waitpid(pid, &status, 0);
		printf(">");
	}
	return 0;
}
