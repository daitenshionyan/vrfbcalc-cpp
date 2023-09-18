# ==== CMAKE CONFIGURATION =================================================== #

cmake_minimum_required(VERSION 3.21.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)

set(CMAKE_AUTOUIC off)
set(CMAKE_AUTOMOC off)
set(CMAKE_AUTORCC off)

# ==== LIBRARY CONFIGURATION ================================================= #

add_library(utillib SHARED
    include/public/strutils.hpp
    src/strutils.cpp
)

target_include_directories(utillib PUBLIC include/public)
target_include_directories(utillib PRIVATE include/private)
