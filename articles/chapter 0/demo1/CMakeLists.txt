cmake_minimum_required (VERSION 2.6)
project (demo)

find_package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
	message(STATUS ${EIGEN3_INCLUDE_DIR})
	include_directories(${EIGEN3_INCLUDE_DIR})
	add_definitions(-DENABLE_EIGEN3)
endif(EIGEN3_FOUND)

SET(CMAKE_CXX_FLAGS "-O3 -std=c++17")

add_executable(demo main.cpp) 
target_link_libraries(demo)