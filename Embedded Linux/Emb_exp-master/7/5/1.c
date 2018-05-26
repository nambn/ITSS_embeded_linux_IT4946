#include <stdio.h>
#include <unistd.h>

#define SLEEP_SEC 1

int main(void)
{
  int i;
  char buf[512];

  // making to daemon
  daemon(1,1);

  // cwd display
  printf("cwd = %s\n", getcwd(buf, sizeof(buf)));

  // daemon executing display
  for(i=0; i<100; i++){
    printf("daemon sample is executing %d\n", i);
    sleep(SLEEP_SEC);
  }

  return 0;
}
