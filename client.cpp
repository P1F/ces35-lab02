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
#include "responseParser.cpp"

//#define SERVER_PORT 8080 /* arbitrary, but client & server must agree */
#define BUFSIZE 4096 /* block transfer size */
using namespace std;

int main(int argc, char **argv)
{
    int c, s, bytes;
    char buf[BUFSIZE] = "";          /* buffer for incoming file */
    struct hostent *h;          /* info about server */
    struct sockaddr_in channel; /* holds IP address */

    vector<string> parsedURL = urlParser(argv[1]);
    if (!parsedURL.size())
        return 0;
    string hostname = parsedURL[1];
    string port = parsedURL[2];
    string resource = parsedURL[3];

    if (parsedURL.size() != 4)
    {
        printf("Incorrect URL! Correct URL example: http://localhost.com:8080/index.html");
        exit(-1);
    }
    h = gethostbyname(&hostname[0]); /* look up host’s IP address */
    if (!h)
    {
        printf("gethostbyname failed to locate %s", &hostname[0]);
        exit(-1);
    }

    s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0)
    {
        printf("socket call failed");
        exit(-1);
    }
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
    uint16_t server_port = stoi(port);
    channel.sin_port = htons(server_port);
    c = connect(s, (struct sockaddr *)&channel, sizeof(channel));
    if (c < 0)
    {
        printf("connect failed");
        exit(-1);
    }
    printf("connecting...\n");

    string protocol = "HTTP/1.0";
    string http_get = "GET /" + resource + " " + protocol + "\r\nHost:" + hostname + "\r\n\r\n";
    string filename = getFilename(resource);
    /* Connection is now established. Send file name including 0 byte at end. */
    write(s, &http_get[0], http_get.size() + 1);
    /* Go get the file and write it to standard output.*/
    string output = "";

    while (1)
    {
        bytes = read(s, buf, BUFSIZE); /* read from socket */
        if (bytes <= 0)
            break; /* check for end of file */
        output += buf;
    }

    vector<string> response = getResponse(output);
    if (response[0] == "200"){
        FILE *outFile = fopen(&filename[0], "w");
        string body = response[2];
        fprintf(outFile, "%s", &body[0]);
        fclose(outFile);
    }

    printf("%s%s\n", &response[1][0], &response[2][0]);

    close(s);
    exit(-1);
}

/* 
    TODO:
    - Pegar STATUS CODE
        - SE 404 ou 400
            - write no console mensagem de erro
        - SE 200
            - Fazer o parsing do body (talvez pelo '\r\n\r\n')
            - write no fd do arquivo a ser gerado
*/