# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>
int main () {
  int pipe1 [2]; // Parent to child
  int pipe2 [2]; // Child to parent
  pid_t pid;
  // TODO : Create both pipes
  
  printf("Successfully started.\n");

  // if ( pipe ( pipe1 ) == -1) { /* error handling */ }
  if (pipe(pipe1) == -1) {
    fprintf(stderr, "Pipe 1 could not be created\n");
    return -1;
  }
  if (pipe(pipe2) == -1) {
    fprintf(stderr, "Pipe 2 could not be created\n");
    return -1;
  }
  
  // TODO : Fork process
  
    pid = fork();
    printf("Pid of process: %d\n", pid);
    if (pid < 0)
    {
      fprintf(stderr, "Fork failed\n");
      return 1;
    }
  if (pid == 0) {
      // Child process
      printf("I'm the child process!\n");
      // TODO : Close unused pipe ends
      // close (pipe1 [1]) ; // Close write end of pipe1
      close(pipe1[1]);
      // close (pipe2 [0]) ; // Close read end of pipe2
      close(pipe2[0]);
      // TODO : Read from parent, send responses
      char *buffer = malloc(4096);
      if (!buffer) { fprintf(stderr, "Buffer wasn't created"); }
      int line_count = 0;
      int char_count = 0;
      char last_char;
      
      ssize_t n;
	while((n = read(pipe1[0], buffer, 4096)) > 0) {
	    last_char = buffer[n - 1];
	    char_count += n;
	    for(int i = 0; i < n; i++) {
	      if (buffer[i] == '\n')
	      {
	        line_count++;
	      } 
	    }
	    printf("Parent's message: %s\n", buffer);
	  }
	  if (n < 0) //implies error occurred
	  {
		  fprintf(stderr, "Failed to read from parent.\n");
		  return 1;
	  }
	if (last_char != '\n' && last_char != '\0') { line_count++; }
	char response[100];
	snprintf(response, sizeof(response), "Lines: %d, Characters: %d", line_count, char_count);
	write(pipe2[1], response, strlen(response));
	close(pipe2[1]);
	free(buffer);
	close(pipe1[0]);
	}
     else {
      // Parent process
      // TODO : Close unused pipe ends
      // close ( pipe1 [0]) ; // Close read end of pipe1
      close(pipe1[0]);
      // close ( pipe2 [1]) ; // Close write end of pipe2
      close(pipe2[1]);
      // TODO : Send messages , read responses
      printf("I'm the parent process!\n");
      char *buffer = malloc(4096);
      char *helloChild = "Hello child!\n";
      write(pipe1[1], helloChild, strlen(helloChild));
      close(pipe1[1]);
      ssize_t n;
	while((n = read(pipe2[0], buffer, 4096)) > 0) {
	    buffer[n] = '\0';
	    fprintf(stdout, "Child response: %s\n", buffer);
	}
	if (n < 0)
	{
		fprintf(stderr, "Failed to read from child.\n");
		return 1;
	}
      // TODO : wait () for child
      free(buffer);
      close(pipe2[0]);
      wait(NULL);
  }
  return 0;
}
