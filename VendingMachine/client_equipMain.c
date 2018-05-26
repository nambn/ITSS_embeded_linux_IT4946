#include <stdio.h>      
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <stdlib.h>    
#include <string.h>     
#include <unistd.h> 
#include <ctype.h>
#include <signal.h>
#include <time.h>

#define maxbuf_size 1024
#define nbq 10
#define PROC_OK  0
#define PROC_NG  -1


typedef struct{
  char name[20];
  int num;
}com;

int checkans;


void equipMain(int sock);
void commoditySales(int choice, int sock);
int   beverages();
void  chosen(int choice);
float drink_cost(int choice);
void readCommodityVM(char str[100], int sock);


int main(int argc, char *argv[])
{

  int sock;
  struct sockaddr_in servAddr;
  unsigned short servPort;
  char *servIP;

 /*-----------menu--------------*/
  
  if(argc != 3)
  {
    fprintf(stderr,"Usage: %s <Server IP> <Echo Port>\n",argv[0]);
    exit(1);
  }
  
  servIP = argv[1];
  servPort = atoi(argv[2]);
  
  if((sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
  {
    perror("socket() failed");
    exit(1);
  }
  
  memset(&servAddr,0,sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr(servIP);
  servAddr.sin_port = htons(servPort);
  
  if(connect(sock,(struct sockaddr*)&servAddr,sizeof(servAddr))<0)
  {
    perror("connect() failed");
    exit(1);
  }

  equipMain(sock);

  return 0;
}


void equipMain(int sock)
{
  int user_at_machine = 1;

  char str[100];

// float item_cost;
  float money;
  float change;
  int flag =0 ;
  while(user_at_machine)
  {
    int choice;
    /* in an actual program floats and doubles
       would not be used for monetary values
       as they are not always exact */
       float item_cost;
       // float money;
       // float change;
       // int flag =0 ;

    /* display items and get users choice */
       choice = beverages();

    /* display pick */
       chosen(choice);

    /* getting money could be in a function */
       if(choice >= 1 && choice <= 20)
       {
      /* get drinks cost */
        item_cost = drink_cost(choice);
        if (flag == 0){
          printf("Enter your money: ");
          scanf(" %f", &money);
          printf("\n\n");
        }
        

        if (money >= item_cost)
        {
          printf("ACCEPTED!\n");
          change = money - item_cost;
          printf("Your change is %.2f", change);
          switch(choice)
          {
            case 1: strcpy(str, "pepsi");      break;
            case 2: strcpy(str, "mirinda");      break;
            case 3: strcpy(str, "dew");      break;
            case 99:
            break;
            default:
            printf("Invalid input!\n");
            break;
          };

  //readCommodityVM(str,sock);

          char *args[] = {"ls", "-l", NULL};
          int iRet = 0;
          pid_t pid = 0;
          int  i, status = 0;

  // Creating child process
          pid = fork();
          if(pid == 0) pid = fork();
          switch (pid){
            case -1:
      // Process creating error processing
            perror("processGenerate fork");
            exit(1);
            case 0:
      // Child process processing

            commoditySales(choice,sock);

            iRet = execv("/bin/ls", args);

      // It doesn't pass when execv succeeds. 
            printf("execv ret = %d\n", iRet);
            if(iRet == PROC_NG)
            {
              exit(1);
            }

            default:
      // Parent process processing


      // Wait for the state transition of the child process. 
            pid = wait(&status);
            printf("pid=%d,status=%d\n", pid, status);
            break;
          }
        }
        else
        {
          printf("NOT ACCEPTED!\n");
        }
        printf("\n\n");
        if(change>0){
          printf("You have %.2f coins, do you want to continue? y/n?\n",change);
          char c;

      /* get newline in stream */
          getchar();
          c = getchar();
          //printf("%c llinh\n",c );
          if (c=='y'||c=='Y'){
            //printf("%c llinh22\n",c );
            money = change;
            flag = 1;
          }else{
            flag = 0;
            money =0;
          }
        }else{
          flag =0;
          getchar();
          getchar();
        }
        
        
      /* get character to pause */
        
      }
      else
      {
      /* is user quitting ? */
        if(choice == 99) {
          user_at_machine = 0;
        }
      }

  } /* end while */

    }


/* display beverages and get pick */
    int beverages()
    {
      int choice;

      printf("---------------------- \n");
      printf("Name list of beverage: ");
      printf("\n\n");
      printf("1.  Pepsi");              printf("\t\t\tRM1.00\n");
      printf("2.  Mirinda");            printf("\t\t\tRM1.00\n");
      printf("3.  Mountain Dew");       printf("\t\t\tRM1.00\n");
      printf("99. Quit");
      printf("\n\n");

      printf("Enter your choice: ");
      scanf(" %d",&choice);

      return choice;
    }


/* display users pick */
    void chosen(int choice)
    {
      switch(choice)
      {
        case 1:
        printf("You choose Pepsi");          printf("\tRM1.00\n");
        break;
        case 2:
        printf("You choose Mirinda");        printf("\tRM1.00\n");
        break;
        case 3:
        printf("You choose Mountain Dew");       printf("\tRM1.00\n");
        break;
        case 99:
        break;
        default:
        printf("Invalid input!\n");
        break;
      }
    }


/*get drink cost */
    float drink_cost(int choice)
    {
      float cost = 0;

      switch(choice)
      {
        case 1:
        cost = 1.00;
        break;
        case 2:
        cost = 1.00;
        break;
        case 3:
        cost = 1.00;
        break;
      }

      return cost;
    }


    void commoditySales(int choice, int sock)
    {
      char str[50];
      strcpy(str, "choice ");
      switch(choice)
      {
        case 1: strcat(str,"pepsi");break;
        case 2: strcat(str,"mirinda");break;
        case 3: strcat(str,"dew");break;
        default: break;
      }
      printf("%s\n", str);
      send(sock,str,strlen(str),0);
    }



    void readCommodityVM(char str[100], int sock)
    {
      FILE *fin,*fout;
      com sp[100];
      int count=0,i;
      fin = fopen("commodityVM.txt","r");
      if(fin==NULL)
      {
        printf("Cannot open file commodityVM.txt\n");
        return;
      }
      while(fscanf(fin,"%s %d\n",sp[count].name,&sp[count].num)!=EOF)
        count++;

      fclose(fin);
      fout = fopen("commodityVM.txt","w");
      if(fout==NULL){
        printf("Cannot open file commodityVM.txt\n");
        return;
      }
      for(i=0;i<count;i++)
        if(strcmp(sp[i].name,str)==0)
          sp[i].num = sp[i].num -1 ;
        for (i=0;i<count;i++)
        {
          if (sp[i].num < 3)
          {
            strcpy(str,"over ");
            strcat(str,sp[i].name);
            send(sock, str, strlen(str),0);
          }
        }

        for(i=0;i<count;i++)
          fprintf(fout,"%s %d\n",sp[i].name,sp[i].num);

      }
