#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include <netdb.h>

#define BUF_SIZE 2048

int main (int argc, char **argv)
{
    int result;
    int socketFd;               /* TCP/IP socket descriptor */

    /* structures for use with getaddrinfo() */
    struct addrinfo hints;      /* hints for getaddrinfo() */
    struct addrinfo *servInfo;  /* list of info returned by getaddrinfo() */
    struct addrinfo *p;         /* pointer for iterating list in servInfo */

    /* argv[1] is host name, argv[2] is port number, make sure we have them */
    if (argc != 3)
    {
        fprintf(stderr,
            "Usage:  %s <server hostname or address> <port number>\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));

    /* type of server we're looking for */
    hints.ai_family = AF_INET;          /* internet address family */
    hints.ai_socktype = SOCK_STREAM;    /* stream sock */
    hints.ai_protocol = IPPROTO_TCP;    /* tcp/ip protocol */
    hints.ai_flags = AI_CANONNAME;      /* include canonical name */

    /* get a linked list of likely servers pointed to by servInfo */
    result = getaddrinfo(argv[1], argv[2], &hints, &servInfo);

    if (result != 0)
    {
        fprintf(stderr, "Error getting addrinfo: %s\n", gai_strerror(result));
        exit(EXIT_FAILURE);
    }


    printf("Trying %s...\n", argv[1]);
    p = servInfo;

    while (p != NULL)
    {
        /* use current info to create a socket */
        socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (socketFd >= 0)
        {
            /***************************************************************
            * We got the socket we asked for try to connect.
            *
            * NOTE: connect() has an unspecified time out.  For a good
            * a sample of connecting with a timeout see
            * http://developerweb.net/viewtopic.php?id=3196 for
            ***************************************************************/
            result = connect(socketFd, p->ai_addr, p->ai_addrlen);

            if (result != 0)
            {
                /* this socket wouldn't except our connection */
                close(socketFd);
            }
            else
            {
                /* we're connected, get out of this loop */
                break;
            }
        }

        p = p->ai_next;     /* try next address */
    }

    if (NULL == p)
    {
        /* we never found a server to connect to */
        fprintf(stderr, "Unable to connect to server.\n");
        freeaddrinfo(servInfo);
        exit(EXIT_FAILURE);
    }

    printf("Connected to %s\n", p->ai_canonname);
    freeaddrinfo(servInfo);     /* we're done with this */

    /***********************************************************************
    * send messages to echo server and receive echos until user sends empty
    * message or the server disconnects.
    ***********************************************************************/

    char buffer[BUF_SIZE];

    ssize_t rcv_len;

    while (1){
	if(rcv_len = recv(socketFd, buffer, BUF_SIZE-1, 0) >0){
		printf("%s\r\n", buffer);
    	}
	else if(rcv_len == 0 && errno == EWOULDBLOCK){
		close(socketFd);
		return EXIT_SUCCESS;
    	}
    }
}
