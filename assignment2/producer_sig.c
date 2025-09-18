#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>

volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

void handle_sigint(int sig) {
        shutdown_flag = 1;
}

void handle_sigusr1(int sig) {
        stats_flag = 1;
}

int main ( int argc , char *argv []) {
	FILE *input = stdin;
	int buffer_size = 4096;
	char opt;
        struct sigaction sa;
        sa.sa_handler = handle_sigint;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);

	int fd;
	char *buffer;

	while((opt = getopt(argc, argv, "f:b:")) != -1){
		switch(opt) {
			case 'f':
				fd = open(optarg, O_RDONLY);
				if (fd < 0) {
					fprintf(stderr, "No file provided\n");
                                        return 1;
				}
				break;
			case 'b':
				char* endptr;
				buffer_size = (int)strtol(optarg, &endptr, 10);
				if (*endptr != '\0')
				{
					fprintf(stderr, "Buffer argument is invalid\n");
                                        return(1);
				}
				break;
		        }
		}
	buffer = malloc(buffer_size);
	if (buffer == NULL)
	{
		fprintf(stderr, "Failed to allocate memory to buffer");
	}
	

	ssize_t n;
	
	while((n = read(fd, buffer, buffer_size)) > 0)
	{
		ssize_t m = write(1, buffer, n);
		if (m < 0)
		{
			fprintf(stderr, "Failed to write to buffer.\n");
			break;
		}
		if (shutdown_flag)
		{
		  close(fd);
		  free(buffer);
		  return 0;
		}
	}
	if (n < 0)
	{
		fprintf(stderr, "Failed to read file.\n");
		return 1;
	}
	close(fd);
	free(buffer);
	return 0;
}
