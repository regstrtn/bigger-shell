#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
//#include <ncurses.h>
#include <curses.h>
/******************************************************************
 * Filename: myhis.c 
 * Created by: Mohammad Luqman
 *
 *
 * ****************************************************************/

typedef struct commands{
	char str[500];
	int i;
	struct commands* next;
	struct commands* prev;
} cmdnode;

cmdnode* readinput(cmdnode*);

char readchar() {
	struct termios term, term_orig;
	tcgetattr(0, &term_orig); //0 stands for STDIN_FILENO
	term = term_orig;
	term.c_lflag &= ~ICANON; //Set local control flag (c_lflag) to non-canonical input;
	term.c_lflag &= ~ECHO; //Turn echo off. Handle echo in the calling program
	tcsetattr(0, TCSANOW, &term); //Implement changes to STDIN_FILENO immediately
	char *c = (char*)malloc(sizeof(char));
	read(0, c, 1) ;//printf("%i\n", *c); //c holds char, 0 is STDIN, 1 is STDOUT
	tcsetattr(0, TCSANOW, &term_orig); //Set mode back to canonical after reading char
	return *c;
}

cmdnode* makebuffer(int n) {
	int i = 0;
	cmdnode *a, *b;
	cmdnode *head = (cmdnode *)malloc(sizeof(cmdnode));
	head->i = 0;
	a = b = head;
	memset(a->str, 0, sizeof(a->str));
	for(i=1;i<n;i++) {
			a->next = (cmdnode *)malloc(sizeof(cmdnode));
			a->next->prev = a;
			a->next->i = i;
			memset(a->next->str, 0, sizeof(a->next->str));
			a = a->next;
	}
	a->next = head;
	head->prev = a;
	for(i = 0;i<n+1;i++) {
		printf("Number: %d\n", b->i);
	 	b = b->next;
	}
	return head;
}

cmdnode* testlinklist(int n, cmdnode* historybuffer, cmdnode* historymarker);

//int main(int argc, char **argv){
	//char* commands[] = {"fire", "in", "the", "hole"};


int testmain(int argc, char **argv) {
//This is the main function that is getting called by the shell
	int i = 0;
	char* arrtoreturn = (char*)malloc(500*sizeof(char));
	cmdnode* historybuffer = makebuffer(10);
	cmdnode *historymarker;
	cmdnode *currcommandmarker;
	while(1) {
					historybuffer = readinput(historybuffer);
					//historybuffer = testlinklist(i, historybuffer, historymarker);
					printf("Curr comm printed from main: %s\n", historybuffer->prev->str);
					if(i++>15) break;
	}
	strcpy(arrtoreturn, historybuffer->prev->str);
	return 1;
}

cmdnode* testlinklist(int n,cmdnode* historybuffer, cmdnode* historymarker) {
	int i = 0;
	historymarker = historybuffer;
	historybuffer = historybuffer->next;
	for(i=0;i<5;i++){	
					printf("All commands: %s\n", historymarker->str);
					historymarker = historymarker->prev;
	}
	printf("Testlinklist called\n");
	char inp[100];
	scanf("%s", inp);
	strcpy(historybuffer->str, inp);
	return historybuffer->next;
}

cmdnode* readinput(cmdnode* historybuffer) {
	char c, arrow;
	int comlen = 0;
	char currentcommand[500];
	cmdnode* historymarker = historybuffer;
	memset(currentcommand, 0, sizeof(currentcommand));
		while((c=readchar())!='\n') {
			if(c==127) {
				printf("\b \b");
				currentcommand[strlen(currentcommand)-1] = '\0';
				//printf("Backspace pressed");
			}
			if(c == '\033') {
				readchar();
				arrow = readchar();
				switch(arrow) {
					case('A'): 
								printf("\033[1K\r"); //Clear everything to the beginning of the line and then display prompt;
								printf(">%s", historymarker->prev->str);
								strcpy(currentcommand, historymarker->prev->str);
								historymarker = historymarker->prev;
								while(historymarker->prev->str[0]=='\0') {
									historymarker = historymarker->prev;
								}
								break;
					case('B'):
								printf("\033[1K\r");
								printf(">%s", historymarker->next->str);
								strcpy(currentcommand, historymarker->next->str);
								historymarker = historymarker->next;
								while(historymarker->next->str[0]=='\0') {
									historymarker = historymarker->next;
								}
								break;
				}
			}
			else {
				printf("%c", c);
				comlen = strlen(currentcommand);
				currentcommand[comlen] = c;
				currentcommand[comlen+1] = '\0';
			}
			fflush(stdout);
		} 
		strcpy(historybuffer->str, currentcommand);
		//historybuffer = historybuffer->next;
		printf("Current command from read fn: %s\n", historybuffer->str);
		return historybuffer->next;
		//currentcommand[0] = '\0';	
} 
	
