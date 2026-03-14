#ifndef WEBCRAWLERMANAGER_H
#define WEBCRAWLERMANAGER_H

#include <iostream>
#include <pqxx/pqxx>
#include <unordered_set>
#include <future>
#include <mutex>

int main();

namespace WebCrawlerManager{
	inline std::vector<std::future<void>> active_crawlers;
    	inline std::mutex vector_mutex;

	void CrawlerEnd(std::unordered_set<std::string> websites);
	void SpawnNewCrawler(std::string startWeb);
}

#endif
