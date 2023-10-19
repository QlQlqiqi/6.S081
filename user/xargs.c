#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
  char *args[MAXARG + 1];
  for (int i = 0; i < argc; i++)
  {
    args[i] = argv[i];
  }

  char buf[256];
  while (1)
  {
    int i = 0;
    while (read(0, buf + i, sizeof(char)) != 0 && buf[i] != '\n')
    {
      i++;
    }

    // EOF
    if (i == 0)
    {
      break;
    }

    buf[i] = 0;
    args[argc] = buf;
    args[argc + 1] = 0;

    if (fork() == 0)
    {
      exec(args[1], args + 1);
      fprintf(2, "xargs: exec failed\n");
    }
    else
    {
      wait(0);
    }
  }

  exit(0);
}
