cmake_minimum_required(VERSION 3.10.2)
project (gaana-dl)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
add_subdirectory(fmt)
add_subdirectory(cpr)
add_subdirectory(md5)
add_subdirectory(id3v2lib)
add_subdirectory(tinyxml2)
include_directories(${CPR_INCLUDE_DIRS} fmt/include md5/ rapidjson/include clipp/include tinyxml2/)
add_executable(gaana-dl main.cpp)
target_link_libraries(gaana-dl PRIVATE fmt::fmt-header-only md5 id3v2 tinyxml2 ${CPR_LIBRARIES})

