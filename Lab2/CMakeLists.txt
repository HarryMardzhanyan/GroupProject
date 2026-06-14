cmake_minimum_required(VERSION 3.10)
project(GraphDro4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Настройки компилятора ---
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wunused)
endif()

# --- 1. Основная библиотека (Логика графов) ---
# Убираем отсюда console.cpp, оставляем только "движок"
add_library(graph_lib STATIC
        src/graph.cpp
        src/graph_backend.cpp
        src/metrics.cpp
        src/graph_generators.cpp
        src/parsers.cpp
)

# Указываем пути к заголовкам для библиотеки и всех, кто её использует
target_include_directories(graph_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external
)

# --- 2. Консольное приложение (Интерфейс) ---
# Создаем отдельный исполняемый файл из console.cpp
add_executable(GraphApp src/console.cpp)

# Линкуем приложение с нашей библиотекой
target_link_libraries(GraphApp PRIVATE graph_lib)

# --- 3. Тесты ---
enable_testing()

# Создаем исполняемый файл для тестов (если файл существует)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_graph.cpp")
    add_executable(test_run tests/test_graph.cpp)
    target_link_libraries(test_run PRIVATE graph_lib)

    add_test(NAME Catch2Tests COMMAND test_run)
endif()
