#include "RadimTheSearchEngine.h"


struct Website {
    std::string url;
    int seen_count;
};

bool sortbysec(const Website& a, const Website& b) {
    return a.seen_count > b.seen_count; // Sort descending (highest seen_count first)
}



int main(){

  const char* dbHostEnv = std::getenv("DB_HOST");
  const char* dbUserEnv = std::getenv("DB_USER");
  const char* dbNameEnv = std::getenv("DB_NAME");
  const char* dbPasswdEnv = std::getenv("DB_PASS");

	std::string dbHost; 
	std::string dbUser; 
	std::string dbName;
	std::string dbPasswd;


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


  try {
    std::string conn_command = std::format("user={} password={} host={} port={} dbname={} target_session_attrs=read-write", dbUser, dbPasswd, dbHost, 5432, dbName); 
    pqxx::connection c(conn_command);

    while(true){
	    std::string input;

	    std::cout << ": ";
	    std::cin >> input;
	    
	    if (input == ""){
		    continue;
	    }

	    pqxx::work w(c);
	    pqxx::result rows = w.exec("SELECT url, seen_count FROM websites WHERE title=$1", pqxx::params{input});
	    w.commit();

	    if (rows.size() > 0){
		    std::vector<Website> results;

		    for (int i = 0; i < rows.size(); i++){ 
			    results.push_back({
					    	rows[i][0].as<std::string>(),
						rows[i][1].as<int>()
					    });
		    }

		    std::sort(results.begin(), results.end(), sortbysec);

		    for (Website web : results){
			    std::cout << web.url << std::endl;
		    }
	    }
	}

  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}

