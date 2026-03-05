#include "RadimTheWebCrawler.h"
#include <curl/curl.h>
#include <regex>
#include <thread>
#include <queue>
#include <unordered_set>
#include <cstdlib>
#include <format>

int main(){

  const char* dbHostEnv = std::getenv("DB_HOST");
  const char* dbUserEnv = std::getenv("DB_USER");
  const char* dbNameEnv = std::getenv("DB_NAME");
  const char* dbPasswdEnv = std::getenv("DB_PASS");
  const char* dbStartWebEnv = std::getenv("START_WEB");

	std::string dbHost; 
	std::string dbUser; 
	std::string dbName;
	std::string dbPasswd;
	std::string startWeb;

  if(dbHostEnv != nullptr){
    dbHost = dbHostEnv;
    std::cout << "database host loaded from env variable" << std::endl;
  }else{
    std::cerr << "DB_HOST env variable is not set" << std::endl;
    return 1;
  }
  if(dbUserEnv != nullptr){
    dbUser = dbUserEnv;
    std::cout << "database user loaded from env variable" << std::endl;
  }else{
    std::cerr << "DB_USER env variable is not set" << std::endl;
    return 1;
  }
  if(dbNameEnv != nullptr){
    dbName = dbNameEnv;
    std::cout << "database name loaded from env variable" << std::endl;
  }else{
    std::cerr << "DB_NAME env variable is not set" << std::endl;
    return 1;
  }
  if(dbPasswdEnv != nullptr){
    dbPasswd = dbPasswdEnv;
    std::cout << "database passwd loaded from env variable" << std::endl;
  }else{
    std::cerr << "DB_PASS env variable is not set" << std::endl;
    return 1;
  }
  if(dbStartWebEnv != nullptr){
    startWeb = dbStartWebEnv;
    std::cout << "start web loaded from env variable" << std::endl;
  }else{
    std::cerr << "START_WEB env variable is not set" << std::endl;
    return 1;
  }

  try {
    std::string conn_command = std::format("user={} password={} host={} port={} dbname={} target_session_attrs=read-write", dbUser, dbPasswd, dbHost, 5432, dbName); 
    pqxx::connection c(conn_command);
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
        std::cout << "Using: " << rows2[0][0].as<std::string>() << " as start web" << std::endl;
      }else{
        std::cerr << "No new websites to explore >> exiting";
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

	toVisit.push(startWeb);

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
				std::regex rgxTitle(R"-(<title>([^>]*)</title>)-");
				std::smatch match;
				std::smatch matchTitle;

        if(std::regex_search(readBuffer, matchTitle, rgxTitle)){
          //std::cout << "Visiting: " << currentWeb << " Title: " << matchTitle[1].str() << std::endl;

          pqxx::work w(c);
          w.exec("INSERT INTO websites (url, title, is_visited) VALUES ($1, $2, TRUE) ON CONFLICT (url) DO UPDATE SET title = $2, is_visited = TRUE;", pqxx::params{currentWeb, matchTitle[1].str()});
          w.commit();


				  while (std::regex_search(readBuffer, match, rgx)){
					  std::string rawURL = match[1].str();
					  std::string validURL = "";
					  if (rawURL.starts_with("https://") || rawURL.starts_with("http://") || rawURL.starts_with("ftp://")){
						  validURL = rawURL;
					  }else{
						  validURL = MakeUrlValid(currentWeb, rawURL);
					  }
	
					  if(validURL != ""){

              pqxx::work w2(c);
              pqxx::result rows = w2.exec("SELECT (seen_count) FROM websites WHERE url = $1", pqxx::params{validURL});
              w2.commit();


						  if(rows.size() == 0){
							  //std::cout << "Found new web: " << validURL << std::endl;
                pqxx::work w3(c);
                w3.exec("INSERT INTO websites (url) VALUES ($1) ON CONFLICT DO NOTHING;", pqxx::params{validURL});
                w3.commit();

							  toVisit.push(validURL);
						  }else{
                int seen_count = rows[0][0].as<int>();
                seen_count++;
                pqxx::work w3(c);
                w3.exec("UPDATE websites SET seen_count = $1 WHERE url = $2;", pqxx::params{seen_count, validURL});
                w3.commit();
              }

					  }
					  readBuffer = match.suffix().str();
				  }
        }else{
          std::cout << "Title not found - skipping website" << std::endl;
        }
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
  std::cerr << "no site to visit. Found dead end" << std::endl;
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
