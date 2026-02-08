cmake_minimum_required(VERSION 3.16)
project(test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(C:/Users/josep/Documents/Cpp/_PACKAGES/myLibs)

# Find SFML
find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics Audio)

add_executable(test test.cpp)

target_link_libraries(test PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics
    SFML::Audio
)

