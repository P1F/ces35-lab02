#include <bits/stdc++.h>

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
    string pch;
    pch = strtok(str, ":/");
    int cnt = 0;
    while (cnt < 3)
    {
        parsedURL.push_back(pch);
        if (cnt != 2)
            pch = strtok(NULL, ":/");
        cnt++;
    }
    pch = strtok(NULL, "");
    parsedURL.push_back(string(pch));
    return parsedURL;
}

// int main()
// {
//     char str[] = "http://gaia.cs.umass.edu:80/wireshark-labs/HTTP-wireshark-file1.html";
//     urlParser(str);
//     return 0;
// }

// int main()
// {
//     char str[] = "wireshark-labs/HTTP-wireshark-file1.html";
//     string test = getFilename(str);
//     return 0;
// }