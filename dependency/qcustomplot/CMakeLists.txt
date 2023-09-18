# ==== CMAKE CONFIGURATION =================================================== #

cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR on)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS off)

set(CMAKE_AUTOUIC on)
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTORCC on)

# ==== LIBRARY CONFIGURATION ================================================= #

set(PROJECT_NAME qcustomplot)
add_library(${PROJECT_NAME} SHARED qcustomplot.h qcustomplot.cpp)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    PrintSupport)

target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::PrintSupport)

target_compile_definitions(${PROJECT_NAME} PRIVATE QCUSTOMPLOT_COMPILE_LIBRARY)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
