#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int fds1[2];
  int fds2[2];
  if (pipe(fds1) < 0 || pipe(fds2) < 0)
  {
    printf("pingpong: pipe failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0)
  {
    printf("pingpong: fork failed\n");
    exit(1);
  }

  // printf("<%d>: asdasdas\n", pid);
  if (pid == 0)
  {
    // child
    char ch;
    if (read(fds2[0], &ch, 1) <= 0)
    {
      printf("pingpong: child read failed\n");
      exit(1);
    }
    write(fds1[1], &ch, 1);
    close(fds1[0]);
    close(fds1[1]);
    close(fds2[0]);
    close(fds2[1]);
    printf("<%d>: received ping\n", getpid());
  }
  else
  {
    // parent
    char ch = '0';
    if (write(fds2[1], &ch, 1) <= 0)
    {
      printf("pingpong: child write failed\n");
      exit(1);
    }
    read(fds1[0], &ch, 1);
    close(fds1[0]);
    close(fds1[1]);
    close(fds2[0]);
    close(fds2[1]);
    printf("<%d>: received pong\n", getpid());
  }

  exit(0);
}
