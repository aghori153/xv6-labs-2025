#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "kernel/fs.h"
#include "user/user.h"

// ---- tiny regex (from K&R / grep.c style) ----
static int matchhere(char *re, char *text);
static int matchstar(int c, char *re, char *text);

static int match(char *re, char *text) {
  if(re[0] == '^') return matchhere(re+1, text);
  do{
    if(matchhere(re, text)) return 1;
  } while(*text++ != '\0');
  return 0;
}
static int matchhere(char *re, char *text) {
  if(re[0] == '\0') return 1;
  if(re[1] == '*') return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0') return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}
static int matchstar(int c, char *re, char *text) {
  do{
    if(matchhere(re, text)) return 1;
  } while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}
// ----------------------------------------------

// run "cmd file" (for -exec)
static void runexec(char *file, int argc_cmd, char **cmdv) {
  char *argv[MAXARG];
  int i = 0;
  for(; i < argc_cmd && i < MAXARG-2; i++) argv[i] = cmdv[i];
  argv[i++] = file;
  argv[i] = 0;

  int pid = fork();
  if(pid == 0){
    exec(argv[0], argv);
    fprintf(2, "find: exec %s failed\n", argv[0]);
    exit(1);
  } else if(pid > 0){
    wait(0);
  } else {
    fprintf(2, "find: fork failed\n");
  }
}

static void recfind(char *path, char *target, int use_regex,
                    int hasexec, int argc_cmd, char **cmdv) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0) return;
  if(fstat(fd, &st) < 0){ close(fd); return; }

  if(st.type == T_FILE){
    // basename
    char *base = path + strlen(path);
    while(base > path && *(base-1) != '/') base--;
    int ok = use_regex ? match(target, base) : !strcmp(base, target);
    if(ok){
      if(hasexec) runexec(path, argc_cmd, cmdv);
      else printf("%s\n", path);
    }
  } else if(st.type == T_DIR){
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){ close(fd); return; }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0) continue;
      if(!strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if(stat(buf, &st) >= 0){
        if(st.type == T_FILE){
          int ok = use_regex ? match(target, de.name) : !strcmp(de.name, target);
          if(ok){
            if(hasexec) runexec(buf, argc_cmd, cmdv);
            else printf("%s\n", buf);
          }
        } else if(st.type == T_DIR){
          recfind(buf, target, use_regex, hasexec, argc_cmd, cmdv);
        }
      }
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "Usage: find path name|pattern [-re] [-exec cmd args]\n");
    exit(1);
  }
  char *path = argv[1];
  char *name_or_pat = argv[2];

  int use_regex = 0;
  int hasexec   = 0;
  int argc_cmd  = 0;
  char **cmdv   = 0;

  // parse optional flags: [-re] [-exec ...]
  int i = 3;
  for(; i < argc; ){
    if(!strcmp(argv[i], "-re")){
      use_regex = 1; i++;
    } else if(!strcmp(argv[i], "-exec")){
      hasexec = 1; i++;
      argc_cmd = argc - i;
      cmdv = &argv[i];
      if(argc_cmd < 1){
        fprintf(2, "find: -exec needs a command\n");
        exit(1);
      }
      break; // exec must be last
    } else {
      fprintf(2, "find: unknown flag %s\n", argv[i]);
      exit(1);
    }
  }

  recfind(path, name_or_pat, use_regex, hasexec, argc_cmd, cmdv);
  exit(0);
}
