cmake_minimum_required(VERSION 3.16)
project(Term_Project_final)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lcurl")
set(Term_Project_final main.cpp json jsoncpp.cpp Stock.h Database.h MarketData.h)

add_executable(Term_Project_final main.cpp main.cpp json jsoncpp.cpp Stock.h Database.h MarketData.h)
target_link_libraries(Term_Project_final  LINK_PUBLIC sqlite3 curl)





