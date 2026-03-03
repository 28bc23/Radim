cmake_minimum_required(VERSION 4.2.3)

project(Radim VERSION 1.0
              DESCRIPTION "Radim is an web crawler"
              LANGUAGES CXX)

find_package(CURL REQUIRED)

add_executable(Radim src/RadimTheWebCrawler.cpp src/RadimTheWebCrawler.h)
