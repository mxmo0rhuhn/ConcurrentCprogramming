/*
 * Uebung 12 - Systemprogrammierung
 * Max Schrimpf 
 * Hausaufgabe 7
 *
 * Client:
 * - Einfache Moeglichkeit Strings ueber TCP IP an einen Server zu senden
 *
 * Basiert auf den im GitHub angegebenen Beispielen
 */

/* for sockaddr_in and inet_addr() */
#include <arpa/inet.h>  
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
/* for printf() and fprintf() and ... */
#include <stdio.h>      
/* for atoi() and exit() and ... */
#include <stdlib.h>     
/* for memset() and ... */
#include <string.h>     
/* for socket(), connect(), send(), and recv() */
#include <sys/socket.h> 
#include <sys/types.h>
/* for close() */
#include <unistd.h>     

#include <termPaperLib.h>
#include <transmission-protocols.h>

char TERM_SYMBOL='\000';

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server IP> [-c Command] [-p Port]\n", argv0);
  printf("Connects to the Server and provides an interactive Commandline\n");
  printf("You may also provide a certain command that is to be executed\n");
  printf("End with Ctrl-C or QUIT\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  /* Test for correct number of arguments */
  if (argc < 2 || argc > 6) {    
    usage(argv[0], "wrong number of arguments please provide an IP at least");
  }

  /* Socket descriptor */
  int sock;                        
  /* Square server address */
  struct sockaddr_in server_address; 
  /* Square server port */
  unsigned short server_port;     
  /* Server IP address (dotted quad) */
  char *server_ip;                    
  /* First arg: server IP address (dotted quad) */
  server_ip = argv[1];             
  int interactive = TRUE; 
  char *cmd;
  int cmdLen;

  // Skip program name & IP 
  server_port = 7000;  
  int i;
  for (i = 2; i < argc; i++)  {
    if (strcmp(argv[i], "-p") == 0)  {
      if (i + 2 <= argc )  {
        i++;
        server_port = atoi(argv[i]);  
      } else {
        usage(argv[0], "please provide a port if you're using -p");
      }
    } else if (strcmp(argv[i], "-c") == 0)  {
      if (i + 2 <= argc )  {

        interactive = FALSE;
        i++;
        cmd = argv[i];
        cmdLen = strlen(cmd);
      } else {
        usage(argv[0], "please provide a CMD if you're using -c");
      }
    } else {
      usage(argv[0], "unknown input");
    }
  }

  int firstRun = TRUE;
  while (firstRun || interactive ) {
    /* Create a reliable, stream socket using TCP */

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    handle_error(sock, "socket() failed", PROCESS_EXIT);

    /* Construct the server address structure */

    /* Zero out structure */
    memset(&server_address, 0, sizeof(server_address));     
    /* Internet address family */
    server_address.sin_family      = AF_INET;             
    /* Server IP address */
    server_address.sin_addr.s_addr = inet_addr(server_ip);   
    /* Server port: htons host to network byte order */
    server_address.sin_port        = htons(server_port); 

    retcode = connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));
    handle_error(retcode, "connect() failed\n", PROCESS_EXIT);

    char *send;
    if (interactive  != TRUE){
      send = cmd;
    } else {
      char line[40];
      printf("CMD=");
      fflush(stdout);
      fgets(line, 40, stdin);
      int n = strlen(line);

      if (n == 0) {
        continue;
      }
      if (strcmp(line, "QUIT\n") == 0) {
        printf("terminated by user\n");
        break;
      }
      // letztes Zeichen \n
//      line[(n-1)] = TERM_SYMBOL;
      strtok(line, "\n");
      send = line;
    }
    printf("Sendling: '%s'\n", send);
    write_string(sock, send, -1);

    while(TRUE) {
      char *buffer_ptr[1];
      size_t received_msg_size = read_and_store_string(sock, buffer_ptr);
      handle_error(received_msg_size, "recive failed", PROCESS_EXIT);

      if (received_msg_size == 0) {
        printf("terminated by server\n");
        break;
      }

      printf("Response=%s \n", *buffer_ptr);
      free(*buffer_ptr);
    }
    close(sock);
    firstRun = FALSE;
  }
  sleep(1);
  exit(0);
}
