#include "kernel/types.h"
#include "user/user.h"

void
memdump(char *fmt, char *data)
{
  for (int i = 0; fmt[i]; i++) {
    char f = fmt[i];
    switch (f) {
    case 'i': {  // 4 bytes: 32-bit int, decimal
      int val = *(int*)data;
      printf("%d\n", val);
      data += 4;
      break;
    }
    case 'p': {  // 8 bytes: 64-bit integer, hex (lowercase, no 0x)
      unsigned long long val = *(unsigned long long*)data;
      printf("%llx\n", val);
      data += 8;
      break;
    }
    case 'h': {  // 2 bytes: 16-bit int, decimal
      short val = *(short*)data;
      printf("%d\n", val);
      data += 2;
      break;
    }
    case 'c': {  // 1 byte: ASCII character
      char val = *data;
      printf("%c\n", val);
      data += 1;
      break;
    }
    case 's': {  // pointer to a C string (8-byte pointer)
      char *s = *(char**)data;
      printf("%s\n", s);
      data += 8;
      break;
    }
    case 'S': {  // string starting at current data
      printf("%s\n", data);
      data += strlen(data) + 1;
      break;
    }
    default:
      // ignore unknown specifiers
      break;
    }
  }
}
