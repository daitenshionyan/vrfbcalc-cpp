# ==== CMAKE CONFIGURATION =================================================== #

cmake_minimum_required(VERSION 3.21.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)

set(CMAKE_AUTOUIC off)
set(CMAKE_AUTOMOC off)
set(CMAKE_AUTORCC off)

# ==== LIBRARY CONFIGURATION ================================================= #

find_package(Eigen3 CONFIG REQUIRED)

add_library(vrfblib SHARED
    include/public/table.hpp
    include/public/vrfbcalc.hpp

    include/private/celleff.hpp
    include/private/shuntcur.hpp

    src/table.cpp
    src/celleff.cpp
    src/shuntcur.cpp
)

target_link_libraries(vrfblib PRIVATE Eigen3::Eigen)
target_link_libraries(vrfblib PRIVATE utillib)

target_include_directories(vrfblib PUBLIC include/public)
target_include_directories(vrfblib PRIVATE include/private)
