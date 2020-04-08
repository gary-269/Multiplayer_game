#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

void player(char *s, int *fd1, int *fd2)
{
  int points=0;
  int dice;
  long int ss=0;
  char turn;
  while(1)
  {
    read(fd1[0], &turn, 1);
    printf("%s: playing my dice\n", s);
    dice =(int) time(&ss)%10 + 1;
    printf("%s: got %d points\n", s, dice);
    points+=dice;
    printf("%s: Total so far %d\n\n", s, points);
    if(points >= 100)
    {
      printf("%s: game over I won\n", s);
      kill(0, SIGTERM);
    }
    sleep(5);    
    write(fd2[1], &turn, 1);
  }
}
int main(int argc, char *argv[])
{ 
  char message[255] = {0};
  int server, portNumber, pid, n;
  socklen_t len;
  struct sockaddr_in servAdd;
  if(argc != 3)
  {
    printf("Call model: %s <IP> <Port #>\n", argv[0]);
    exit(0);
  }
  if((server=socket(AF_INET, SOCK_STREAM, 0))<0)
  {
    fprintf(stderr, "Cannot create socket\n");
    exit(1); 
  }
  servAdd.sin_family = AF_INET;
  sscanf(argv[2], "%d", &portNumber); 
  servAdd.sin_port = htons((uint16_t)portNumber);
  if(inet_pton(AF_INET, argv[1],&servAdd.sin_addr)<0)
  { 
    fprintf(stderr, " inet_pton() has failed\n"); 
    exit(2);
  }
  if(connect(server, (struct sockaddr *) &servAdd,sizeof(servAdd))<0)
  {
    fprintf(stderr, "Unable to create connection, exiting\n");
    exit(3);
  }
  read(server, message, 255);
  fprintf(stderr, "message received: %s\n", message);
  pid=fork();
  if(pid)
    while(1) /* reading server’s messages */
      if(n=read(server, message, 255))
      {
        fprintf(stderr,"%s\n", message);
        if(!strcasecmp(message, "Bye\n"))
        {
          kill(pid, SIGTERM);
          exit(0); 
        }
      }
  else while(1) // sending messages to server
      if(n=read(0, message, 255))
      {
         write(server, message, strlen(message)+1);
         if(!strcasecmp(message, "Bye\n"))
        {
          kill(getppid(), SIGTERM);
          exit(0);
        }
       }
}