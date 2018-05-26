#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<sys/mman.h>

typedef struct{
  char      str[128];
  long      lval;
  double  dval;
}TestData;

#define NUMBER (10)

int main()
{
  int pid;
  int          fd;
  char        c;
  long        psize,size;
 TestData *ptr;
  long        i,lval;
  double    dval;
  char        buf[512],*p;

  /* mapping file open */
  if((fd=open("MapFile",O_RDWR|O_CREAT,0666))==-1){
    perror("open");
    exit(-1);
  }
  /* Calculate size in which boundary match is done by page 
size. */
  psize=sysconf(_SC_PAGE_SIZE);
  size=(NUMBER*sizeof(TestData)/psize+1)*psize;

  switch (pid = fork()){
    case -1:
      // Process creating error processing
      perror("processGenerate fork");
      exit(1);
    case 0:
      ptr =  (TestData *)
               mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
      if((int)ptr==-1){
        perror("mmap");
        exit(-1);
      }

      TestData *ptr1 = ptr;
      for(i=0;i<NUMBER;i++) {
        printf("%s %ld %lf\n",ptr1->str,ptr1->lval,ptr1->dval); 
        ptr1++;
      }
      /* Synchronize a file with a memory map */
      msync(ptr,size,MS_ASYNC);
      /* Unmap */
      if(munmap(ptr,size)==-1){
        perror("munmap");
      }

      /* File close */
      close(fd);
      break;
    default:
      for(i=0; i<NUMBER;i++) {
	TestData data = {"abc", i, 1.5};
	write(fd, &data,sizeof(TestData));
	lseek(fd, sizeof(TestData)*(i+1), SEEK_SET);
      }
  }

  printf("main           : end\n");
  return 0;
}
