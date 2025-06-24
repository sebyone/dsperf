cmake_minimum_required(VERSION 3.10)

project(dsperf)

# Directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Version Header
configure_file(
    src/version.h.in
    src/version.h
    @ONLY
)

# Uses libdaas
option(WITH_DAAS "Enable support for libdaas" OFF)
message(STATUS "WITH_DAAS = ${WITH_DAAS}")


# Standard C/C++
set(CMAKE_CXX_FLAGS "-I c:/msys64/usr/include") 
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# Add the given directories to those the compiler uses to search for include files.
#target_include_directories(<target> [SYSTEM] [AFTER|BEFORE]
#  <INTERFACE|PUBLIC|PRIVATE> [items1...]
#  [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])
#set(ENV export IMPORTED_CXX_MODULES_INCLUDE_DIRECTORIES=/ucrt64/include)
#In case load library from system. For the static library and shared library: 
#set ( PROJECT_LINK_LIBS libsoftware.a libhardware.dylib)
# link library from installed directory
#link_directories( ${CMAKE_INSTALL_FULL_LIBDIR} )

 
#Define GNU standard installation directories
include( GNUInstallDirs )

add_executable(dsperf src/main.cpp)


#Link from installed packages
#target_link_libraries( phone software hardware )

target_sources(dsperf PRIVATE "src/options.cpp")

# Main sources
set(SRC_FILES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/options.cpp
    ${SRC_DIR}/hardware.cpp
    ${SRC_DIR}/models/model_daasfrs.cpp
    ${SRC_DIR}/models/model_ipv4tcp.cpp
    ${SRC_DIR}/helpers/sysinfo.cpp
    ${SRC_DIR}/helpers/timer.c
)   

## Headers
set(HDR_FILES
    ${SRC_DIR}/locals.h
    ${SRC_DIR}/options.h
    ${SRC_DIR}/hardware.h
    ${SRC_DIR}/version.h
    ${SRC_DIR}/models/model_daasfrs.h
    ${SRC_DIR}/models/model_ipv4tcp.h
    ${SRC_DIR}/helpers/sysinfo.h
    ${SRC_DIR}/helpers/timer.h
)

target_sources(dsperf PRIVATE ${SRC_FILES})
target_precompile_headers(dsperf PRIVATE ${HDR_FILES})

#add_subdirectory(src) require CmakeList.txt

# Include directory 
#target_include_directories(dsperf PRIVATE
#    ${INC_DIR}
#)


# Cerca tutti i .cpp e .h in src e sottocartelle


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




