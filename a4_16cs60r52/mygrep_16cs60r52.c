#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <regex.h>

/******************************************************************
 * Filename: mygrep.c
 * Created by: Mohammad Luqman
 * Emulate the functionality of the grep function
 *
 * ****************************************************************/

extern int optind;
extern char* optarg;
int invflag = 0;
int maxcount = -1;

int handleoptions(int argc, char** argv) {
	int c;
	int option_index = 0;
	int digit_optind = 0;
	//int this_option_optind = optind ? optind : 1;
	struct option long_options[] = {
					{"invert-match", no_argument, 0, 'v'},
					{"max-count", required_argument, 0, 'm'}
				};
	while((c = getopt_long(argc, argv, "m:v", long_options,&option_index))!=-1) { 
		if(c=='?') {
			printf("mygrep: Invalid arguments\n");
			return 0;
		}
		switch(c) {
			case 'v':
							//printf("V\n");
							invflag = 1;
							break;
			case 'm':
							//printf("M %s\n", optarg);
							maxcount = atoi(optarg);
							break;
		}		
	}
	//if(c==-1) return 0;
	 return optind;
}

int matchregex(char* line, const char* pattern) {
	regex_t regex;
	int reti;
	int match = 1;
	char buffer[500];
	match = regcomp(&regex, pattern, REG_EXTENDED);
	match = regexec(&regex, line, 0, NULL, 0);
	if(match) {
		//printf("Match not found\n");
	}
	if(!match) ;//printf("Match found\n");
	else {
		regerror(match, &regex, buffer, sizeof(buffer));
		//fprintf(stderr, "Error: %s\n", buffer);
		//exit(1);
	}
	if(match == 0) return 1;
	else return 0;
}

int processfile(const char *pattern, char* filename, int pipeflag) {
	FILE *fp;
	if(!pipeflag) fp = fopen(filename, "r");
	if(pipeflag)  fp = stdin;
	char *buffer = NULL;
	size_t bufsize;
	int size;
	int regexmatch = 0;
	char *matchedline = NULL;
//	char *searchstring = pattern;
	//printf("Search string: %s\n", pattern);
	int i = 0, j = 0, linestoprint=100000;
	printf("Pattern: %s, Filename: %s\n", pattern, filename);
	if(maxcount>0) linestoprint = maxcount;
	if(!fp) {printf("File can't be opened\n"); return 0;}
	
	while((size = getline(&buffer, &bufsize, fp))>0) {
		buffer[size] = '\0';
		matchedline = strstr((const char*)buffer, pattern);
		//printf("%s ", buffer);
		regexmatch = matchregex(buffer, pattern);
		//printf("Regexmatch return: %d\n", regexmatch);
		if(regexmatch && !invflag) {
			printf("%d %s",i, buffer);
			i++;
		} 
		if(!regexmatch && invflag) {
			printf("%d %s",i,  buffer);
			i++;
		}
   if(i==linestoprint) break;
	}
	return 1;
}

/*int readfile(const char *pattern, char* filename) {
	FILE *fp = fopen(filename, "r");
	char *buffer = NULL;
	size_t bufsize;
	char *matchedline;
	int charsread;
	printf("Pattern received: %s\n", pattern);
	while((charsread = getline(&buffer, &bufsize, fp))>0) {
					buffer[charsread] = '\0';
					//buffer[charsread-1] = 'x';
				//	printf("%s %s", buffer, pattern);
					matchedline = strstr(buffer, pattern);
					if(matchedline!=NULL) printf("%s\n", buffer);
	}
}*/


int main(int argc, char **argv){
	int pipeflag = 0;
	char filename[200];
	if(argc<3) {
					pipeflag = 1;
	}
	if(argc<2) {
					printf("mygrep takes atleast 2 arguments\n"); 
					return 1;
	}
	int argvpos = handleoptions(argc, argv);
	//printf("optind : %d %s\n", argvpos, argv[optind]);
	if(argvpos) {
		//if(invflag) printf("Invert flag set\n");
		//if(maxcount>0) printf("Max flag set\n");
	}
	if(!pipeflag) strcpy(filename, argv[optind+1]);
	else strcpy(filename, "stdin");
	processfile((const char*)argv[optind], filename, pipeflag);
	if(argvpos == 0) perror("mygrep");
	return 0;
}

//Test Main function
/*int main(int argc, char **argv){
	if(argc<3) {
					printf("mygrep takes atleast 3 arguments\n"); 
					return 1;
	}
	int argvpos = handleoptions(argc, argv);
	printf("optind : %d %s\n", argvpos, argv[optind]);
	if(argvpos) {
		if(invflag) printf("Invert flag set\n");
		if(maxcount>0) printf("Max flag set\n");
	}
	//processfile((const char*)argv[optind], argv[optind+1]);
	readfile(argv[optind], argv[optind+1]);
	if(argvpos == 0) perror("mygrep");
	return 0;
}
*/
