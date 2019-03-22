cmake_minimum_required(VERSION 3.10.2)
project (HappyHTTP)
set(CMAKE_BUILD_TYPE Debug)
add_library(happyhttp STATIC happyhttp.cpp)
