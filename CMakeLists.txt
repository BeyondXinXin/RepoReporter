﻿cmake_minimum_required(VERSION 3.1.0)
project(RepoReporter VERSION "0.0")

include(cmake/clean-all.cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(BUILD_SHARED_LIBS "" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent)
find_package(Qt5LinguistTools)

add_subdirectory(3rd)
add_subdirectory(src)

add_custom_target(clean-all
   COMMAND ${CMAKE_BUILD_TOOL} clean
   COMMAND ${CMAKE_COMMAND} -P clean-all.cmake
)

