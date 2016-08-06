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
 * Filename: 
 * Created by: Mohammad Luqman
 *
 *
 * ****************************************************************/

void writer(const char* message, FILE* stream, char**argv) {
	execvp("ls", argv);
	fprintf(stream, "%s\n", message);
	fflush(stream);
}

void reader(FILE *stream) {
	char buffer[1024];
	while(!feof(stream) && fgets(buffer, sizeof(buffer), stream)!=NULL){ 
		fputs(buffer, stdout);
	}
}

int main(int argc, char **argv) {

	int fd[2];
	pipe(fd);
	pid_t childpid;
	if((childpid = fork())==0) { //Then this is the child process
	close(fd[0]);
	FILE *stream;
	char **argarr;
	argarr=++argv;
	dup2(fd[1], STDOUT_FILENO);
	execvp(argv[1], argv);
	//writer("hello world", stream, argv);
	close(fd[1]);
	}
	else {  //Then this is the parent process
	close(fd[1]);
	FILE *stream;
	stream=fdopen(fd[0], "r");
	reader(stream);
	close(fd[0]);
	}
	return 0;
}
