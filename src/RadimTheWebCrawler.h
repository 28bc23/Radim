#ifndef RADIMTHEWEBCRAWLER_H
#define RADIMTHEWEBCRAWLER_H

#include <iostream>

void FindWebsites(std::string& startWeb);
std::string MakeUrlValid(std::string& baseURL, std::string& rawURL);

#endif
