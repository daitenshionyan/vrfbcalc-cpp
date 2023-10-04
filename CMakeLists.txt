# ==== CMAKE CONFIGURATION =================================================== #

cmake_minimum_required(VERSION 3.21.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(PROJECT_ARCH_TARGET "amd64")

# qt generators
set(CMAKE_AUTOUIC on)
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTORCC on)

# configure to outbut required binaries to a single folder
set(dir ${CMAKE_BINARY_DIR}/output)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir})
set(CMAKE_BUILD_FILES_DIRECTORY ${dir})
set(CMAKE_BUILD_DIRECTORY ${dir})
set(CMAKE_BINARY_DIR  ${dir})
set(EXECUTABLE_OUTPUT_PATH ${dir})
set(LIBRARY_OUTPUT_PATH ${dir}lib)
set(CMAKE_CACHEFILE_DIR ${dir})

# ==== PROJECT CONFIGURATION ================================================= #

project(vrfbcalc-cpp
    LANGUAGES CXX
)

# add dependency sub directory
add_subdirectory(dependency/qcustomplot)

# add library sub directory
add_subdirectory(library/utillib)
add_subdirectory(library/vrfblib)

# add executable sub directory
add_subdirectory(app)

# ==== TESTING =============================================================== #

enable_testing()
