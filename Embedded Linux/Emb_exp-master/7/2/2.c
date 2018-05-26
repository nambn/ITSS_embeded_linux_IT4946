#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<time.h>
#include<unistd.h>

void sigHandleSigint(int sigNo); 

int main(int argc, char **argv) {
  signal(SIGINT, sigHandleSigint);

  for(;;) {
    printf("Hello\n");
    usleep(1000000);
  }
  
  return 0;
}

void sigHandleSigint(int sigNo) {
  printf("Interrupt\n");
  exit(0);
}
