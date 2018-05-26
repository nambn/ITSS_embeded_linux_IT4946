#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main() {
  char  str[512],*ptr;
  FILE  *fp;

  if((fp=popen("netstat","r"))==NULL){
    printf("popen error\n");
    return(-1);
  }

  while(1){
    fgets(str,512,fp);
    if(feof(fp)){
      break;
    }

    // \n is converted into \0. 
    ptr=strchr(str,'\n');
    if(ptr!=NULL){
      *ptr='\0';
    }
    printf("popen result   %s\n",str);
  }

  pclose(fp);
  return 0;
}
