#include "RadimTheWebCrawler.h"
#include <cstdio>
#include <cstring>

int main(){
  int max = 10;
  char* startWeb = "https://wikipedia.org";
  char** webs = nullptr;
  int size = 0;
  int cap = 0;
  FindNumWebsites(max, startWeb, webs, size, cap);

  for(int i = 0; i < max; i++){
    printf("%s\n", webs[i]);
  }
}

void FindNumWebsites(int max, char* startWeb, char**& outWebs, int& size, int& cap){
  for (int i = 0; i < max; i++){
    AppendString(&outWebs, size, cap, startWeb);
  }
}

void AppendString(char*** charArr, int& size, int& cap, char* str){
  ExtendArray(charArr, size, cap);
  (*charArr)[size] = new char[strlen(str) + 1];
  strcpy((*charArr)[size], str);
  size++;
}

void ExtendArray(char*** charArr, int& size, int& cap){
  if(size < cap){
    return;
  }

  int newCap = cap + 4;

  char** temp = new char*[newCap]();

  for (int i = 0; i < size; i++){
    temp[i] = (*charArr)[i];
  }

  delete[] *charArr;
  *charArr = temp;
  cap = newCap;
}
