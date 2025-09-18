#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

int main ( int argc , char *argv []) {
	FILE *input = stdin;
	int buffer_size = 4096;
	char opt;

	//argc = ARGument Count / number of arguments
	//argv = ARGument Vector (array)

	// TODO : Parse command line arguments
	// -f filename (optional)
	// -b buffer_size (optional)

        //extra notes for myself
	//getopt takes in the # of arguments, the argument  array, and a string of allowed flags. If the flag ends with a colon, it means that it's expected to come with an argument
	//it returns a flag character when it's called, and when called again, it returns the next flag
	//opt will equal the next flag found in argv + a pointer called optarg pointing to the argument (if there is one). It returns -1 when there are no more arguments

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
                                        return 1;
				}
				break;
		        }
		}
	buffer = malloc(buffer_size); //makes a pointer, buffer, to a dynamically sized character array
	if (buffer == NULL)
	{
		fprintf(stderr, "Failed to allocate memory to buffer");
	}


	// TODO : Open file if -f provided

	// TODO : Allocate buffer

	// TODO : Read from input and write to stdout
	

	ssize_t n; //return type of read and write, specifies the the number of bytes processed or returns a negative for an error
	
	while((n = read(fd, buffer, buffer_size)) > 0)  //reads (at max) a buffer-sized chunk of bytes from the file descriptor each call
	{
		ssize_t m = write(1, buffer, n); //takes the bytes that were read in buffer and writes them to stdout (fd = 1)
		if (m < 0)
		{
			fprintf(stderr, "Failed to write to buffer.\n");
			break;
		}
	}
	if (n < 0) //implies error occurred
	{
		fprintf(stderr, "Failed to read file.\n");
		return 1;
	}
	
	// TODO : Cleanup
	close(fd);
	free(buffer);
	return 0;
}
