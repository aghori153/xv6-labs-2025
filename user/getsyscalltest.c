// user/getsyscalltest.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  // make a few syscalls: write, getpid, sleep, open/close (open will fail gracefully)
  printf("hello from test\n");               // write()
  int pid = getpid();                        // getpid()
  sleep(1);                                  // sleep()
  int fd = open("no_such_file", 0);          // open()
  if (fd >= 0) close(fd);                    // close() if it succeeded

  int c = getsyscallcount();                 // <-- counts includes this call too
  printf("pid %d made %d syscalls (including this query)\n", pid, c);
  exit(0);
}
