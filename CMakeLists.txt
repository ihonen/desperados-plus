cmake_minimum_required(VERSION 3.0)

# ------------------------------------------------------------------------------

project(desperados-plus VERSION 0.0.1)

# ------------------------------------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ------------------------------------------------------------------------------

set(EXTERN ${CMAKE_SOURCE_DIR}/extern)

# ------------------------------------------------------------------------------

add_subdirectory(src)
