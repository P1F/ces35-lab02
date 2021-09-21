/*
    Alunos: Eduardo Menezes e Rodrigo Teixeira
    CES35 LAB 2
    Server code
*/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "urlParser.cpp"
#include "requestParser.cpp"

#define BUF_SIZE 4096 /* block transfer size */
#define QUEUE_SIZE 10
using namespace std;

struct thread_data
{
    int *total;               // number of threads runnning simultaneously
    int sa_id;                // socket accept ID
    string dir;               // requested file directory
    bool is_occupied = false; // check if thread is being occupied
};

void *sendData(void *thread_arg)
{
    struct thread_data *td;
    td = (struct thread_data *)thread_arg;
    *td->total = *td->total + 1;
    printf("SocketAccept ID --> %d\n", td->sa_id);

    int bytes;
    char request_buf[BUF_SIZE];                                        /* buffer for outgoing file */
    read(td->sa_id, request_buf, BUF_SIZE);                            /* read file name from socket */
    string source = getRequestSource(request_buf, td->dir, td->sa_id); /* file directory */
    /* Get and return the file. */
    int fd = open(&source[0], O_RDONLY); /* open the file to be sent back */
    /* Calculate file content length. */
    ifstream file(source, ios::binary | ios::ate);
    string content_length = to_string((int)file.tellg());

    bool isOK = false;

    if (source.empty()) // bad request 400 status code
    {
        string bad_request = "HTTP/1.1 400 Bad Request\r\nAccept-Ranges: bytes\r\nContent-Length: 173\r\nContent-Type: text/html; charset=iso-8859-1\r\n\r\n";
        bad_request += "<html><head>\r\n<title>400 Bad Request</title>\r\n</head><body>\r\n<h1>Bad Request</h1>\r\n<p>Your browser sent a request that this server could not understand.<br />\r\n</p>\r\n</body></html>\r\n";
        write(td->sa_id, &bad_request[0], bad_request.size() + 1);
    }
    else
    {
        if (fd < 0 or source == (td->dir + "/")) // invalid request 404 status code
        {
            printf("On socket [%d]: file not found!\n", td->sa_id);
            string not_found = "HTTP/1.1 404 Not Found\r\nAccept-Ranges: bytes\r\nContent-Length: 142\r\nContent-Type: text/html; charset=iso-8859-1\r\n\r\n";
            not_found += "<html><head>\r\n<title>404 Not Found</title>\r\n</head><body>\r\n<h1>Not Found</h1>\r\n<p>The requested URL was not found on this server.</p>\r\n</body></html>\r\n";
            write(td->sa_id, &not_found[0], not_found.size() + 1);
        }
        else // 200 status code
        {
            isOK = true;
            printf("On socket [%d]: file found! Sending to client socket...\n", td->sa_id);
            string msg = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: " + content_length + "\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
            char http_header[msg.length()];
            strcpy(http_header, msg.c_str());
            /* write response header */
            write(td->sa_id, http_header, sizeof(http_header));
        }
    }
    if (isOK)
    {
        char body_buf[4096] = "";
        /* write response body */
        while (1)
        {
            bytes = read(fd, body_buf, BUF_SIZE); /* read from file */
            if (bytes <= 0)
                break; /* check for end of file */

            write(td->sa_id, body_buf, bytes + 1); /* write bytes to socket */
        }
    }

    close(fd);        /* close file */
    close(td->sa_id); /* close connection */
    printf("Connection closed on socket ID [%d] !\n", td->sa_id);

    *td->total = *td->total - 1;
    td->is_occupied = false;

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    char *host_name = argv[1];
    char *port = argv[2];
    char *dir = argv[3];

    struct hostent *he;
    he = gethostbyname(host_name);

    int s, b, l, sa, on = 1;
    int total = 0;

    struct sockaddr_in channel; /* holds IP address */
    /* Build address structure to bind to socket. */
    memset(&channel, 0, sizeof(channel));
    /* zero channel */
    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY);
    channel.sin_port = htons(atoi(port));
    /* Passive open. Wait for connection. */
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* create socket */
    if (s < 0)
    {
        printf("socket call failed!");
        exit(-1);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

    b = bind(s, (struct sockaddr *)&channel, sizeof(channel));
    if (b < 0)
    {
        printf("bind failed!");
        exit(-1);
    }

    pthread_t threads[QUEUE_SIZE];
    struct thread_data td[QUEUE_SIZE];

    l = listen(s, QUEUE_SIZE); /* specify queue size */
    if (l < 0)
    {
        printf("listen failed!");
        exit(-1);
    }
    /* Socket is now set up and bound. Wait for connection and process it. */
    while (1)
    {
        printf("Trying connection...\n");

        sa = accept(s, 0, 0); /* block for connection request */

        if (sa < 0)
        {
            printf("accept failed!");
            exit(-1);
        }

        printf("accept successfully on socket [%d] !\n", sa);

        int rc;
        if (total < 10)
        {
            int i = total;
            while (td[total].is_occupied)
            {
                i = (i + 1) % QUEUE_SIZE;
            }
            td[i].is_occupied = true;
            td[i].sa_id = sa;
            td[i].dir = dir;
            td[i].total = &total;

            rc = pthread_create(&threads[i], NULL, sendData, (void *)&td[i]);
            if (rc)
            {
                printf("Error:unable to create thread [%d] of SocketAccept ID [%d]\n", rc, sa);
                exit(-1);
            }
        }
    }
}
