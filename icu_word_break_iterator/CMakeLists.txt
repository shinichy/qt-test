project(nested_eventloop)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Network REQUIRED)
find_library(ICUUC_LIBRARY NAMES libicuuc.a PATHS /usr/local/lib)
find_library(ICUDATA_LIBRARY NAMES libicudata.a PATHS /usr/local/lib)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -Wall -pedantic -Wextra -Wnon-virtual-dtor -Woverloaded-virtual -g")
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
  include_directories(SYSTEM
    /usr/local/include
    src
)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Network Qt5::Xml ${ICUUC_LIBRARY} ${ICUDATA_LIBRARY})
