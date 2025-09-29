// user/memdump.c
#include "kernel/types.h"
#include "user/user.h"

void
memdump(char *fmt, char *data)
{
  for (int i = 0; fmt[i]; i++) {
    switch (fmt[i]) {
    case 'i': {
      int v = *(int*)data;
      printf("%d\n", v);
      data += 4;
      break;
    }
    case 'p': {
      unsigned long long v = *(unsigned long long*)data;
      printf("%llx\n", v);
      data += 8;
      break;
    }
    case 'h': {
      short v = *(short*)data;
      printf("%d\n", v);
      data += 2;
      break;
    }
    case 'c': {
      char v = *data;
      printf("%c\n", v);
      data += 1;
      break;
    }
    case 's': {
      char *s = *(char**)data;
      printf("%s\n", s);
      data += 8;
      break;
    }
    case 'S': {
      printf("%s\n", data);
      data += strlen(data) + 1;
      break;
    }
    default:
      break;
    }
  }
}

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    // ---- Example 1 ----
    printf("Example 1:\n");
    int e1[2] = {61810, 2025};
    memdump("ii", (char*)e1);

    // ---- Example 2 ----
    printf("Example 2:\n");
    char ex2[] = "a string";
    memdump("S", ex2);

    // ---- Example 3 ----
    printf("Example 3:\n");
    char ex3[] = "another";
    memdump("S", ex3);

    // ---- Example 4 ----
    // Layout: "BD0\0" | int 1819438967 | int 100 | 'z' | (char*) "xyzzy"
    printf("Example 4:\n");
    char *tail = "xyzzy";
    char e4[4 + 4 + 4 + 1 + sizeof(char*)];
    int off = 0;
    e4[off++] = 'B';
    e4[off++] = 'D';
    e4[off++] = '0';
    e4[off++] = 0;
    *(int*)(e4 + off) = 1819438967; off += 4;
    *(int*)(e4 + off) = 100;        off += 4;
    e4[off++] = 'z';
    *(uint64*)(e4 + off) = (uint64)tail;
    memdump("Siics", e4);  // S -> "BD0", then i, i, c, s

    // ---- Example 5 ----
    // Layout: "hello\0world"
    printf("Example 5:\n");
    char e5[] = "hello\0world";
    memdump("Sccccc", e5); // "hello", then w o r l d one per line

    exit(0);
  }

  // Format provided: read stdin into buffer and dump
  char buf[1024];
  int n = 0, m;
  while ((m = read(0, buf + n, sizeof(buf) - n)) > 0) {
    n += m;
    if (n >= (int)sizeof(buf)) break;
  }
  if (n < (int)sizeof(buf)) buf[n] = 0;

  memdump(argv[1], buf);
  exit(0);
}
