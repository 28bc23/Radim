cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(RadimWebCrawler VERSION 1.0
              DESCRIPTION "Radim is an web crawler"
              LANGUAGES CXX)

find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED IMPORTED_TARGET libpqxx)

add_executable(RadimWebCrawler src/WebCrawlerManager.cpp src/RadimTheWebCrawler.cpp)

target_include_directories(RadimWebCrawler PRIVATE src)

target_link_libraries(RadimWebCrawler PRIVATE CURL::libcurl PkgConfig::PQXX)
