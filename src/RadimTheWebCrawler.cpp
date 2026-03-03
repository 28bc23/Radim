#include "RadimTheWebCrawler.h"
#include <cstdio>

int main(){
  int max = 10;
  const char* startWeb = "https://wikipedia.org";
  const char* webs[max];
  FindNumWebsites(max, startWeb, webs);

  for(int i = 0; i < max; i++){
    printf("%s\n", webs[i]);
  }
}

void FindNumWebsites(int max, const char* startWeb, const char** outWebs){
  
}
