cmake_minimum_required (VERSION 2.8)
project (HL-Hitters)

include_directories("lib")
find_package(Boost 1.46 REQUIRED)
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory("src")