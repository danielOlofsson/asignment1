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
#include <calcLib.h>
// Enable if you want debugging to be printed, see examble below.
// Alternative, pass 
#define DEBUG
#define BUFFERSIZE 256
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

  if(argc!=2)
  {
    printf("(ip argument missing) Usage; %s <ip>:<port> /n", argv[0]);
    exit(1);
  }

  initCalcLib();

  /*strcat test
  float test = 1.1;
  char testchar[20];
  memset(testchar,0,sizeof(testchar));
  sprintf(testchar,"%f",test);
  strcat(testchar,"\n");

  printf("Test av strcat kolla efter nerhopp %s", testchar);
  */


  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */


  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  
  printf("Host %s, and port %s.\n", Desthost, Destport);

  // *Desthost now points to a string holding whatever came before the delimiter, ':'. aka ip
  // *Dstport points to whatever string came after the delimiter. aka port

  int recivedValue;
  int numbytes;
  char buf[BUFFERSIZE];
  char s[INET6_ADDRSTRLEN];
  struct addrinfo *serverinfo, *p;
  struct addrinfo hints;
  char operation[100];
  char resultchar[100];
  memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
  

  if ((recivedValue = getaddrinfo(argv[1], Destport, &hints, &serverinfo)) != 0) 
  {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(recivedValue));
		return 1;
	}
  
  int client_socket;


  for(p = serverinfo; p != NULL; p = p->ai_next) 
  {
	  if ((client_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
			perror("talker: socket");
		  continue;
		}
	break;
	}

  if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}
  //ansluta till server
  if (connect(client_socket,p->ai_addr, p->ai_addrlen) < 0 ) {
	  perror("connecton error .\n");
	  exit(1);
	}

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		  s, sizeof s);
	printf("client: connecting to %s\n", s);


	freeaddrinfo(serverinfo);

  while(1)
  {
  memset(&buf, 0, sizeof(buf));
  
  if ((numbytes = recv(client_socket, buf, sizeof(buf), 0)) == -1) {
	  perror("recv error");
	  exit(1);
	}
	
	//buf[numbytes] = '\0';
	
  if(numbytes <= 0)
  {
    break;
  }

	printf("client: received %s",buf);

  if(strcmp(buf, "TEXT TCP 1.0\n\n") == 0)
  {
    memset(&buf, 0, sizeof(buf));
    strncpy(buf,"OK\n",sizeof(buf));
    if ((numbytes = send(client_socket, buf, sizeof(buf), 0)) == -1) 
    {
	    perror("sendto:");
	    exit(1);
	  }

	  printf("sent %d bytes and string:%s\n", numbytes, buf);

    //reciving <operation><value><value> to buf
    memset(&buf, 0, sizeof(buf));
    if ((numbytes = recv(client_socket, buf, sizeof(buf), 0)) == -1) 
    {
	    perror("recv error");
	    exit(1);
	  }

    printf("client: received %s",buf);
    sscanf(buf,"%s",operation);
    printf("Inlasta Scanf charen = %s", operation);

    if(buf[0]=='f') 
    {
      double value1, value2, result;

      sscanf(buf,"%s%lg%lg",operation, &value1, &value2);
      
      printf("\n\nvalue1 = %8.8g\nvalue2 = %8.8g\n", value1,value2);

      if(strcmp(operation,"fadd") == 0)
      {
        result = value1 + value2;
        printf("\nResult char add: %8.8g",result);
        sprintf(resultchar,"%8.8g",result);
       
        strcat(resultchar,"\n");
        
      }
      else if(strcmp(operation,"fdiv") == 0)
      {
        result = (value1/value2);
        printf("Float div blev: %8.8g\n",result);
        sprintf(resultchar,"%8.8g",result);
        strcat(resultchar,"\n");
        

      }
      else if(strcmp(operation,"fmul") == 0)
      {
        result = value1*value2;

        printf("Float mul blev: %8.8g\n",result);
        sprintf(resultchar,"%8.8g",result);
        strcat(resultchar,"\n");

      }
      else if(strcmp(operation,"fsub") == 0)
      {
        result = value1 - value2;

        printf("Float subtraktionen blev: %8.8g\n",result);
        sprintf(resultchar,"%8.8g",result);
        strcat(resultchar,"\n");
      }
      else
      {
        perror("sendto error: did not find proper operation");
      }
    //Se om de fungerar.
    printf("\nresult with hoppfully backspace: %sEND", resultchar);

    if ((numbytes = send(client_socket, resultchar, strlen(resultchar), 0)) == -1) 
    {
	    perror("sendto error: float");
	    exit(1);
	  }
    printf("sent %d bytes and string:%s\n", numbytes, resultchar);
    }
    else
    {
      int valueInt1 = 0, valueInt2 = 0, resultInt = 0;
      sscanf(buf,"%s%d%d",operation, &valueInt1, &valueInt2);
      
      printf("\n\nvalue1 = %d\nvalue2 = %d\n", valueInt1,valueInt2);

      if(strcmp(operation,"add")==0){
        resultInt=valueInt1+valueInt2;
      } 
      else if (strcmp(operation, "sub")==0)
      {
        resultInt=valueInt1-valueInt2;
      
      } 
      else if (strcmp(operation, "mul")==0)
      {
       resultInt=valueInt1*valueInt2;
      } 
      else if (strcmp(operation, "div")==0)
      {
      
      resultInt=valueInt1/valueInt2;
    } 
    else {
      printf("No match\n");
    }
    
    sprintf(resultchar,"%d",resultInt);
    strcat(resultchar,"\n");

    if ((numbytes = send(client_socket, resultchar, strlen(resultchar), 0)) == -1) 
    {
	    perror("sendto error: float");
	    exit(1);
	  }
    printf("sent %d bytes and string:%s\n", numbytes, resultchar);
    }
  }
  
  if(strcmp(buf, "\n") == 0)
  {
    exit(2);
  } 

  }

  /* Do magic */
  int port=atoi(Destport);
#ifdef DEBUG 
  printf("Host %s, and port %d.\n",Desthost,port);
#endif

  
}
