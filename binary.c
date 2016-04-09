#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "assemble.h"
#include "common.h"
#include "display.h"
#include "elf_gen.h"
#include "exedir.h"
#include "ptrace.h"
#include "child.h"

#include "binary.h"

#define STDIN_BUF_SZ 64000000 // 64mb
#define BYTECODE_BUF_SZ 64000000 // 64mb

extern struct options_t options;



void exec_binary(const pid_t pid, const char *binary, const char *offsetstr, const char *bytesstr)
{
  if((options.bytesstr == NULL) || (options.bytesstr == NULL)){
    if(options.count == 0){
      fprintf(stderr, "Need to use -c and -o in conjunction with -b\n");
      return;
    }
  }


    //parse2int(optarg);

}

void binary_mode()
{

	uint8_t *const bytecode = xmalloc(options.count);

	const int fd = open(options.binary, O_RDONLY);

	if(fd == -1){
	  perror("Had trouble opening the binary:");
	  exit(EXIT_FAILURE);
	}

	const off_t offset = lseek(fd, options.offset, SEEK_SET);

	if(offset == -1){
	  perror("Had trouble setting the offset in the binary:");
	  exit(EXIT_FAILURE);
	}

	const size_t bytecode_sz = read_data(fd, bytecode, options.count);


	if (bytecode_sz != options.count) {
	  fprintf(stderr, "Could not read that many bytes (%zu vs %zu)...\n", options.count, bytecode_sz);
	  exit(EXIT_FAILURE);
	}

	verbose_printf("Got assembly:\n");
	verbose_dump(bytecode, bytecode_sz, -1);


	const pid_t child_pid = gen_child();

	verbose_printf("child process is %d\n", child_pid);

	struct proc_info_t info = {};
	ARCH_INIT_PROC_INFO(info);

	ptrace_launch(child_pid);
	ptrace_cont(child_pid, &info);
	ptrace_reap(child_pid, &info);

	display(&info);

	
	ptrace_write(child_pid, (void *)options.start, bytecode, bytecode_sz);

	
	ptrace_reset(child_pid, options.start);

	ptrace_cont(child_pid, &info);

	ptrace_reap(child_pid, &info);
	ptrace_detatch(child_pid, &info);

	display(&info);
	
	free(bytecode);
	
}
