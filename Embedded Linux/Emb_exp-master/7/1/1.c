#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>

int flag = 0;

void sigHandleSigint(int sigNo); 
void sigHandleSigalrm(int sigNo); 

int main(int argc, char **argv) {
  signal(SIGINT, sigHandleSigint);
  signal(SIGALRM, sigHandleSigalrm);
  int num_seconds = atoi(argv[1]);
  alarm(num_seconds);

  for(;;) {
    if(flag) {
      printf("Hello\n");
      flag = 0;
      alarm(num_seconds);
    }
  }
  
  return 0;
}

void sigHandleSigint(int sigNo) {
  printf("Interrupt\n");
  exit(0);
}

void sigHandleSigalrm(int sigNo) {
  flag = 1;
}
