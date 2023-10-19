#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MIN_NUM 2
#define MAX_NUM 35

void print_prime(int *input, int num)
{
  if (num <= 0)
  {
    return;
  }

  int first = input[0];
  printf("prime %d\n", first);

  int output[num];
  int output_num = 0;

  // 移除 non-prime number
  for (int i = 1; i < num; i++)
  {
    if (input[i] % first == 0)
    {
      continue;
    }
    output[output_num++] = input[i];
  }

  // nothing
  if (output_num == 0)
  {
    return;
  }

  int p_fd[2], c_fd[2];
  pipe(p_fd);
  pipe(c_fd);

  if (fork() == 0)
  {
    // child process work
    close(c_fd[1]);
    close(p_fd[0]);
    int input[MAX_NUM - MIN_NUM + 1];
    int num = 0;
    while (read(c_fd[0], input + num, sizeof(int)) > 0)
    {
      num++;
    }
    close(c_fd[0]);
    print_prime(input, num);

    // 告诉父进程结束
    write(p_fd[1], &num, sizeof(int));
    close(p_fd[1]);
  }
  else
  {
    // parent waits child working
    close(c_fd[0]);
    close(p_fd[1]);

    write(c_fd[1], output, sizeof(int) * output_num);
    close(c_fd[1]);

    read(p_fd[0], &output_num, sizeof(int));
    close(p_fd[0]);
  }
}

int main(int argc, char *argv[])
{

  int num = MAX_NUM - MIN_NUM + 1;
  int output[num];
  for (int i = MIN_NUM; i <= MAX_NUM; i++)
  {
    output[i - MIN_NUM] = i;
  }
  print_prime(output, num);
  exit(0);
}
