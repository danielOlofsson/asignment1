#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* You will to add includes here */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
// Enable if you want debugging to be printed, see examble below.
// Alternative, pass 
#define DEBUG

// hjälpfuntion från simple-tcp-client
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Included to get the support library
#include <calcLib.h>

int main(int argc, char *argv[]){

  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */


  //HEJ
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  
  // *Desthost now points to a string holding whatever came before the delimiter, ':'. aka ip
  // *Dstport points to whatever string came after the delimiter. aka port

  int recivedValue;
  char s[INET6_ADDRSTRLEN];
  struct addrinfo *serverinfo, *p;
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
  

  if ((recivedValue = getaddrinfo(argv[1], Destport, &hints, &serverinfo)) != 0) 
  {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(recivedValue));
		return 1;
	}
  
  int client_socket; //= socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  /*
  if (client_socket == -1)
  {
    perror("Could not create socket\n");
  }
  */

  	for(p = serverinfo; p != NULL; p = p->ai_next) 
    {
		  if ((client_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			  perror("talker: socket");
			  continue;
		}

		break;
	}
  
  if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}

  if (connect(client_socket,p->ai_addr, p->ai_addrlen) < 0 ) {
	  perror("talker2: connect .\n");
	  exit(1);
	}

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		  s, sizeof s);
	printf("client: connecting to %s\n", s);


	freeaddrinfo(serverinfo);

	

  
  /* Do magic */
  int port=atoi(Destport);
#ifdef DEBUG 
  printf("Host %s, and port %d.\n",Desthost,port);
#endif

  
}
