#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>

/******************************************************************
 * Filename: Parse input lines 
 * Created by: Mohammad Luqman
 * Input: One stream terminated by the newline character
 * Output: For each input, in loop, array of character pointers
 *
 *
 * ****************************************************************/

typedef struct command {
 int argc;
 char *argv[50];
 char cmdstring[1000];
}cmd;

int numpipes(char* inp) {
		int n1 = 0;
		int i = 0;
		while(inp[i]!='\0') {
			if(inp[i] == '|') n1++;
			i++;
		}
		return n1;
}


cmd *splitintoargs(int argc, char *inp) {
	cmd *cmdarr;
	int i = 0, j = 0, k = 0, l = 0;
	int n1, n2;
	char *token;
	char *s2 = " \t\n\r";
	char *s1 = "|";
	n1 = 0;
	char *inpbuffer = (char*)malloc(strlen(inp)+1);
	strcpy(inpbuffer, inp);
	while(inp[i]!='\0') {
		if(inp[i] == '|') n1++;
		i++;
	}
	i = 0;
	cmdarr = (cmd*)malloc(sizeof(cmd)*n1);
	token = strtok(inpbuffer, s1);
	while(token!=NULL) {
		strcpy((cmdarr+i)->cmdstring, token);
		printf("cmdstring: %s\n", (cmdarr+i)->cmdstring);
		token = strtok(NULL, s1);
		i++;
	}
	for(i=0;i<=n1;i++) {
		j = 0;
		token = strtok((cmdarr+i)->cmdstring, s2);
		while(token!=NULL) {
			//(cmdarr+i)->argv[j] = (char*)malloc(strlen(token+1));
			//strcpy((cmdarr+i)->argv[j], token);
			(cmdarr+i)->argv[j] = token;	
			token = strtok(NULL, s2);
			printf("cmd string tokens: %s\n", (cmdarr+i)->argv[j]);
			j++;
		}
			(cmdarr+i)->argc = j;
			(cmdarr+i)->argv[j] = NULL;
		  (cmdarr+i)->argv[j] = NULL; //I think this is not required
	} return cmdarr;
}


char* read_user_input() {
	char *buffer = NULL;
	size_t bufsize = 0;
	getline(&buffer, &bufsize, stdin);
	return buffer;
}

int create_process(int in, int out, cmd* cmdarr) {
	pid_t pid;
	if((pid=fork())==0) {
		if(in!=0) { //One of the mid chain processes, direct their stdin to pipe
			dup2(in, 0);
			close(in);
		}
		if(out!=1) { //Direct stdout to pipe
			dup2(out, 1);
			close(out);
		}
		return execvp(cmdarr->argv[0], (char* const*)cmdarr->argv);
	}
	return pid;
}

int runchains(int pipes, cmd* cmdarr) {
 printf("Runchains called\n"); 
 int i;
 pid_t pid;
 int in, fd[2];
 in = 0; //Set first process input as stdin
 for(i=0;i<pipes;i++) {
		pipe(fd);
		create_process(in, fd[1], cmdarr+i);
		close(fd[1]); //The child process will write to pipe
		in = fd[0]; //Set in for the input of next process
 }
 if(in!=0) dup2(in, 0);
 return execvp((cmdarr+i)->argv[0], (char * const*)cmdarr[i].argv);
}


int cmd_execute(int pipes, cmd* cmdarr) {
	pid_t pid;
	int status;
	int bkg = 0;
	if(cmdarr->argv[0]==NULL) { perror("No arguments"); return 1;}
	if(strcmp(cmdarr->argv[cmdarr->argc-1], "&")==0) {
				 	bkg = 1;
					cmdarr->argv[cmdarr->argc-1] = NULL;
	}
	printf("Number of pipes: %d\n", pipes);
	if((pid = fork())==0) { //This is the child
					if(pipes>0) runchains(pipes,cmdarr);
					printf("Command to execute: %s\n", cmdarr->argv[0]);
					if(bkg) setpgid(0, 0);
					if(execvp(cmdarr[0].argv[0], (char* const*)cmdarr[0].argv)==-1) perror("Execvp: ");
					exit(0);
					
	}
	else { //Parent
					usleep(10000);
					if(!bkg) wait(&status);
					if(bkg) waitpid(pid, &status, WNOHANG);
	}
	return 0;
}

int main(int argc, char **argv){
  char *cmdinput;
	cmd *cmdarr;
	int pipes;
	int retval = 1;
	while(1) {
		printf(">");
		cmdinput = read_user_input();
		pipes = numpipes(cmdinput);
		cmdarr = splitintoargs(pipes, cmdinput);
		retval = cmd_execute(pipes, cmdarr);
	}
	return 0;
}

/*
int parseinputs(int argc, char *inputline) {
	char p = '|';
	char sp = ' ';
	int prev, next, i =0, num = 0;
	char *copyline = inputline;
 	strcpy(copyline, inputline);
	char *next1, next2;
	cmd cmdarr[num];
	prev = 0;
	i = 0;
	next1 = strchr(copyline, '|');
	printf("%s\n", next1);
	strcpy(cmdarr[0].cmdstring, "Hello world");
	printf("%s\n", cmdarr[0].cmdstring);
}*/
