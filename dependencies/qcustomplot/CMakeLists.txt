cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME qcustomplot)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    PrintSupport)

add_library(${PROJECT_NAME} SHARED qcustomplot.h qcustomplot.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::PrintSupport)

target_compile_definitions(${PROJECT_NAME} PRIVATE QCUSTOMPLOT_COMPILE_LIBRARY)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
