cmake_minimum_required(VERSION 3.0)
project(VirtualGameBox)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)

# Find the QtWidgets library
find_package(Qt5Widgets)

set(SOURCE_FILES
        atmega2560.h
        AVR.cpp
        AVR.h
        AVRIO.cpp
        AVRIO.h
        AVRRegister.h
        AVRRegisterBank.cpp
        AVRRegisterBank.h
        main.cpp periph/AVRGPIO.cpp periph/AVRGPIO.h periph/Display.cpp periph/Display.h periph/AVRTimer8.cpp periph/AVRTimer8.h periph/AVRTimer16.cpp periph/AVRTimer16.h
        Window.cpp)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(VirtualGameBox ${SOURCE_FILES})

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

target_link_libraries(VirtualGameBox Threads::Threads)
target_link_libraries(VirtualGameBox Qt5::Widgets)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
