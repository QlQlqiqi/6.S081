#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char cur_dir[DIRSIZ];
char parent_dir[DIRSIZ];

char *
fmtname(char *path)
{
  static char buf[DIRSIZ + 1];
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if (strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  return buf;
}

void find(char *path, char *name)
{
  // printf("find: path: %s    name: %s\n", path, name);
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type)
  {
  case T_FILE:
  {
    // char cur[DIRSIZ];
    // memcpy(cur, fmtname(path), DIRSIZ);
    // printf("T_FILE: %d %s   %d %s\n", strlen(cur), cur, strlen(name), name);
    if (strcmp(fmtname(path), name) == 0)
    {
      printf("%s\n", path);
    }
    break;
  }

  case T_DIR:
  {
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if (stat(buf, &st) < 0)
      {
        printf("find: cannot stat %s\n", buf);
        continue;
      }

      char *fmt_name = fmtname(buf);
      if (strcmp(fmt_name, cur_dir) == 0 || strcmp(fmt_name, parent_dir) == 0)
      {
        // printf("ignore %s\n", fmt_name);
        continue;
      }
      // printf("%s %s %d %d %d\n", fmt_name, buf, st.type, st.ino, st.size);
      find(buf, name);
      // printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("find need 3 argument\n");
    exit(1);
  }
  memcpy(cur_dir, fmtname("."), DIRSIZ);
  memcpy(parent_dir, fmtname(".."), DIRSIZ);
  
  char name[DIRSIZ];
  memcpy(name, fmtname(argv[2]), DIRSIZ);
  find(argv[1], name);
  exit(0);
}
