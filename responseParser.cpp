#include <iostream>
#include <string>

using namespace std;

vector<string> getResponse(string response)
{
    string header = response.substr(0, response.find("\r\n\r\n"));
    string body = response.substr(response.find("\r\n\r\n"));

    if (response.find("200 OK") != -1)
        return {"200", header, body};
    
    if (response.find("400 Bad Request") != -1)
        return {"400", header, body};
    
    if (response.find("404 Not Found") != -1)
        return {"404", header, body};
    
    return {"-1", "Unidentified Error", ""};
}