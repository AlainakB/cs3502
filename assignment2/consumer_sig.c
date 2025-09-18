#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>

volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

void handle_sigint(int sig) {
        shutdown_flag = 1;
}

void handle_sigusr1(int sig) {
        stats_flag = 1;
}

int main ( int argc , char * argv []) {
        struct sigaction sa, sa_usr1;
        sa.sa_handler = handle_sigint;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);

        sa_usr1.sa_handler = handle_sigusr1;
        sigemptyset(&sa_usr1.sa_mask);
        sa_usr1.sa_flags = 0;
        sigaction(SIGUSR1, &sa_usr1, NULL);
        
        clock_t start = clock();

	int max_lines = -1; // -1 means unlimited
	int verbose = 0;
	
	char opt;
	while ((opt = getopt(argc, argv, "n:v")) != -1)
	{
		switch(opt) {
                case 'n':
			max_lines = (int)strtol(optarg, NULL, 10);
			break;
		case 'v':
			verbose = 1; //truthy
			break;
		}
	}
	
	char *buffer = malloc(512);
	ssize_t n;
	int line_count = 0;
	int char_count = 0;
	int hit_max = 0;
	char last_char;

	while((n = read(0, buffer, 512)) > 0)
	{
		char_count += n;
	 	for(int i = 0; i < n; i++)
		{
			if (buffer[i] == '\n')
			{
				line_count++;
			}
			if (max_lines != -1 && line_count >= max_lines)
			{
			  hit_max = 1;
			  break;
			}
		}
		if (verbose)
		{       
                        fwrite(buffer, 1, n, stdout);
		}

		if (stats_flag)
		{
		  printf("Stats:\n");
		  clock_t end = clock();
	          double cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;
	          double lines_per_second = line_count / cpu_time;
	          double bytes_per_second = char_count / cpu_time;
	          double MBs = ((double)char_count / 1024 / 1024) / cpu_time;
	          printf("Latency: %lf\nThroughput:\nLines per second: %lf\nBytes per second %lf\nMB/s = %lf", cpu_time, lines_per_second, bytes_per_second, MBs);
	          fflush(stdout);
	          stats_flag = 0;
	          }
		if (shutdown_flag)
		{
		  printf("Shutdown:\n");
		  clock_t end = clock();
	          double cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;
	          double lines_per_second = line_count / cpu_time;
	          double bytes_per_second = char_count / cpu_time;
	          double MBs = ((double)char_count / 1024 / 1024) / cpu_time;
	          printf("Latency: %lf\nThroughput:\nLines per second: %lf\nBytes per second %lf\nMB/s = %lf", cpu_time, lines_per_second, bytes_per_second, MBs);
	          fflush(stdout);
	          free(buffer);
	          return 0;
		}
		if (hit_max)
		{
		  break;
		}
		last_char = buffer[n - 1];	
	}
	if (last_char != '\n') {
		line_count++;
	}
	fprintf (stderr, "Lines: %d, Characters: %d\n", line_count, char_count);
	printf("Completion:\n");
	clock_t end = clock();
	double cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;
	double lines_per_second = line_count / cpu_time;
	double bytes_per_second = char_count / cpu_time;
	double MBs = ((double)char_count / 1024 / 1024) / cpu_time;
        printf("Latency: %lf\nThroughput:\nLines per second: %lf\nBytes per second %lf\nMB/s = %lf", cpu_time, lines_per_second, bytes_per_second, MBs);
	free(buffer);
	return 0;
}
