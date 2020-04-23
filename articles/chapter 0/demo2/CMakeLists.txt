cmake_minimum_required (VERSION 2.6)
project (demo)

find_package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
	message(STATUS ${EIGEN3_INCLUDE_DIR})
	include_directories(${EIGEN3_INCLUDE_DIR})
	add_definitions(-DENABLE_EIGEN3)
endif(EIGEN3_FOUND)

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
	message(STATUS ${ZLIB_INCLUDE_DIR})
	message(STATUS ${ZLIB_LIBRARY})
	include_directories(${ZLIB_INCLUDE_DIR})
endif(ZLIB_FOUND)

find_package(PNG REQUIRED)
if(PNG_FOUND)
	message(STATUS ${PNG_INCLUDE_DIR})
	message(STATUS ${PNG_LIBRARY})
	include_directories(${PNG_INCLUDE_DIR})
endif(PNG_FOUND)


SET(CMAKE_CXX_FLAGS "-O3 -std=c++17")

add_executable(demo main.cpp) 
target_link_libraries(demo ${ZLIB_LIBRARY} ${PNG_LIBRARY})