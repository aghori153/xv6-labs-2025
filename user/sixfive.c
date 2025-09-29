#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static char seps[] = " -\r\t\n./,";

static int is_sep(char c){
  for (int i = 0; seps[i]; i++)
    if (c == seps[i]) return 1;
  return 0;
}

static void process_fd(int fd){
  char buf[512];
  int n;

  int innum = 0;        // currently inside a run of digits
  int valid = 0;        // this run started after a separator/start?
  int num   = 0;
  int prev_sep = 1;     // start-of-file counts as a separator

  while ((n = read(fd, buf, sizeof(buf))) > 0){
    for (int i = 0; i < n; i++){
      char c = buf[i];

      if (c >= '0' && c <= '9'){
        if (!innum){
          innum = 1;
          num = (c - '0');
          valid = prev_sep;     // only valid if started after a separator
        } else {
          num = num * 10 + (c - '0');
        }
        // still inside digits; prev_sep doesn’t change here
      } else if (is_sep(c)){
        if (innum){
          if (valid && (num % 5 == 0 || num % 6 == 0))
            printf("%d\n", num);
          innum = 0; num = 0; valid = 0;
        }
        prev_sep = 1;           // separator seen
      } else {
        // non-digit, non-separator: breaks numbers and makes next start invalid
        if (innum){
          // run ends but is invalid because it didn’t end with a separator
          innum = 0; num = 0; valid = 0;
        }
        prev_sep = 0;           // next digit run (if any) won’t be valid unless a separator appears
      }
    }
  }

  // EOF acts as a separator
  if (innum){
    if (valid && (num % 5 == 0 || num % 6 == 0))
      printf("%d\n", num);
  }
}

int
main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(2, "Usage: sixfive file [file...]\n");
    exit(1);
  }

  for (int i = 1; i < argc; i++){
    int fd = open(argv[i], 0);
    if (fd < 0){
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      continue;                 // try remaining files
    }
    process_fd(fd);
    close(fd);
  }
  exit(0);
}

