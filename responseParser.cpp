#include <iostream>
#include <string>

using namespace std;

int getResponseStatus(string response)
{
    string header = response.substr(0, response.find("\r\n\r\n"));

    if (response.find("200 OK") != -1)
        return 200;
    
    if (response.find("400 Bad Request") != -1)
        return 400;
    
    if (response.find("404 Not Found") != -1)
        return 404;
    
    return -1;
}

string getBody(string response)
{
    string body = response.substr(response.find("\r\n\r\n"));

    return body;
}