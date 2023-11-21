# ==== CMAKE CONFIGURATION =================================================== #

cmake_minimum_required(VERSION 3.21.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# qt generators
set(CMAKE_AUTOUIC on)
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTORCC on)

# ==== EXECTABLE CONFIGURATION =============================================== #

find_package(Xlnt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS
    Concurrent
    Core
    Gui
    Widgets
    Charts
)

set(VRFBCALC-CPP_VERSION "0.3.4")
configure_file(include/vrfbcalccfg.hpp.in include/vrfbcalccfg.hpp)

add_executable(vrfbcalc-cpp WIN32
    ${CMAKE_CURRENT_BINARY_DIR}/include/vrfbcalccfg.hpp
    src/main.cpp

    include/driver/vrfbdriver.hpp
    src/driver/vrfbdriver.cpp
    include/driver/vrfbdriver_io.hpp
    src/driver/vrfbdriver_io.cpp

    include/util/qtutilities.hpp

    resource/resource.qrc

    include/view/mainwindow.h
    include/view/mainwindow_impl.h
    src/view/mainwindow.ui
    src/view/mainwindow.cpp
    src/view/mainwindow_impl.cpp

    include/view/plotpanel.h
    src/view/plotpanel.ui
    src/view/plotpanel.cpp

    # :::: Cell Efficiency ::::::::::::::::::::::::::::::::::::::::::::::::::: #

    include/view/celleff/celleffconfigpopup.h
    src/view/celleff/celleffconfigpopup.ui
    src/view/celleff/celleffconfigpopup.cpp

    include/view/celleff/celleffdatasetform.h
    src/view/celleff/celleffdatasetform.ui
    src/view/celleff/celleffdatasetform.cpp

    include/view/celleff/celleffdataentryform.h
    src/view/celleff/celleffdataentryform.ui
    src/view/celleff/celleffdataentryform.cpp

    include/view/celleff/celleffloadpresetconfigform.h
    src/view/celleff/celleffloadpresetconfigform.ui
    src/view/celleff/celleffloadpresetconfigform.cpp

    include/view/celleff/celleffmanualconfigform.h
    src/view/celleff/celleffmanualconfigform.ui
    src/view/celleff/celleffmanualconfigform.cpp

    include/view/celleff/celleffresultview.h
    src/view/celleff/celleffresultview.ui
    src/view/celleff/celleffresultview.cpp

    # :::: Shunt Current ::::::::::::::::::::::::::::::::::::::::::::::::::::: #

    include/view/shuntcur/shuntcursimconfigpopup.h
    src/view/shuntcur/shuntcursimconfigpopup.ui
    src/view/shuntcur/shuntcursimconfigpopup.cpp

    include/view/shuntcur/shuntcursimpccconfigpanel.h
    src/view/shuntcur/shuntcursimpccconfigpanel.ui
    src/view/shuntcur/shuntcursimpccconfigpanel.cpp

    include/view/shuntcur/shuntcursimesiposconfigpanel.h
    src/view/shuntcur/shuntcursimesiposconfigpanel.ui
    src/view/shuntcur/shuntcursimesiposconfigpanel.cpp

    include/view/shuntcur/shuntcursimreportpopup.h
    src/view/shuntcur/shuntcursimreportpopup.ui
    src/view/shuntcur/shuntcursimreportpopup.cpp

    resource/app.o          # executable icon
)

target_include_directories(vrfbcalc-cpp PRIVATE
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

# cache variable used by package.bat
set(VRFBCALC_EXECUTABLE_NAME "vrfbcalc_v${VRFBCALC-CPP_VERSION}"
    CACHE STRING "Output executable name" FORCE)
set_target_properties(vrfbcalc-cpp PROPERTIES OUTPUT_NAME
    ${VRFBCALC_EXECUTABLE_NAME}
)

# link libraries
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
target_link_libraries(vrfbcalc-cpp PRIVATE vrfblib)

# compile definition for qcustomplot
target_compile_definitions(vrfbcalc-cpp PRIVATE QCUSTOMPLOT_USE_LIBRARY)

# post build to install required qt dlls
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
        -printsupport                               # for qcustomplot
        \"$<TARGET_FILE:vrfbcalc-cpp>\"
)
