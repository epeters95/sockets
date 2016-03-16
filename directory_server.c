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
#define QUERY_CONTENT 2
#define EXIT 3

///////////////////////////
// structs for P2P part
// ONLY used for GET request
struct request {
    char hostname[MAX_HOSTNAME];    // client's host name
    char client[16];                // client ip address
    int req;                        // request number
    char requested_file[MAX_FILENAME_LEN];  // filename
};

struct response {
    char file[MAXBUF - 4];      // 124 byte files allowed
    int status;                 // e.g. 200, 400, 404, 505
};

//////////////////////////
// structs for directory server part
// struct request {
//     int message_type;
//     char hostname[MAX_HOSTNAME];
//     char client[16];
//     char content[MAX_LINESIZE];     // will contain entry if message type is Inform and Update
//     int req;    // request number
// };

// struct response {
//     char status[13];        // 200 (OK) or 400 (ERROR)
//     char content[MAX_LINES][MAX_LINESIZE];   // array of "hostname:filename.txt:size" strings
// };


///////////////////////
// server's filename directory
int lastentry = 0;
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

                // /* Block until receive message from a client */
                // struct request r;
                // if ((recvMsgSize = recvfrom(sock, &r, MAXBUF, 0,
                //         (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
                //         DieWithError("recvfrom() failed");

                // if (r.message_type == INFORM_UPDATE) {

                //     // perform inform and update on server directory
                // }
                // else if (r.message_type == QUERY_CONTENT) {

                //     // query the directory for files from a given host
                // }
                // else if (r.message_type == QUIT) {

                //     // remove requesting client's entries from directory
                // }

                
        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, echoBuffer, MAXBUF, 0,
            (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        /* Send received datagram back to the client */
        if (sendto(sock, echoBuffer, recvMsgSize, 0, 
             (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
            DieWithError("sendto() sent a different number of bytes than expected");

        }
}
void HandleTCPClient(int clntSocket)
{
    char echoBuffer[MAXBUF];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */

    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, echoBuffer, MAXBUF, 0)) < 0)
        DieWithError("recv() failed");

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0)      /* zero indicates end of transmission */
    {

        // now send the file here.  You would actually check for file, read, and send as a response
        char responseBuffer[50000];
        strcpy(responseBuffer, "Thie is file content A...\n");
        int responseBufferSize = strlen(responseBuffer);

        /* Echo message back to client */
        if (send(clntSocket, responseBuffer, responseBufferSize, 0) != responseBufferSize)
            DieWithError("send() failed");

        /* See if there is more data to receive */
        //if ((recvMsgSize = recv(clntSocket, echoBuffer, MAXBUF, 0)) < 0)
        //    DieWithError("recv() failed");

    // You could wait for more command, but, in this case, we can just terminate the socket

    break;
    }

    close(clntSocket);    /* Close client socket */
}
int main()
{
        unsigned short serverPort;
                        printf("please enter a server port ");
                        scanf("%hu", &serverPort);
                        server(serverPort);
        return 0;
}
                                   
