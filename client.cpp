/* This page contains a client program that can request a file from the server program
* on the next page. The server responds by sending the whole file.
*/
#include <bits/stdc++.h> // remember to remove it!
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "urlParser.cpp"

//#define SERVER_PORT 8080 /* arbitrary, but client & server must agree */
#define BUFSIZE 4096 /* block transfer size */
using namespace std;

int main(int argc, char **argv)
{
    int c, s, bytes;
    char buf[BUFSIZE];          /* buffer for incoming file */
    struct hostent *h;          /* info about server */
    struct sockaddr_in channel; /* holds IP address */

    // parsedURL[0] := protocol (ex.: http)  |  parsedURL[2] := port
    // parsedURL[1] := hostname              |  parsedURL[3] := resource
    vector<string> parsedURL = urlParser(argv[1]);

    if (parsedURL.size() != 4) {
        printf("Incorrect URL! Correct URL example: http://localhost.com:8080/index.html");
        exit(-1);
    }
    
    h = gethostbyname(&parsedURL[1][0]); /* look up host’s IP address */
    if (!h) {
        printf("gethostbyname failed to locate %s", &parsedURL[1][0]);
        exit(-1);
    }

    s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0) {
        printf("socket call failed");
        exit(-1);
    }
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
    uint16_t server_port = stoi(parsedURL[2]);
    channel.sin_port = htons(server_port);
    c = connect(s, (struct sockaddr *)&channel, sizeof(channel));
    if (c < 0) {
        printf("connect failed");
        exit(-1);
    }
    /* Connection is now established. Send file name including 0 byte at end. */
    write(s, &parsedURL[3][0], parsedURL[3].size() + 1);
    /* Go get the file and write it to standard output.*/
    while (1) {
        bytes = read(s, buf, BUFSIZE); /* read from socket */
        if (bytes <= 0)
            exit(0);          /* check for end of file */
        write(1, buf, bytes); /* write to standard output */
    }
}
