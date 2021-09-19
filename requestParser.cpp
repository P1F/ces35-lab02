#include <bits/stdc++.h>

using namespace std;

string getRequestSource(char request[], string dir)
{
    char *token;
    token = strtok(request, " ");
    token = strtok(NULL, " ");
    cout << dir + token << "\n";
    return dir + token;
}

// int main()
// {
//     char str[] = "GET /index.html HTTP/1.0\r\nHost:127.0.0.1\r\n\r\n";
//     string dir = "/home/duduita";
//     string test = getRequestSource(str, dir);
//     return 0;
// }