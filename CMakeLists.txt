cmake_minimum_required(VERSION 3.10)
#project(dsperf VERSION 0.0.35 LANGUAGES C CXX)

project(dsperf CXX)

# Uses libdaas
option(WITH_DAAS "Enable support for libdaas" OFF)
message(STATUS "WITH_DAAS = ${WITH_DAAS}")

# Standard C/C++
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#set(CMAKE_CXX_)

include_directories(C:/msys64/usr/include/sys)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(INC_DIR ${LIB_DIR}/include)

#set(SYSTEM_INFO_DIR ${SRC_DIR}/system_info_wrapper)
#set(LOOPBACK_DIR ${SRC_DIR})

# Version header
configure_file(
    ${SRC_DIR}/version.h.in
    ${SRC_DIR}/version.h
    @ONLY
)

#target_sources(some_target PRIVATE more_cool_stuff.cpp)

# add_library(dsperf )
add_executable(dsperf src/main.cpp)

target_sources(dsperf PRIVATE ${SRC_DIR}/hardware.cpp)
#add_subdirectory(src) require CmakeList.txt

file(GLOB_RECURSE SOURCES ".cpp" ".h" )

# Main sources
#set(SRC_FILES
#    ${SRC_DIR}/main.cpp
#    ${SRC_DIR}/options.cpp
#    ${SRC_DIR}/hardware.cpp
#    ${SRC_DIR}/models/model_daasfrs.cpp
#    ${SRC_DIR}/models/model_ipv4tcp.cpp
#    ${SRC_DIR}/helpers/sysinfo.cpp
#    ${SRC_DIR}/helpers/timer.cpp
#)   





## Headers
#set(HDR_FILES
#    ${SRC_DIR}/local.h
#    ${SRC_DIR}/options.h
#    ${SRC_DIR}/hardware.h
#    ${SRC_DIR}/version.h
#    ${SRC_DIR}/models/model_daasfrs.h
#    ${SRC_DIR}/models/model_ipv4tcp.h
#    ${SRC_DIR}/helpers/sysinfo.h
#    ${SRC_DIR}/helpers/timer.h
#)

# Cerca tutti i .cpp e .h in src e sottocartelle
#file(GLOB_RECURSE SOURCES "src/.cpp" "src/.h")

# Aggiunge facoltativamente header daas **solo se esistono**
if(WITH_DAAS)
    if(EXISTS "${INC_DIR}/daas.hpp" AND EXISTS "${INC_DIR}/daas_types.hpp")
        list(APPEND HDR_FILES
            ${INC_DIR}/daas.hpp
            ${INC_DIR}/daas_types.hpp
        )
    else()
        message(FATAL_ERROR "Can't find libdaas in ${INC_DIR}")
    endif()
endif()

# System's libraries

#target_link_libraries(
   # dsperf
   # PRIVATE pthread
   # PRIVATE bluetooth
#)

# Links libdaas
if(WITH_DAAS)
    message(STATUS "Compiling WITH libdaas")
    target_compile_definitions(dperf PRIVATE "WITH_DAAS")
    target_include_directories(dperf PRIVATE ${INC_DIR})
    target_link_libraries(dperf PRIVATE ${LIB_DIR}/libdaas.a)
else()
    message(STATUS "Compiling WITHOUT libdaas")
endif()

# Include directory 
#target_include_directories(dsperf PRIVATE
#    ${INC_DIR}
#)



