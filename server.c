#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<signal.h>

void servicePlayers(int player1, int player2)
{
  int n, pid;
  char *gameOverMessage1, *gameOverMessage2;
  char *welcomeMessage1, *welcomeMessage2;
  char *turnMessage;
  int dice;
  char namePlayer1[255] = {0};
  char namePlayer2[255] = {0};
  int pointsPlayer1 = 0;
  int pointsPlayer2 = 0;
  int winningPoints = 100;

  gameOverMessage1 = "Game over: You won the game";
  gameOverMessage2 = "Game over: You lost the game";
  welcomeMessage1 = "Welcome: Player1";
  welcomeMessage2 = "Welcome: Player2";
  turnMessage = "You can now play";

  write(player1, welcomeMessage1, 16);
  write(player2, welcomeMessage2, 16);

  if((pid=fork()))
  {
    read(player1, namePlayer1, 255);
    fprintf(stderr, "Player 1 name received: %s\n", namePlayer1);
    read(player2, namePlayer2, 255);
    fprintf(stderr, "Player 2 name received: %s\n", namePlayer2);
    int roundNumber = 1;

    while(1)
    {
      fprintf(stderr,"Round %d starts =>\n\n",roundNumber);
      if(pointsPlayer1 <= winningPoints || pointsPlayer2 <= winningPoints)
      {
        send(player1, turnMessage, strlen(turnMessage), 0); 
        send(player2, turnMessage, strlen(turnMessage), 0); 
      }

      if((n = read(player1, &dice, sizeof(int))))
      { 
        fprintf(stderr,"%s: playing dice\n",namePlayer1);
        fprintf(stderr,"%s: got %d points\n",namePlayer1 ,dice);
        pointsPlayer1+=dice;
        fprintf(stderr,"%s: Total so far: %d\n\n",namePlayer1, pointsPlayer1);

        if(pointsPlayer1 >= winningPoints)
        {
          write(player1, gameOverMessage1, strlen(gameOverMessage1));
          write(player2, gameOverMessage2, strlen(gameOverMessage2));
          kill(pid, SIGTERM);
          kill(getppid(), SIGTERM);
          exit(0); 
        }
      }
  
      if(n == read(player2, &dice, sizeof(int)))
      {
        fprintf(stderr,"%s: playing dice\n",namePlayer2);
        fprintf(stderr,"%s: got %d points\n",namePlayer2 ,dice);
        pointsPlayer2+=dice;
        fprintf(stderr,"%s: Total so far: %d\n\n",namePlayer2, pointsPlayer2);
        if(pointsPlayer2 >= winningPoints)
        {
          send(player2 , gameOverMessage1, strlen(gameOverMessage1), 0); 
          send(player1 , gameOverMessage2, strlen(gameOverMessage2), 0); 
          kill(pid, SIGTERM);
          kill(getppid(), SIGTERM);
          exit(0); 
        }
     }
     roundNumber++;
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
  exit(0);
}
    