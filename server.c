#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

void servicePlayers(player1, player2)
{
  char message[255] = {0};
  int n, pid;
  write(player1, "You can now play TOTO", 16);
  if(pid=fork()) /* reading client messages */
  while(1)
    if(n=read(player1, message, 255))
    {
      fprintf(stderr,"%s\n", message);
      if(!strcasecmp(message, "Bye\n"))
      {
      kill(pid, SIGTERM);
      exit(0); 
      }
    }
  write(player2, "You can now play TITI", 16);
  if(pid=fork()) /* reading client messages */
  while(1)
    if(n=read(player2, message, 255))
    {
      fprintf(stderr,"%s\n", message);
      if(!strcasecmp(message, "Bye\n"))
      {
      kill(pid, SIGTERM);
      exit(0); 
      }
    }
}

int socketcreator(char *port)
{
  int sd, portNumber;
  struct sockaddr_in servAdd;
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if(sd <0)
  {
    printf("Cannot create socket\n");
    exit(1); 
  }
  servAdd.sin_family = AF_INET; 
  servAdd.sin_addr.s_addr = htonl(INADDR_ANY); 
  sscanf(port, "%d", &portNumber); 
  servAdd.sin_port = htons((uint16_t)portNumber);
  int binding = bind(sd,(struct sockaddr*)&servAdd,sizeof(servAdd));
  if(binding<0)
  {
    perror("Unable to bind");
    exit(1);
  }
  listen(sd, 5);
  return sd;
}
int main(int argc, char *argv[])
{ 
  int sd, player1, player2, status;
  socklen_t len;
  if(argc != 2)
  {
    printf("Call model: %s <Port #>\n", argv[0]);
    exit(0);
  }
  sd = socketcreator(argv[1]);

  printf("Hi, Refree here\n");
  printf("This is a 2 player game\n");

  while(1)
  {
   player1 = accept(sd,(struct sockaddr *)NULL,NULL);
   player2 = accept(sd,(struct sockaddr *)NULL,NULL);

   printf("Got both players\n");

   if(!fork())
    servicePlayers(player1, player2);
    close(player1);
    close(player2);
    waitpid(0, &status, WNOHANG);
  }
}
    