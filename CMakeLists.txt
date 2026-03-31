cmake_minimum_required(VERSION 3.10)
project(GraphDro4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Общие флаги компилятора
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(GLOBAL_WARNINGS "-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wunused")
endif()

# Создаём библиотеку и даем ей имя graph_lib
add_library(graph_lib STATIC
        src/graph.cpp
        src/graph_backend.cpp
        src/metrics.cpp
        src/graph_generators.cpp
        src/parsers.cpp
        include/graph_generators.hpp
        include/metrics.hpp
        src/console.cpp
        include/serial.hpp
)

# Настраиваем пути к заголовочным файлам для ЭТОЙ библиотеки
target_include_directories(graph_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external
)

# Тесты
add_executable(test_graph tests/test_graph.cpp)
target_link_libraries(test_graph graph_lib)

# Тестирование
enable_testing()
add_test(NAME Catch2Tests COMMAND test_graph)