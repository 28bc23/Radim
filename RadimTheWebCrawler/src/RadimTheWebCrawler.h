#ifndef RADIMTHEWEBCRAWLER_H
#define RADIMTHEWEBCRAWLER_H

#include <iostream>
//#include <pqxx/pqxx>

namespace RadimTheWebCrawler {
	void FindWebsites(std::string startWeb, int num);
	std::string MakeUrlValid(std::string& baseURL, std::string& rawURL);
}
#endif
