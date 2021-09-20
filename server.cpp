/* This is the server code */
 // remember to remove it!
#include <iostream>
#include <thread>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "urlParser.cpp"
#include "requestParser.cpp"

//#define SERVER_PORT 8080 /* arbitrary, but client & server must agree*/
#define BUF_SIZE 4096 /* block transfer size */
#define QUEUE_SIZE 10
using namespace std;

void printHello(int sa, string dir)
{
   cout << " SocketAccept ID : " << sa << endl;
   //sleep(10);
   int bytes;
   char buf[BUF_SIZE];         /* buffer for outgoing file */
   read(sa, buf, BUF_SIZE); /* read file name from socket */
   string source = getRequestSource(buf, dir);
   cout << "dir + source: " << source << "\n";
   /* Get and return the file. */
   int fd = open(&source[0], O_RDONLY); /* open the file to be sent back */
   if (fd < 0)
      printf("open failed");
   string httpResponse = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Length: 128\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
   write(sa, &httpResponse[0], httpResponse.size());
   while (1)
   {
      bytes = read(fd, &source[0], BUF_SIZE); /* read from file */
      if (bytes <= 0)
         break; /* check for end of file */

      /*
            TODO:
               - acrescentar cabeçalho HTTP no response
               - add multithreathling
         */
      write(sa, &source[0], bytes); /* write bytes to socket */
   }
   close(fd); /* close file */
   close(sa); /* close connection */
   printf("ble");
   printf("bla");
};

int main(int argc, char *argv[])
{
   char *host_name = argv[1];
   char *port = argv[2];
   char *dir = argv[3];

   struct hostent *he;
   he = gethostbyname(host_name);
   in_addr ip_address = *((struct in_addr *)he->h_addr_list[0]);

   int s, b, l, sa, on = 1;

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
      printf("socket call failed");
      exit(-1);
   }
   setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

   b = bind(s, (struct sockaddr *)&channel, sizeof(channel));
   if (b < 0)
   {
      printf("bind failed");
      exit(-1);
   }

   l = listen(s, QUEUE_SIZE); /* specify queue size */
   if (l < 0)
   {
      printf("listen failed");
      exit(-1);
   }
   /* Socket is now set up and bound. Wait for connection and process it. */
   while (1)
   {
      cout << "Trying connection" << "\n";
      sa = accept(s, 0, 0); /* block for connection request */

      if (sa < 0)
      {
         printf("accept failed");
         exit(-1);
      }

      cout << "Connection accepted -> sa = " << sa << endl;

      thread t(printHello, sa, dir);
      t.join();
   }
}
