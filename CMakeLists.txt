cmake_minimum_required(VERSION 3.10)
project(GraphDro4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Общие флаги компилятора
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(GLOBAL_WARNINGS "-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wunused")
endif()

# Создаём библиотеку из общих исходников
add_library(
    src/graph.cpp
    src/graph_backend.cpp
)

# Добавляем заголовочные файлы для библиотеки
target_include_directories(calc_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)
