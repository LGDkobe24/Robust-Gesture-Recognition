cmake_minimum_required(VERSION 3.3)
project(HandGesture)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_package(OPENCV REQUIRED)

set(SOURCE_FILES src/detector/detector.h main.c src/ui/UI.h src/util/config.h src/util/util.h)
include_directories(${OpenCV_INCLUDE_DIRS} )

add_executable(HandGesture ${SOURCE_FILES})
target_link_libraries(HandGesture ${OpenCV_LIBS})