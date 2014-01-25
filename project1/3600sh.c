#include "3600sh.h"

#define MAXLINE 200  // Constants
#define MAXARGS 20

#define USE(x) (x) = (x)

static char * getword(char * begin, char **end_ptr) {
  char * end = begin;
  while ( *begin == ' ' || *begin == '\t')
    begin++;  /* Get rid of leading spaces and tabs */
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
  return begin; /* This word is now a null-terminated string. return it. */
}

static void getargs(char cmd[], int *argcp, char *argv[]) {
  char *cmdp = cmd;
  char *end;
  int i = 0;

  /* fgets creates null-terminated string. stdin is pre-defined C constant
   *   for standard intput.  feof(stdin) tests for file:end-of-file.
	*/
  if (fgets(cmd, MAXLINE, stdin) == NULL) {
		do_exit();
	}
		
  while ( (cmdp = getword(cmdp, &end)) != NULL ) { /* end is output param */
    argv[i++] = cmdp;
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
			exit(0);
    }
		do_exit();
  }
	
	else //parent: in parent, childpid was set to pid of child process
		waitpid(childpid, NULL, 0);
	  //do_exit();
}

void interrupt_handler() {
  printf("\n^C was recognized. System will send a SIGINT to the child process.");
}

int main(int argc, char *argv[]) {
	USE(argc);
	USE(argv);
	setvbuf(stdout, NULL, _IONBF, 0); 
	
	void interrupt_handler();
  char cmd[MAXLINE] = "";
  char *childargv[MAXARGS];
  int childargc = 0;

	signal(SIGINT, interrupt_handler);
	
	char hostname[512] = "";
	gethostname(hostname, 512);
		
	while (!feof(stdin)) {
		
		printf("%s@%s:%s> ", getenv("USER"),hostname,getenv("PWD"));
		getargs(cmd, &childargc, childargv);
		
	  if ( childargc > 0 && strcmp(childargv[0], "exit") == 0 )
	    do_exit();
	  else
	    execute(childargv);
	}
	
	do_exit();
  return 0;
}

void do_exit() {
  printf("So long and thanks for all the fish!\n");
  exit(0);
}