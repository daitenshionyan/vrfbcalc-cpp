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
    include/public/vrfblib/vrfblib.hpp

    include/private/celleff.hpp
    include/private/shuntcur/shuntcur.hpp
    include/private/shuntcur/conn_pcc.hpp

    src/celleff.cpp
    src/shuntcur.cpp
)

target_link_libraries(vrfblib PRIVATE Eigen3::Eigen)
target_link_libraries(vrfblib PUBLIC utillib)

target_include_directories(vrfblib PUBLIC include/public)
target_include_directories(vrfblib PRIVATE include/private)

# ==== TESTING =============================================================== #

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(shunttest
    test/shunttestconst_pcc.hpp
    test/shunttest.cpp
)

target_link_libraries(shunttest PRIVATE GTest::gtest_main)
target_link_libraries(shunttest PRIVATE Eigen3::Eigen)
target_link_libraries(shunttest PRIVATE vrfblib)

target_include_directories(shunttest PRIVATE include/private)

include(GoogleTest)
gtest_discover_tests(shunttest)
