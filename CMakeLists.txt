cmake_minimum_required(VERSION 3.10)
project(dsperf C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/helpers
    ${PROJECT_SOURCE_DIR}/src/models
)

file(GLOB_RECURSE SOURCES
    src/*.c
)

# Escludi tutti i file in src/models/olds/
foreach(source ${SOURCES})
    if(${source} MATCHES ".*/src/models/olds/.*\\.c$")
        list(REMOVE_ITEM SOURCES ${source})
    endif()
endforeach()

add_executable(dsperf ${SOURCES})
target_link_libraries(dsperf m)
