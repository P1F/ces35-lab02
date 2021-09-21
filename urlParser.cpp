/*
    Alunos: Eduardo Menezes e Rodrigo Teixeira
    CES35 LAB 2
    Client URL parser
*/
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

string getFilename(string parsedURL)
{
    size_t lastOccurrence;
    lastOccurrence = parsedURL.find_last_of('/');

    if (lastOccurrence == -1)
        return parsedURL;

    return parsedURL.substr(lastOccurrence + 1);
}

vector<string> urlParser(char str[])
{
    vector<string> parsedURL;
    char *token;
    token = strtok(str, ":/");
    int cnt = 0;
    while (cnt < 3)
    {
        parsedURL.push_back(token);
        if (cnt != 2)
            token = strtok(NULL, ":/");
        cnt++;
    }
    token = strtok(NULL, "");
    if (parsedURL.size() < 2)
    {
        printf("Error! There is an incomplete request.\n");
        return {};
    }
    for (auto i : parsedURL[2])
        if (!isdigit(i))
        {
            printf("Error! There isn't a valid port number.\n");
            return {};
        }
    parsedURL.push_back(string(token));
    if (parsedURL.size() != 4)
    {
        printf("Error! There is an invalid request.\n");
        return {};
    }
    return parsedURL;
}