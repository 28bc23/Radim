#include "WebCrawlerManager.h"
#include <thread>
#include "RadimTheWebCrawler.h"



int main(){

	int numWebCrawlers = 5;
	std::string startingWebs[] = { "https://www.wikipedia.org/", "https://www.youtube.com/", "https://randomgenerate.io/random-link-generator", "https://cppreference.com/", "https://github.com/" };

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
		SpawnNewCrawler("https://randomgenerate.io/random-link-generator");
	}

	void SpawnNewCrawler(std::string startWeb){
		std::lock_guard<std::mutex> lock(vector_mutex);
    		active_crawlers.push_back(
        		std::async(std::launch::async, RadimTheWebCrawler::FindWebsites, startWeb, 1000)
    		);
	}
}
