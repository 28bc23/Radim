cmake_minimum_required(VERSION 4.2.3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Radim VERSION 1.0
              DESCRIPTION "Radim is an web crawler"
              LANGUAGES CXX)

find_package(CURL REQUIRED)

add_executable(Radim src/RadimTheWebCrawler.cpp src/RadimTheWebCrawler.h)

target_link_libraries(Radim PRIVATE CURL::libcurl)
