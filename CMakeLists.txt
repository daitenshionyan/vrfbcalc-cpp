# ==== CMAKE CONFIGURATION =================================================== #

cmake_minimum_required(VERSION 3.21.0)

set(CMAKE_AUTOUIC on)
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTORCC on)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)

# ==== PROJECT CONFIGURATION ================================================= #

project(vrfbcalc-cpp
    LANGUAGES CXX
)

set(dir ${CMAKE_BINARY_DIR}/output)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir})
set(CMAKE_BUILD_FILES_DIRECTORY ${dir})
set(CMAKE_BUILD_DIRECTORY ${dir})
set(CMAKE_BINARY_DIR  ${dir})
SET(EXECUTABLE_OUTPUT_PATH ${dir})
SET(LIBRARY_OUTPUT_PATH ${dir}lib)
SET(CMAKE_CACHEFILE_DIR ${dir})

include_directories(src include ${CMAKE_CURRENT_BINARY_DIR}/include)

# ==== LIBRARY =============================================================== #

set(VRFBCALC_PUBLIC_HEADERS
    include/table.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/include/vrfbcalccfg.hpp
    include/vrfbcalc.hpp)
set(VRFBCALC_PRIVATE_HEADERS
    include/celleff.hpp
    include/strutils.hpp)
set(VRFBCALC_SOURCES
    src/strutils.cpp
    src/table.cpp
    src/celleff.cpp)

add_library(vrfbcalc SHARED
    ${VRFBCALC_PUBLIC_HEADERS}
    ${VRFBCALC_PRIVATE_HEADERS}
    ${VRFBCALC_SOURCES})

# ==== EXECUTABLE ============================================================ #

set(VRFBCALC-CPP_VERSION "0.1.0")

configure_file(include/vrfbcalccfg.hpp.in include/vrfbcalccfg.hpp)

find_package(Xlnt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS
    Concurrent
    Core
    Gui
    Widgets)

add_executable(vrfbcalc-cpp WIN32
    src/main.cpp

    include/driver/vrfbdriver.hpp
    src/driver/vrfbdriver.cpp
    include/driver/vrfbdriver_io.hpp
    src/driver/vrfbdriver_io.cpp

    resource/resource.qrc

    include/view/mainwindow.h
    src/view/mainwindow.ui
    src/view/mainwindow.cpp

    include/view/celleffconfigpopup.h
    src/view/celleffconfigpopup.ui
    src/view/celleffconfigpopup.cpp

    include/view/celleffdatasetform.h
    src/view/celleffdatasetform.ui
    src/view/celleffdatasetform.cpp

    include/view/celleffdataentryform.h
    src/view/celleffdataentryform.ui
    src/view/celleffdataentryform.cpp

    include/view/celleffloadpresetconfigform.h
    src/view/celleffloadpresetconfigform.ui
    src/view/celleffloadpresetconfigform.cpp

    include/view/celleffmanualconfigform.h
    src/view/celleffmanualconfigform.ui
    src/view/celleffmanualconfigform.cpp

    resource/app.o
)

set(VRFBCALC_EXECUTABLE_NAME "vrfbcalc_v${VRFBCALC-CPP_VERSION}" CACHE STRING "Output executable name" FORCE)
set_target_properties(vrfbcalc-cpp PROPERTIES OUTPUT_NAME ${VRFBCALC_EXECUTABLE_NAME})

target_link_libraries(vrfbcalc-cpp PRIVATE vrfbcalc)
target_link_libraries(vrfbcalc-cpp PRIVATE xlnt::xlnt)
target_link_libraries(vrfbcalc-cpp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(vrfbcalc-cpp PRIVATE
    Qt6::Concurrent
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets)

set(PACKAGE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/package)
file(MAKE_DIRECTORY ${PACKAGE_OUTPUT_DIR})

add_custom_command(TARGET vrfbcalc-cpp POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE}
        --verbose 1
        --release
        --no-compiler-runtime
        --no-network
        --no-pdf
        --skip-plugin-types
            imageformats
        --no-opengl
        --no-opengl-sw
        --no-system-d3d-compiler
        --no-translations
        \"$<TARGET_FILE:vrfbcalc-cpp>\"
)
