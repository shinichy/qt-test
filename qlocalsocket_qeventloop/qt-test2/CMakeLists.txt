project(qttest2)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Network REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -Wall -pedantic -Wextra -Wnon-virtual-dtor -Woverloaded-virtual -g")
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
INCLUDE_DIRECTORIES(msgpack-c/include .)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Network Qt5::Xml)
