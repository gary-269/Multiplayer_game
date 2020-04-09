#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<signal.h>
#include <time.h> 
#include <sys/ipc.h>
#include <arpa/inet.h>

void player(int server, int pid, char *playerName)
{
  int dice, n;
  pid = fork();

  if(pid)
  { 
    while(1)
    { 
      char message[255] = {0};
      if((n=read(server, message, 255)))
      {
        fprintf(stderr,"%s\n", message);
        if(!strcasecmp(message, "Game over: you won the game"))
        {
          fprintf(stderr,"I won the game\n");
          kill(pid, SIGTERM);
          exit(0);
        }
        else if(!strcasecmp(message, "Game over: you lost the game"))
        {
          fprintf(stderr,"I lost the game\n");
          kill(pid, SIGTERM);
          exit(0);
        }  
      }
     }
  }

  else
  {  
    send(server , playerName , strlen(playerName), 0 ); 
    while(1)
    { 
      sleep(2);
      dice = (rand() % 6) + 1;
      fprintf(stderr,"%s : got %d points\n\n", playerName, dice);
      send(server, &dice, sizeof(dice), 0);
    }
  }
}

int socketConnector(char *ipAddress, char *port)
{
  int portNumber, server;
  struct sockaddr_in servAdd;
  if((server=socket(AF_INET, SOCK_STREAM, 0))<0)
  {
    fprintf(stderr, "Cannot create socket\n");
    exit(1);
  }
  servAdd.sin_family = AF_INET;
  sscanf(port, "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber); 

  if(inet_pton(AF_INET, ipAddress,&servAdd.sin_addr)<0)
  { 
    fprintf(stderr, " inet_pton() has failed\n");
    exit(2);
  }
  if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0)
  {
    fprintf(stderr, "connect() has failed, exiting\n");
    exit(3);
  }

  return server;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int server, pid;
    char welcomeMessage[255] = {0};
    if(argc != 4)
    {
      printf("Call model: %s <Player Name> <IP> <Port #>\n", argv[0]);
      exit(0);
    }

    server = socketConnector(argv[2], argv[3]);

    read(server, welcomeMessage, 255);
    fprintf(stderr, "%s\n\n", welcomeMessage);
    sleep(2);
    player(server, pid, argv[1]);
}