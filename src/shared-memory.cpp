#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

char sharename[1024];
const int sharesize = 1024*1024;

struct share {
  int frame;
  char message[1024];
}

void child() {
  printf("child: I am the share=%s\n",sharename);
  int fd = open(sharename,  O_RDWR, 0);
  void *base = mmap(NULL, sharesize, PROT_READ|PROT_WRITE, MAP_SHARED,fd,0);
  printf("child: mapped %p\n",base);
  share *shared = (share*) base;
}

void parent() {
  printf("parent: I am the parent share=%s\n",sharename);
  int fd = open(sharename,  O_RDWR, 0);
  void *base = mmap(NULL, sharesize, PROT_READ|PROT_WRITE, MAP_SHARED,fd,0);
  printf("parent: mapped %p\n",base);

  share *shared = (share*) base;
  for (int frame = 1; frame <= 10; ++frame) {
    snprintf(shared->message,sizeof(shared->message),"message % %d",frame);
    shared->frame = frame;
  }
}

int main(int argc, char *argv[])
{
  printf("main started (pid=%d)\n", getpid());

  snprintf(sharename, sizeof(sharename),"/tmp/shm.%d",getpid());
  printf("sharename='%s'\n", sharename);

  int fd = creat(sharename, O_CREAT|S_IRUSR|S_IWUSR);
  if (fd < 0) {
    perror(0);
    exit(1);
  }
  char zero[4096];
  for (int i=0; i<sizeof(zero); ++i) zero[i]=0;
  for (int i=0; i<sharesize; i += sizeof(zero)) {
    write(fd, zero,sizeof(zero));
  }
  close(fd);

  pid_t child_pid;
  child_pid = fork();
  if (child_pid == 0) {
    child();
  } else {
    parent();
  }
  return 0;
}
