/*
    Alunos: Eduardo Menezes e Rodrigo Teixeira
    CES35 LAB 1
    Server parse and check client request
*/
#include <iostream>
#include <string>
#include <regex>

using namespace std;

string getRequestSource(char request[], string dir, int sa_id)
{
    string token;
    token = strtok(request, " ");
    if (token != "GET")
    {
        printf("On socket [%d]: Bad Request, this server only receives GET requests.\n", sa_id);
        return "";
    }
    token = strtok(NULL, " ");
    regex integer_expr("[\"#@,;:<>*^|]");
    if (regex_search(token, integer_expr))
    {
        printf("On socket [%d]: Bad Request, there are some invalid characteres in the file name.\n", sa_id);
        return "";
    }
    return dir + token;
}
