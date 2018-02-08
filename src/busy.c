#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  printf("'shell' is running\n");
  pid_t child_pid;

  child_pid = fork();

  if (child_pid == 0) {
    printf("child started\n");
    int j=0;
    for (int i=0; i<2000000000; ++i) {
      j=i-3+j;
    }
    printf("j=%d",j);
    /*    sleep(5);*/
    /*    execl("/bin/ls","ls","-ll",NULL); */

    printf("child ended\n");
    return 3;
  } else {
    printf("parent waiting for child\n");
    int child_status;
    waitpid(child_pid,&child_status,0);
    printf("child exited with status %d\n", (child_status/256));
  }
  return 0;
}
