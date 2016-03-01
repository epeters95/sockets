#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
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
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */

    struct request{
      char client_ip[16];
      int inc;
      int client;
      int req;
      char c;
    };

   struct request r;

    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("server: socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    char str[50];
    char* str2;
    str2 = inet_ntop(AF_INET, &echoServAddr.sin_addr.s_addr, str, 50);
    printf("Running server at %s\n", str2);

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("server: bind() failed");
    char newstr[50];
    strcpy(newstr, inet_ntop(AF_INET, &echoServAddr.sin_addr.s_addr, tsr2, 50));
    printf("No wait, running server at %s\n", newstr);

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);

        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, &r, sizeof(struct request), 0,
            (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("server: recvfrom() failed");

        printf("server: handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        printf("server: received request from client_ip=%s, inc=%d, client=%d, req=%d, c=%c\n", r.client_ip, r.inc, r.client, r.req, r.c );

        /* Send received datagram back to the client */
        if (sendto(sock, &r, sizeof(struct request), 0, 
             (struct sockaddr *) &echoClntAddr, sizeof( echoClntAddr ) ) != recvMsgSize)
            DieWithError("server: sendto() sent a different number of bytes than expected");
    }
    /* NOT REACHED */
}
