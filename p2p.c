#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
        perror(msg);
        exit(0);
}
void tcpClient()
{
        int sockfd, newsockfd, portno;

        //clilen stores the size of the address of the client. This is required for the accept system call.
        socklen_t clilen;

        //server reads characters from the socket connection into this buffer.
        char buffer[256];

        //sockaddr_in is a structure containing an internet address
        /*
        struct sockaddr_in
        {
        short   sin_family; //must be AF_INET
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8]; // Not used, must be zero
        };
        */
        //in_addr structure, contains only one field, a unsigned long called s_addr.

        //serv_addr will contain the address of the server, and cli_addr will contain the address of the client which connects to the server.
        struct sockaddr_in serv_addr, cli_addr;
        int n;

        //create socket
 //it take three arguments - address domain, type of socket, protocol (zero allows the OS to choose thye appropriate protocols based on type of socket)
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                error("ERROR opening socket");

        //set server address buffer with zeros using bzero or memset
        //two arguments - pointer to buffer and sizeof buffer
        bzero((char *)&serv_addr, sizeof(serv_addr));

        //atoi() function can be used to convert port number from a string of digits to an integer, if your input is in the form of a string.
        portno;
        printf("please enter a port number: ");
        scanf("%d", &portno);
        if (portno < 0)
                error("A port number is required");

        //contains a code for the address family
        serv_addr.sin_family = AF_INET;

        //contains the IP address of the host
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        //contain the port number
        serv_addr.sin_port = htons(portno);

        //bind() system call binds a socket to an address, in this case the address of the current host and port number on which the server will run.
        //three arguments, the socket file descriptor, the address to which is bound, and the size of the address to which it is bound.
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr)) < 0)
                error("ERROR on binding");
        //listen system call allows the process to listen on the socket for connections.
        //The first argument is the socket file descriptor, and second is number of connections that can be waiting while the process is handling a particular connection.
        listen(sockfd, 5);

        clilen = sizeof(cli_addr);

        //accept() system call causes the process to block until a client connects to the server.
        newsockfd = accept(sockfd,
                (struct sockaddr *) &cli_addr,
                &clilen);
 if (newsockfd < 0)
                error("ERROR on accept");

        //After a connection a client has successfully connected to the server
        //initialize the buffer using the bzero() function
        bzero(buffer, 256);

        //reads from the socket into a buffer for a maximum of 255 characters
        //read call uses new file descriptor, the one returned by accept()
        n = read(newsockfd, buffer, 255);
        if (n < 0) error("ERROR reading from socket");

        //both the server can read and write after a connection has been established.
        //everything written by the client will be read by the server, and everything written by the server will be read by the client.
        printf("Here is the message: %s\n", buffer);
        n = write(newsockfd, "I got your message", 18);
        if (n < 0) error("ERROR writing to socket");

        //close connections using file descriptors
        close(newsockfd);
        close(sockfd);

}
void tcpServer(char *servIP, char *fileString)
{
        int sockfd, portno, n;

        //serv_addr will contain the address of the server
        struct sockaddr_in serv_addr;
        struct hostent *server;
        char buffer[256];
        portno = 8888;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                error("ERROR opening socket");
        portno;
        printf("please enter a port number: ");
        scanf("%d", &portno);

        bzero((char *)&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;

        serv_addr.sin_addr.s_addr = inet_addr(servIP);
        serv_addr.sin_port = htons(portno);
        //connect function is called by the client to establish a connection to the server. It takes three arguments, the socket file descriptor, the address of the host to which it wants to connect (including the port number), and the size of this address
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                error("ERROR connecting");
        printf("\nPlease enter the message: ");

        int stringLen = strlen(fileString);
        if (send(sockfd, fileString, stringLen, 0) != stringLen)
        error("send() sent a different number of bytes than expected");

    /* Receive the same string back from the server */
    int totalBytesRcvd = 0;
    int bytesRcvd;
    printf("Received: ");                /* Setup to print the echoed string */
    while (totalBytesRcvd < stringLen)
    {
        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sockfd, buffer, 256 - 1, 0)) <= 0)
            error("recv() failed or connection closed prematurely");
        totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        buffer[bytesRcvd] = '\0';  /* Terminate the string! */
        printf("%s", buffer);      /* Print the echo buffer */
    }

        //close connections using file descriptors
        close(sockfd);
}
void udpServer() {
    int sock;                        /* Socket descriptor */
        struct sockaddr_in echoServAddr; /* Echo server address */
        struct sockaddr_in fromAddr;     /* Source address of echo */
        unsigned int fromSize;           /* In-out of address size for recvfrom() */
        char echoBuffer[256 + 1];      /* Buffer for receiving echoed string */
        int echoStringLen;               /* Length of string to echo */
        int respStringLen;               /* Length of received response */


        char* echoStr;
        char* srv;
        int* len;
        puts("Enter the Server IP: ");

        int bites = getline(&srv, &len, stdin);
        puts("Enter the filename (or nothing to list all): ");
        bites = getline(&echoStr, &len, stdin);
        const char* servIP = (const char*) srv;
        const char* echoString = (const char*) echoStr;


        // if ((echoStringLen = strlen(echoString)) > 256)  /* Check input length */
        //         error("Echo word too long");

        int portno = 8888;
        printf("please enter a port number: \n");
        scanf("%d", &portno);
        /* Create a datagram/UDP socket */
            printf("\ntest\n");
            printf("\ntest w/ %d \n", portno);
            portno += 1;
            printf("finished");
        if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
                error("socket() failed");


        /* Construct the server address structure */
        memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
        echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
        echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
        echoServAddr.sin_port = htons(portno);     /* Server port */

        /* Send the string to the server */
 if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)
                &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
                error("sendto() sent a different number of bytes than expected");
        /* Recv a response */
        fromSize = sizeof(fromAddr);
        if ((respStringLen = recvfrom(sock, echoBuffer, 256, 0,
                (struct sockaddr *) &fromAddr, &fromSize)) != echoStringLen)
                error("recvfrom() failed");

        if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
        {
                fprintf(stderr, "Error: received a packet from unknown source.\n");
                exit(1);
        }

        /* null-terminate the received data */
        echoBuffer[respStringLen] = '\0';
        printf("Received: %s\n", echoBuffer);    /* Print the echoed arg */

        close(sock);
}
int main(int argc, char *argv[])
{
        int option;
        printf("Welcome, what would you like to do?\n 1. Inform and update \n 2. Query for content \n 3. Exit \n");
        scanf("%d", &option);
        printf("option is: %d \n", option);

        while(option != 3)
        {
                if(option == 1)
                {
                        printf("Inform and update\n");
                         tcpClient();
                }
                else if(option == 2)
                {
                        char *servIP;
                        char *fileString;
                        int portno;

                        printf("Query for content\n");
                        udpServer();
                        exit(0);
                }
                else
                {
                        printf("Please enter a valid option\n");
                }
                printf("What would you like to do? \n");
                scanf("%d", &option);
        }
        return 0;
}
                                             
                                                              
