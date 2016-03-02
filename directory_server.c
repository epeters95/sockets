#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <unistd.h>     /* for close() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

#define MAXBUF 128
#define MAX_USERS 10
#define MAX_FILENAME_LEN 100
#define MAX_HOSTNAME 20
#define MAX_ENTRIES 100

// per "Query for Content" request
#define MAX_LINESIZE 23
#define MAX_LINES 5

///////////////////////
// message types, for all files
#define INFORM_UPDATE 1
#define QUERY 2
#define EXIT 3

///////////////////////////
// structs for P2P part
// ONLY used for GET request
struct request {
    char hostname[MAX_HOSTNAME];    // client's host name
    char client[16];    // client ip address
    int req;            // request number
    char requested_file[MAX_FILENAME_LEN];  // filename
}

struct response {
    char file[MAXBUF - 4];      // 124 byte files allowed
    int status;     // e.g. 200, 400, 404, 505
}
//////////////////////////
// structs for directory server part
struct request {
    int message_type;
    char hostname[MAX_HOSTNAME];
    char client[16];
    char content[MAX_LINESIZE];     // will contain entry if message type is Inform and Update
    int req;    // request number
}

struct response {
    char status[13];        // 200 (OK) or 400 (ERROR)
    char content[MAX_LINES][MAX_LINESIZE];   // array of "hostname:filename.txt:size" strings
}



///////////////////////
// server's filename directory
char directory[MAX_ENTRIES][MAX_FILENAME_LEN];
// arr of strings "hostname:filename:size"

char hosts[MAX_USERS][MAX_HOSTNAME];
// arr of strings "hostname" indexed by client number
char ips[MAX_USERS][16];
// arr of ip addresses indexed by client number

////////////////////////

void DieWithError(const char *errorMessage) /* External error handling function */
{
    perror(errorMessage);
    exit(1);
}

// void client(const char* servIP, const char* echoString, unsigned short echoServPort)
// {
//         int sock;                        /* Socket descriptor */
//         struct sockaddr_in echoServAddr; /* Echo server address */
//         struct sockaddr_in fromAddr;     /* Source address of echo */
//         unsigned int fromSize;           /* In-out of address size for recvfrom() */
//         char echoBuffer[MAXBUF + 1];      /* Buffer for receiving echoed string */
//         int echoStringLen;               /* Length of string to echo */
//         int respStringLen;               /* Length of received response */

//         if ((echoStringLen = strlen(echoString)) > MAXBUF)  /* Check input length */
//                 DieWithError("Echo word too long");

//         printf("%s\n",servIP );
//         /* Create a datagram/UDP socket */
//         if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
//                 DieWithError("socket() failed");

//         /* Construct the server address structure */
//         memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
//         echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
//         echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
//         echoServAddr.sin_port = htons(echoServPort);     /* Server port */

//         /* Send the string to the server */
//  if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)
//                 &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
//                 DieWithError("sendto() sent a different number of bytes than expected");
//         /* Recv a response */
//         fromSize = sizeof(fromAddr);
//         if ((respStringLen = recvfrom(sock, echoBuffer, MAXBUF, 0,
//                 (struct sockaddr *) &fromAddr, &fromSize)) != echoStringLen)
//                 DieWithError("recvfrom() failed");

//         if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
//         {
//                 fprintf(stderr, "Error: received a packet from unknown source.\n");
//                 exit(1);
//         }

//         /* null-terminate the received data */
//         echoBuffer[respStringLen] = '\0';
//         printf("Received: %s\n", echoBuffer);    /* Print the echoed arg */

//         close(sock);
//         exit(0);
// }

void server(unsigned short Port)
{
        int sock;                        /* Socket */
        struct sockaddr_in echoServAddr; /* Local address */
        struct sockaddr_in echoClntAddr; /* Client address */
        unsigned int cliAddrLen;         /* Length of incoming message */
        char echoBuffer[MAXBUF];        /* Buffer for echo string */
        int recvMsgSize;                 /* Size of received message*/
        unsigned short echoServPort = Port;

        /* Create socket for sending/receiving datagrams */
        if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
                DieWithError("socket() failed");

        /* Construct local address structure */
        memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
        echoServAddr.sin_family = AF_INET;                /* Internet address family */
        echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
        echoServAddr.sin_port = htons(echoServPort);      /* Local port */
        /* Bind to the local address */
 if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
                DieWithError("bind() failed");

        for (;;) /* Run forever */
        {
                /* Set the size of the in-out parameter */
                cliAddrLen = sizeof(echoClntAddr);

                /* Block until receive message from a client */
                if ((recvMsgSize = recvfrom(sock, echoBuffer, MAXBUF, 0,
                        (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
                        DieWithError("recvfrom() failed");

                printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

                /* Send received datagram back to the client */
                // if (sendto(sock, echoBuffer, recvMsgSize, 0,
                //         (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
                //         DieWithError("sendto() sent a different number of bytes than expected");
        }
}
int main()
{
        // int option;
        // unsigned short serverPort;
        // char servIPaddress[16];
        // char message[MAXBUF];
        // unsigned short echoPort;
        // printf("Welcome, what would you like to do?\n 1. Inform and update \n 2. Query for content \n 3. Exit \n");
        // scanf("%d", &option);
        // printf("option is: %d \n", option);

        // while(option != 3)
        // {
        //         if(option == 1)
        //         {
        //                 printf("Inform and update\n");
        //                 printf("Please enter a server IP address, message, and port number \n");

        //                 struct request;
        //                 request->message = "";

        //                 scanf("%s %s %hu", &servIPaddress, &message, &echoPort);
        //                 printf("it scanned\n");
        //                 client(servIPaddress, message, echoPort);
        //         }
        //         else if(option == 2)
        //         {
        //                 printf("Query for content\n");
                        printf("please enter a server port ");
                        scanf("%hu", &serverPort);
                        server(serverPort);
        //         }
        //         else
        //         {
        //                 printf("Please enter a valid option\n");
        //         }
        //         printf("What would you like to do? \n");
        //         scanf("%d", &option);
        // }
        return 0;
}
                                   
