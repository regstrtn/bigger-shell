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

int main(int argc, char **argv){
	setvbuf(stdin, NULL, _IONBF,1);
	setvbuf(stdout, NULL, _IONBF, 1);
	char c;
	while((c = getchar())!='\n') putchar(c);
	printf("Set unbuffered mode");
	return 0;
}
