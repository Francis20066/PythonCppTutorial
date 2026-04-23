cmake_minimum_required(VERSION 3.18)
project(FibonacciPythonExtension LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT Python_EXECUTABLE)
    find_program(PYTHON_EXECUTABLE_FROM_PATH NAMES python python3)
    if(PYTHON_EXECUTABLE_FROM_PATH)
        set(Python_EXECUTABLE "${PYTHON_EXECUTABLE_FROM_PATH}" CACHE FILEPATH "Python executable" FORCE)
    endif()
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

execute_process(
    COMMAND "${Python_EXECUTABLE}" -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX') or '.pyd')"
    OUTPUT_VARIABLE PYTHON_EXTENSION_SUFFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_library(fibonacci_cpp MODULE
    src/fibonacci.cpp
)

target_link_libraries(fibonacci_cpp PRIVATE Python::Module)
target_include_directories(fibonacci_cpp PRIVATE ${Python_INCLUDE_DIRS})

if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(fibonacci_cpp PRIVATE -static -static-libgcc -static-libstdc++)
endif()

set_target_properties(fibonacci_cpp PROPERTIES
    PREFIX ""
    SUFFIX "${PYTHON_EXTENSION_SUFFIX}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

message(STATUS "Python executable: ${Python_EXECUTABLE}")
message(STATUS "Python include dirs: ${Python_INCLUDE_DIRS}")
message(STATUS "Python extension suffix: ${PYTHON_EXTENSION_SUFFIX}")
