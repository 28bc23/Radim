#include "RadimTheWebCrawler.h"
#include <curl/curl.h>
#include <regex>
#include <thread>
#include <queue>
#include <unordered_set>


int main(){
	std::string startWeb = "https://wikipedia.org";

  try {
    pqxx::connection c("user=admin password=tajne_heslo host=db port=5432 dbname=search_engine target_session_attrs=read-write");
    pqxx::work w(c);
    w.exec("CREATE TABLE IF NOT EXISTS websites (url TEXT PRIMARY KEY, title TEXT, seen_count INT DEFAULT 1, is_visited BOOLEAN DEFAULT FALSE);");
    w.commit();

    pqxx::work w2(c);
    pqxx::result rows = w2.exec("SELECT url FROM websites WHERE url=$1;", pqxx::params{startWeb});
    w2.commit();

    if (rows.size() == 0){
      pqxx::work w3(c);
      w3.exec("INSERT INTO websites (url) VALUES ($1) ON CONFLICT DO NOTHING", pqxx::params{startWeb});
      w3.commit();
    }else{
      pqxx::work w4(c);
      pqxx::result rows2 = w4.exec("SELECT url FROM websites WHERE is_visited=FALSE;");
      w4.commit();

      if (rows2.size() > 0){
        startWeb = rows2[0][0].as<std::string>();
      }else{
        std::cout << "No new websites to explore >> exiting";
        return 1;
      }
    }
    FindWebsites(startWeb, c);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp){
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void FindWebsites(std::string& startWeb, pqxx::connection& c){

	std::queue<std::string> toVisit;
    	std::unordered_set<std::string> visited;

	toVisit.push(startWeb);
	visited.insert(startWeb);

	while (!toVisit.empty()){
		std::string currentWeb = toVisit.front();
		toVisit.pop();
		
		CURL* curl;
		CURLcode res;
		std::string readBuffer;

		curl = curl_easy_init();

  		if (curl){
			curl_easy_setopt(curl, CURLOPT_URL, currentWeb.c_str());
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "RadimTheWebCrawler");
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
	
			if(res != CURLE_OK) {
	                        std::cerr << "cURL ERROR: " << curl_easy_strerror(res) << std::endl;
        	        } else {
				std::regex rgx(R"-(<a\s+[^>]*href="([^"]+)")-");
				std::smatch match;
				while (std::regex_search(readBuffer, match, rgx)){
					std::string rawURL = match[1].str();
					std::string validURL = "";
					if (rawURL.starts_with("https://") || rawURL.starts_with("http://") || rawURL.starts_with("ftp://")){
						validURL = rawURL;
					}else{
						validURL = MakeUrlValid(currentWeb, rawURL);
					}
	
					if(validURL != ""){
						if(visited.find(validURL) == visited.end()){
							std::cout << validURL << std::endl;
							visited.insert(validURL);
							toVisit.push(validURL);
						}

					}
					readBuffer = match.suffix().str();
				}
			}

		
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

}

std::string MakeUrlValid(std::string& baseURL, std::string& rawURL){
	if (rawURL.starts_with("#")){
		return "";
	}

	CURLU *h = curl_url();
	if (!h) return "";

	CURLUcode rc = curl_url_set(h, CURLUPART_URL, baseURL.c_str(), 0);
    	if (rc != CURLUE_OK) {
        	curl_url_cleanup(h);
        	return "";
    	}

	rc = curl_url_set(h, CURLUPART_URL, rawURL.c_str(), 0);
    	if (rc != CURLUE_OK) {
        	curl_url_cleanup(h);
        	return "";
    	}

	char *absoluteUrl;
    	rc = curl_url_get(h, CURLUPART_URL, &absoluteUrl, 0);

	std::string result = "";
    	if (rc == CURLUE_OK) {
        	result = absoluteUrl;
        	curl_free(absoluteUrl);        
        	if (!result.starts_with("http")) {
         	   result = "";
        	}
    	}

	curl_url_cleanup(h);
	return result;
}
