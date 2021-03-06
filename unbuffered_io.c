#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int main(int argc, char *argv[], char *envp[]) {
  int fd[2];

  if(pipe(fd) < 0) { 
    printf("Can\'t create pipe\n");
    exit(-1); 
  }

  struct termios term, term_orig;

  if(tcgetattr(0, &term_orig)) {
    printf("tcgetattr failed\n");
    exit(-1); 
  }

  term = term_orig;

  term.c_lflag &= ~ICANON;
  term.c_lflag |= ECHO;
  term.c_cc[VMIN] = 0;
  term.c_cc[VTIME] = 0;

  if (tcsetattr(0, TCSANOW, &term)) {
    printf("tcsetattr failed\n");
    exit(-1);
  }

  pid_t fpid = fork();
  if (fpid == 0) {
    close(0);
    close(fd[1]);
    char *s = (char *) malloc(sizeof(char));
    while(1) if (read(fd[0], s, 1)) printf("%i\n", *s);
  }
  close(fd[0]);
  char *c = (char *) malloc(sizeof(char));
  while (1) {
    if (read(0, c, 1) > 0) write(fd[1], c, 1);
  }
  return 0;
} 