#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
// ---- helpers: safe (unaligned) loads, little-endian ----
static inline uint64 load_u64_le_unaligned(const char *p) {
  uint64 v = 0;
  for (int i = 7; i >= 0; --i) v = (v << 8) | (unsigned char)p[i];
  return v;
}
static inline int load_i32_le_unaligned(const char *p) {
  int v = 0;
  for (int i = 3; i >= 0; --i) v = (v << 8) | (unsigned char)p[i];
  return v;
}
static inline short load_i16_le_unaligned(const char *p) {
  short v = 0;
  for (int i = 1; i >= 0; --i) v = (v << 8) | (unsigned char)p[i];
  return v;
}

// print uint64 as lowercase hex without 0x, no leading zeros
static void printhex64(uint64 v){
  static const char digits[] = "0123456789abcdef";
  char buf[16];
  int n = 0;
  if (v == 0) { printf("0\n"); return; }
  while (v) { buf[n++] = digits[v & 0xF]; v >>= 4; }
  for (int i = n-1; i >= 0; --i) printf("%c", buf[i]);
  printf("\n");
}

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  // Your code here.
 for (int i = 0; fmt[i]; i++) {
    switch (fmt[i]) {
    case 'i': { // 4 bytes: 32-bit int (decimal)
      int val = load_i32_le_unaligned(data);
      printf("%d\n", val);
      data += 4;
      break;
    }
    case 'p': { // 8 bytes: 64-bit integer printed in hex
      uint64 val = load_u64_le_unaligned(data);
      printhex64(val);
      data += 8;
      break;
    }
    case 'h': { // 2 bytes: 16-bit int (decimal)
      short val = load_i16_le_unaligned(data);
      printf("%d\n", val);
      data += 2;
      break;
    }
    case 'c': { // 1 byte: ASCII char
      char val = *data;
      printf("%c\n", val);
      data += 1;
      break;
    }
    case 's': { // next 8 bytes = pointer to C string; print the string
      char *s = (char*)load_u64_le_unaligned(data);
      printf("%s\n", s);
      data += 8;
      break;
    }
    case 'S': { // rest is a C string at 'data'
      printf("%s\n", data);
      data += strlen(data) + 1; // move past terminating NUL
      break;
    }
    default:
      // ignore unknown specifiers
      break;
    }
  }
}
