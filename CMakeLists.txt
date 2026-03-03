cmake_minimum_required(VERSION 4.2.3)

project(Radim VERSION 1.0
              DESCRIPTION "Radim is an web crawler"
              LANGUAGES CXX)

add_library(one STATIC src/RadimTheWebCrawler.cpp include/RadimTheWebCrawler.h)

target_include_directories(one PUBLIC include)

