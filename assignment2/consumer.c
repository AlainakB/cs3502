#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

int main ( int argc , char * argv []) {

	int max_lines = -1; // -1 means unlimited
	int verbose = 0;
	// TODO : Parse arguments ( -n max_lines ,-v verbose )
	
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
	// TODO : Read from stdin line by line
	ssize_t n;
	int line_count = 0;
	int char_count = 0;
	int hit_max = 0;
	char last_char;

	while((n = read(0, buffer, 512)) > 0)
	{
		char_count += n;
		//n is the bytes read
	 	for(int i = 0; i < n; i++)
		{
			//printf("buffer[%d] = '%c' (ASCII %d)\n", i, buffer[i], (unsigned char)buffer[i]);
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
                        //printf("Is verbose, printing buffer");
                        fwrite(buffer, 1, n, stdout);
		}	
		if (hit_max)
		{
		  break;
		}
		last_char = buffer[n - 1];	
	}
	if (last_char != '\n') { //accounts for if the last line has no '\n'
		line_count++;
	}

	// Count lines and characters 
	// If verbose, echo lines to stdout
	// TODO : Print statistics to stderr
	// fprintf (stderr, "Lines: %d \n", line_count);
	fprintf (stderr, "Lines: %d, Characters: %d\n", line_count, char_count);
	free(buffer);
	return 0;
}
