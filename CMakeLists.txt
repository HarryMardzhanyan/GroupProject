cmake_minimum_required(VERSION 4.0)
project(untitled3)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(include external)


add_executable(untitled3
        src/graph.cpp
        src/graph.cpp
        external/catch.hpp
        include/graph.hpp
        include/dfs_visitor.h

)
