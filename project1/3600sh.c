#include "3600sh.h"

#define MAXLINE 200  /* This is how we declare constants in C */
#define MAXARGS 20

#define USE(x) (x) = (x)

static char * getword(char * begin, char **end_ptr) {
  char * end = begin;
  while ( *begin == ' ' )
    begin++;  /* Get rid of leading spaces. */
    end = begin;
	
  while ( *end != '\0' && *end != '\n' && *end != ' ' && *end != '#')
    end++;  /* Keep going. */
  
  if ( end == begin )
    return NULL;  /* if no more words, return NULL */
    *end = '\0';  /* else put string terminator at end of this word. */
    *end_ptr = end;
  if (begin[0] == '$') { /* if this is a variable to be expanded */
    begin = getenv(begin+1); /* begin+1, to skip past '$' */
    if (begin == NULL) {
	  perror("getenv");
	  begin = "UNDEFINED";
    }
  }
  return begin; /* This word is now a null-terminated string.  return it. */
}

static void getargs(char cmd[], int *argcp, char *argv[]) {
  char *cmdp = cmd;
  char *end;
  int i = 0;

  /* fgets creates null-terminated string. stdin is pre-defined C constant
   *   for standard intput.  feof(stdin) tests for file:end-of-file.
   */
  if (fgets(cmd, MAXLINE, stdin) == NULL && feof(stdin)) {
    printf("Couldn't read from standard input. End of file? Exiting ...\n");
    do_exit();
  }
  while ( (cmdp = getword(cmdp, &end)) != NULL ) { /* end is output param */
    argv[i++] = cmdp;
    /* "end" brings us only to the '\0' at end of string */
	cmdp = end + 1;
    }
    argv[i] = NULL; /* Create additional null word at end for safety. */
    *argcp = i;
}

static void execute (char *argv[]) {
  pid_t childpid;
  childpid = fork();
  if (childpid == -1) {
    perror("fork");
    printf("Failed to execute command.\n");
  }
  if (childpid == 0) {
    if (-1 == execvp(argv[0], argv)) {
      printf("Error: Command not found.\n");
    }
    do_exit();
  }
    else //parent: in parent, childpid was set to pid of child process
      waitpid(childpid, NULL, 0);
  
  return;
}

void interrupt_handler() {
  printf("\n^C was recognized. System will send a SIGINT to the child process.");
}

int main(int argc, char *argv[]) {
	USE(argc);
	USE(argv);
	setvbuf(stdout, NULL, _IONBF, 0); 

    void interrupt_handler();

    char cmd[MAXLINE];
    char *childargv[MAXARGS];
    int childargc;

	signal(SIGINT, interrupt_handler);
	
	char hostname[1024];
	gethostname(hostname, 1024);

    while (1) {
	  printf("%s@%s:%s> ", getenv("USER"),hostname,getenv("PWD"));
	  fflush(stdout); /* flush from output buffer to terminal itself */
	  getargs(cmd, &childargc, childargv);
	
	  if ( childargc > 0 && strcmp(childargv[0], "exit") == 0 )
	    do_exit();
      else
	    execute(childargv);
      }
	return 0;
}

void do_exit() {
  printf("So long and thanks for all the fish!\n");
  exit(0);
}