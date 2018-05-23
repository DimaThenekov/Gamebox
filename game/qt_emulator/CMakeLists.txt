cmake_minimum_required(VERSION 3.1.0)

project(gamebox)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)

# Find the QtWidgets library
find_package(Qt5Widgets)

add_executable(gamebox main.cpp window.cpp renderarea.cpp)
target_compile_definitions(gamebox PUBLIC EMULATED)

# Use the Widgets module from Qt 5.
target_link_libraries(gamebox Qt5::Widgets)
