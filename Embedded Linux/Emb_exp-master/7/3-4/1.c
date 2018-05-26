#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main() {
  if(system("ls -l") == -1) {
    perror("error\n");
    return -1;
  }
  return 0;
}
