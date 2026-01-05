cmake_minimum_required(VERSION 3.20)
project(TicTacToeV3)

set(CMAKE_CXX_STANDARD 17)

# Find the BGI library
find_package(Threads REQUIRED)
find_library(BGI_LIBRARY NAMES bgi HINTS "C:/Program Files (x86)/Colorado/cs1300/lib")
find_path(BGI_INCLUDE_DIR NAMES winbgim.h HINTS "C:/Program Files (x86)/Colorado/cs1300/include")

if(BGI_LIBRARY AND BGI_INCLUDE_DIR)
    message(STATUS "Found BGI library: ${BGI_LIBRARY}")
    message(STATUS "Found BGI include dir: ${BGI_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "BGI library not found. Please set the path to winbgim library and header files.")
endif()

include_directories(${BGI_INCLUDE_DIR})

add_executable(TicTacToeV3 main.cpp XShape.cpp OShape.cpp Cell.cpp Grid.cpp Player.cpp Game.cpp)

target_link_libraries(TicTacToeV3 ${BGI_LIBRARY} gdi32 user32 comdlg32 winmm Threads::Threads)