#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  // Prints ticks since boot (as required).
  // (Optional) seconds ≈ uptime()/100 on xv6-riscv.
  printf("%d\n", uptime());
  exit(0);
}
