#include "WebCrawlerManager.h"
#include <thread>
#include <random>
#include "RadimTheWebCrawler.h"

const std::string startingWebs[] = { 
    "https://en.wikipedia.org/wiki/Special:Random", 
    "https://www.youtube.com/", 
    "https://wiby.me/surprise/", 
    "https://cppreference.com/", 
    "https://github.com/" 
};

std::string GetRandomWeb() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 4);
    return startingWebs[dist(gen)];
}

int main(){

	int numWebCrawlers = 5;

	for (int i = 0; i < numWebCrawlers; i++){
		WebCrawlerManager::SpawnNewCrawler(startingWebs[i]);
	}

	while(true) {
        	std::this_thread::sleep_for(std::chrono::seconds(1));
    	}
    	return 0;
}

namespace WebCrawlerManager {
	void CrawlerEnd(std::unordered_set<std::string> websites){
		std::cout << "crawler search ended" << std::endl;
		SpawnNewCrawler(GetRandomWeb());
	}

	void SpawnNewCrawler(std::string startWeb){
		std::lock_guard<std::mutex> lock(vector_mutex);
    		active_crawlers.push_back(
        		std::async(std::launch::async, RadimTheWebCrawler::FindWebsites, startWeb, 1000)
    		);
	}
}
