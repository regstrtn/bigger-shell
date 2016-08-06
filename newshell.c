#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#define PROMPT '>'
#define CMDLEN 1250
#define MAXNUMTOKENS 100

/**********************************************
 * Filename: shell.c
 * Created by: Mohammad Luqman
 * This is the main file that implements a basic shell with the following bash shell equivalent operations: 
 * ls, mv, pwd, rm, cd, ps, tail, mkdir
 * To exit, type "exit".
 ********************************************/

void setpathvar() {
	/*Set path variable so commands can be used from anywhere*/
	char *pathname;
	char curdir[500],  newpath[1500];
	getwd(curdir);
	pathname = getenv("PATH");
	strcpy(newpath, pathname);
	strcat(newpath, ":");
	strcat(newpath, curdir);
	setenv("PATH", newpath, 1);
}


char *shellfn[] = {"mycd","exit"};
int numshellfn = 2;

//Implement the shell exit function
int myexit(char **args) {
	exit(0);
}

//Change directory function
int mycd(char **args) {
	if(args[1] == NULL) {
			perror("mycd");
	}
	else {
					chdir(args[1]);
	}
	return 1;
}

//Execute commands with arguments
int cmd_execute(char** args) {
	pid_t pid;
	int status;
	int i=0;
	if(args[0]==NULL) {
					return 1;
	}
	for(i=0;i<numshellfn;i++) {
		if(strcmp(args[0], shellfn[i])==0) {
			if(i == 0) mycd(args);
			else if(i==1) myexit(args);
			return 1;
		}
	}

	if((pid = fork())==0) {
	//printf("%s\n", args[0]);
		if(execvp(args[0], args)==-1) {
			perror("Error");
		}
	exit(0);
	}
	else wait(&status);
	return 1;
}

//Tokenize user input
char** split_into_tokens(char* str) {
	/*This function takes a string as input and outputs tokens as command or parameter*/
	char* pch;
	char* delim = " \t\n\r";
	int pos = 0;
	char** tokens = (char **)malloc(MAXNUMTOKENS*sizeof(char*));
	pch = strtok(str, delim);
	while(pch!=NULL) {
		tokens[pos] = pch;
		//printf("%s\n", pch);
		pch = strtok(NULL, delim);
		pos++;
	}
	tokens[pos] = NULL;
	return tokens;
}

//Read user input from stdin
char* read_user_input() {
				char *inputline = NULL;
				size_t buffersize = 0;
				getline(&inputline, &buffersize, stdin);
				return inputline;
}

int main() {
	char *cmdinput;
	char **args;
	int retval; 
	char *debugarg[3] = {"./mypwd", "mypwd", NULL};
	setpathvar();
	while(1) {
			printf("%c", PROMPT);
			cmdinput = read_user_input();
			args 		 = split_into_tokens(cmdinput);
			//printf("%s\n", args[0]);
			retval 	 = cmd_execute(args);
		}
}
