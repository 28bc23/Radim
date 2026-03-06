cmake_minimum_required(VERSION 4.1.3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(RadimWebCrawler VERSION 1.0
              DESCRIPTION "Radim is an web crawler"
              LANGUAGES CXX)

find_package(CURL REQUIRED)
find_package(libpqxx REQUIRED)

add_executable(RadimWebCrawler src/RadimTheWebCrawler.cpp src/RadimTheWebCrawler.h)

target_link_libraries(RadimWebCrawler PRIVATE CURL::libcurl)
target_link_libraries(RadimWebCrawler PRIVATE libpqxx::pqxx)
