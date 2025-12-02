#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// simple delay to simulate some work in the parent
void
delay(long ticks)
{
  for (long t = 0; t < ticks; t++) {
    for (volatile int i = 0; i < 1000000; i++) {
      // busy wait
    }
  }
}

int
main(void)
{
  int p[2];
  char buf[1];
  int count = 0;

  if (pipe(p) < 0) {
    printf("iopipe: pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    printf("iopipe: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // ---- CHILD: mostly I/O-bound, blocking on read() ----
    close(p[1]); // close write end
    printf("iopipe child: waiting on pipe...\n");
    while (1) {
      int n = read(p[0], buf, 1);   // will block when no data
      if (n <= 0) {
        // writer closed -> exit
        break;
      }
      count++;
      // Only print every 100 bytes to avoid spamming
      if (count % 100 == 0) {
        printf("iopipe child: processed %d bytes\n", count);
      }
    }
    exit(0);
  } else {
    // ---- PARENT: occasional writer, no prints in the loop ----
    close(p[0]); // close read end
    printf("iopipe parent: writing occasionally...\n");
    while (1) {
      delay(30);              // slow down writes a lot
      write(p[1], "x", 1);    // this wakes the child sometimes
    }
  }
}
