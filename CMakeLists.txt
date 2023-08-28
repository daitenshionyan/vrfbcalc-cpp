# ==== CMAKE CONFIGURATION =================================================== #

cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")

# ==== PROJECT CONFIGURATION ================================================= #

project(vrfbcalc-cpp
    LANGUAGES CXX
)

set(VRFBCALC-CPP_VERSION "0.1.0")

configure_file(include/vrfbcalccfg.hpp.in include/vrfbcalccfg.hpp)

# ==== EXECUTABLE ============================================================ #

add_executable(vrfbcalc-cpp
    src/main.cpp
    src/vrfbdriver.cpp
    src/strutils.cpp
    src/table.cpp
    src/celleff.cpp
)

target_include_directories(vrfbcalc-cpp PRIVATE
    ${PROJECT_BINARY_DIR}/include
)
target_include_directories(vrfbcalc-cpp PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(vrfbcalc-cpp PRIVATE nlohmann_json::nlohmann_json)
