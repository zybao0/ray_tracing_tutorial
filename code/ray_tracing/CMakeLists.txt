cmake_minimum_required (VERSION 2.6)
project(ray_tracing)

file(REMOVE ${PROJECT_SOURCE_DIR}/header/ray_tracing.h)

file(GLOB_RECURSE files RELATIVE ${PROJECT_SOURCE_DIR}/header "*.h")

file(WRITE ${PROJECT_SOURCE_DIR}/header/ray_tracing.h "//this header include all other headers\n")

file(APPEND ${PROJECT_SOURCE_DIR}/header/ray_tracing.h "#pragma once\n")

FOREACH(header ${files})
	#MESSAGE(${header})
	file(APPEND ${PROJECT_SOURCE_DIR}/header/ray_tracing.h "#include<${header}>\n")
ENDFOREACH()

find_package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
	message(STATUS ${EIGEN3_INCLUDE_DIR})
	include_directories(${EIGEN3_INCLUDE_DIR})
	add_definitions(-DENABLE_EIGEN3)
endif(EIGEN3_FOUND)

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
	message(STATUS ${ZLIB_INCLUDE_DIR})
	#message(STATUS ${ZLIB_LIBRARY})
	include_directories(${ZLIB_INCLUDE_DIR})
endif(ZLIB_FOUND)

find_package(PNG REQUIRED)
if(PNG_FOUND)
	message(STATUS ${PNG_INCLUDE_DIR})
	#message(STATUS ${PNG_LIBRARY})
	include_directories(${PNG_INCLUDE_DIR})
endif(PNG_FOUND)

include_directories(${PROJECT_SOURCE_DIR}/header)
SET(CMAKE_CXX_FLAGS "-O2 -std=c++17")
SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

ADD_SUBDIRECTORY(src)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
ADD_EXECUTABLE(ray_tracing ${SRC_LIST})
TARGET_LINK_LIBRARIES(ray_tracing tools core objects ${ZLIB_LIBRARY} ${PNG_LIBRARY})
