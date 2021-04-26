cmake_minimum_required(VERSION 3.16)
project(HW1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lcurl")

set(HW1 MarketDataJson.cpp json jsoncpp.cpp)

add_executable(HW1 MarketDataJson.cpp json jsoncpp.cpp)

target_link_libraries(HW1 curl)