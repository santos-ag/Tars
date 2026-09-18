cmake_minimum_required(VERSION 3.20)

project(tars-ml LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(tars-ml
    src/main.cpp
    src/Train.cpp
)
