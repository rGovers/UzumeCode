cmake_minimum_required (VERSION 3.0)
project (UzumeCode C)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include/")

file(GLOB SOURCES "src/*.c")

add_executable(UzumeCode ${SOURCES})