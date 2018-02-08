#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  printf("main started (pid=%d)\n", getpid());

  pid_t child_pid;
  child_pid = fork();
  printf("fork called (my pid=%d fork returned %d)\n",getpid(), child_pid);

  if (child_pid == 0) {
    int sec=5;
    int code=3;
    printf("child process (pid=%d) sleeping %d seconds\n", getpid(),sec);
    sleep(sec);
    printf("child process (pid=%d) execing ls -ll\n", getpid());    
    execl("/bin/ls","ls","-ll",NULL);
    printf("child process (pid=%d) returned from exec, exit code %d.\n", getpid(),code);
    return code;
  } else {
    printf("parent process (pid=%d) waiting for child (pid=%d)\n",getpid(),child_pid);
    int child_status;
    waitpid(child_pid,&child_status,0);
    printf("parent process (pid=%d) child (pid=%d) exited with status %d\n",
	   getpid(), child_pid, (child_status/256));
    return 0;
  }
}
