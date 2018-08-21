cmake_minimum_required(VERSION 3.1.0)

project(gamebox)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)

# Find the QtWidgets library
find_package(Qt5Widgets)

add_executable(gamebox main.cpp window.cpp renderarea.cpp)
target_sources(gamebox PUBLIC ../BackspaceInvaders.cpp ../BreakOut.cpp ../Flappy.cpp
<<<<<<< HEAD
               ../Raycaster.cpp ../Saper.cpp ../Snake.cpp ../Snail.cpp ../Snake2.cpp ../Tester.cpp)
target_sources(gamebox PUBLIC ../application.cpp ../avrfix.cpp ../Ghostbuster.cpp ../menu.cpp ../random.cpp)
=======
               ../Raycaster.cpp ../Saper.cpp ../Snake.cpp ../SpaceShips.cpp ../Snake2.cpp
		../Tester.cpp ../Snail.cpp ../Mario.cpp)
target_sources(gamebox PUBLIC ../application.cpp ../avrfix.cpp ../menu.cpp
               ../random.cpp)
>>>>>>> 96fe9745c6be9613cdcb8bf4a65dbce635196a75
target_compile_definitions(gamebox PUBLIC EMULATED)

# Use the Widgets module from Qt 5.
target_link_libraries(gamebox Qt5::Widgets)
