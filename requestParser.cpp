#include <iostream>
#include <string>
#include <regex>
using namespace std;

string getRequestSource(char request[], string dir)
{
    string token;
    token = strtok(request, " ");
    if (token != "GET")
    {
        printf("Bad Request, this server only receives GET requests.\n");
        return "";
    }
    token = strtok(NULL, " ");
    regex integer_expr("[\"#@,;:<>*^|]");
    if (regex_search(token, integer_expr))
    {
        printf("Bad Request, there are some invalid characteres in the file name.\n");
        return "";
    }
    cout << dir + token << "\n";
    return dir + token;
}

// int main()
// {
//     char str[] = "GET index.html HTTP/1.0\r\nHost:127.0.0.1\r\n\r\n";
//     string dir = "/home/duduita";
//     string test = getRequestSource(str, dir);
//     return 0;
// }