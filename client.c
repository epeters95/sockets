#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define ECHOMAX 255     /* Longest string to echo */

void DieWithError(const char *errorMessage) /* External error handling function */
{
    perror(errorMessage);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
    int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */

    struct request{
      char client_ip[16];
      int inc;
      int client;
      int req;
      char c;
    };

   struct request r;
   strcpy( r.client_ip, "111.222.333.444" );
   r.inc = 0;
   r.client = 1;
   r.req = 4;
   r.c = 'x';

    if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
        exit(1);
    }

    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
    echoString = argv[2];       /* Second arg: string to echo */

      if ((echoStringLen = strlen(echoString)) > ECHOMAX)  /* Check input length */
          DieWithError("client: Echo word too long");

    if (argc == 4)
        echoServPort = atoi(argv[3]);  /* Use given port, if any */
    else
        echoServPort = 7;  /* 7 is the well-known port for the echo service */

    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("client: socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port   = htons(echoServPort);     /* Server port */

    printf("client: before sendto() server with client_ip=%s, inc=%d, client=%d, req=%d, c=%c\n", r.client_ip, r.inc, r.client, r.req, r.c );

    /* Send the string to the server */
    if (sendto(sock, &r, sizeof( struct request), 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != sizeof( struct request ) )
        DieWithError("client: sendto() sent a different number of bytes than expected");
  
    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if ((respStringLen = recvfrom(sock, &r, sizeof( struct request ), 0, 
         (struct sockaddr *) &fromAddr, &fromSize)) != sizeof(struct request) )
        DieWithError("client: recvfrom() failed");

    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
        fprintf(stderr,"client: Error: received a packet from unknown source.\n");
        exit(1);
    }

    printf("client: received echoed request from server with client_ip=%s, inc=%d, client=%d, req=%d, c=%c\n", r.client_ip, r.inc, r.client, r.req, r.c );
    
    close(sock);
    exit(0);
}