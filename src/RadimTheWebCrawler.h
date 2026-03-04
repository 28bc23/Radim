#ifndef RADIMTHEWEBCRAWLER_H
#define RADIMTHEWEBCRAWLER_H

#include <iostream>
#include <pqxx/pqxx>

void FindWebsites(std::string& startWeb, pqxx::connection& c);
std::string MakeUrlValid(std::string& baseURL, std::string& rawURL);

#endif
