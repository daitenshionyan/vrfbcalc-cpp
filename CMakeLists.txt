# ==== CMAKE CONFIGURATION =================================================== #

cmake_minimum_required(VERSION 3.21.0)

set(CMAKE_AUTOUIC on)
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTORCC on)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS on)

set(PROJECT_ARCH_TARGET "amd64")

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

include_directories(src include ${CMAKE_CURRENT_BINARY_DIR}/include)

add_subdirectory(library/utillib)

# ==== LIBRARY =============================================================== #

set(VRFBCALC_PUBLIC_HEADERS
    include/table.hpp
    include/vrfbcalc.hpp)
set(VRFBCALC_PRIVATE_HEADERS
    include/celleff.hpp)
set(VRFBCALC_SOURCES
    src/table.cpp
    src/celleff.cpp)

add_library(vrfbcalc SHARED
    ${VRFBCALC_PUBLIC_HEADERS}
    ${VRFBCALC_PRIVATE_HEADERS}
    ${VRFBCALC_SOURCES})

target_link_libraries(vrfbcalc PRIVATE utillib)

# ==== EXECUTABLE ============================================================ #

set(VRFBCALC-CPP_VERSION "0.2.0")

configure_file(include/vrfbcalccfg.hpp.in include/vrfbcalccfg.hpp)

add_subdirectory(dependencies/qcustomplot)

add_executable(vrfbcalc-cpp WIN32
    ${CMAKE_CURRENT_BINARY_DIR}/include/vrfbcalccfg.hpp
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

    include/view/celleffresultview.h
    src/view/celleffresultview.ui
    src/view/celleffresultview.cpp

    include/view/graphplotform.h
    src/view/graphplotform.ui
    src/view/graphplotform.cpp

    resource/app.o
)

set(VRFBCALC_EXECUTABLE_NAME "vrfbcalc_v${VRFBCALC-CPP_VERSION}" CACHE STRING "Output executable name" FORCE)
set_target_properties(vrfbcalc-cpp PROPERTIES OUTPUT_NAME ${VRFBCALC_EXECUTABLE_NAME})

find_package(Xlnt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS
    Concurrent
    Core
    Gui
    Widgets
    Charts)

target_link_libraries(vrfbcalc-cpp PRIVATE vrfbcalc)
target_link_libraries(vrfbcalc-cpp PRIVATE xlnt::xlnt)
target_link_libraries(vrfbcalc-cpp PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(vrfbcalc-cpp PRIVATE
    Qt6::Concurrent
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts)
target_link_libraries(vrfbcalc-cpp PRIVATE qcustomplot)
target_link_libraries(vrfbcalc-cpp PRIVATE utillib)

target_compile_definitions(vrfbcalc-cpp PRIVATE QCUSTOMPLOT_USE_LIBRARY)

set(PACKAGE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/package)
file(MAKE_DIRECTORY ${PACKAGE_OUTPUT_DIR})

add_custom_command(TARGET vrfbcalc-cpp POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE}
        --verbose 1
        $<IF:$<CONFIG:Debug>,--debug,--release>
        --no-compiler-runtime
        --no-network
        --no-pdf
        --skip-plugin-types
            imageformats
        --no-opengl-sw
        --no-system-d3d-compiler
        --no-translations
        -printsupport                           # for qcustomplot
        \"$<TARGET_FILE:vrfbcalc-cpp>\"
)
