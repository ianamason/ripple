#include <getopt.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "common.h"
#include "exedir.h"
#include "pipe.h"
#include "binary.h"
#include "ui.h"

// Defaults
struct options_t options = {
	.start = 0x400000,
	.verbose = 0,
	.raw = 0,
	.allregs = 0,
	.savefile = NULL,
	.binary = NULL,
	.offsetstr = NULL,
	.bytesstr = NULL
};



static
void usage(
		const char *argv0)
{
	fprintf(stderr, "Usage: %s [options]\n"
			"\t-h\t\tDisplay this help\n"
			"\t-r\t\tTreat stdin as raw bytecode (useful for ascii shellcode)\n"
			"\t-p\t\tPass signals to child process (will allow child to kill itself via SIGSEGV, others)\n"
			"\t-s <filename>\tSave generated exe to <filename>\n"
			"\t-x\t\tDisplay all registers (FP)\n"
			"\t-v\t\tIncrease verbosity\n"
			"\t-b <binary>\t\tLoad from an binary (need to also use -c)\n"
			"\t-o <offset>\t\toffset into the binary\n"
			"\t-c <bytes>\t\tnumber of bytes to read from the binary\n"
			, argv0);

	exit(EXIT_FAILURE);
}

static
void parse_opts(
		int argc,
		char **argv) {
  int c;
  
  while ((c = getopt(argc, argv, "s:b:o:c:hrpvx")) != -1)
    switch (c) {
    case 'h':
      usage(argv[0]);
      break;
    case 'r':
      ++options.raw;
      break;
    case 's':
      options.savefile = optarg;
      break;
    case 'p':
      ++options.passsig;
      break;
    case 'v':
      ++options.verbose;
      break;
    case 'x':
      ++options.allregs;
      break;
    case 'b':
      options.binary = optarg;
      break;
    case 'o':
      options.offsetstr = optarg;
      break;
    case 'c':
      options.bytesstr =  optarg;
      break;
    default:
      exit(EXIT_FAILURE);
    }

  

}

int main(int argc, char **argv) {
	// Lot of arg parsing here

	clean_exedir();

	parse_opts(argc, argv);

	if (options.binary != NULL)
	  binary_mode();
	else if (isatty(STDIN_FILENO))
	  interact(argv[0]);
	else
	  pipe_mode();
	return 0;
}