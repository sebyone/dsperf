cmake_minimum_required(VERSION 3.10)
project(dsperf LANGUAGES CXX)

# Version Header
configure_file(
    src/version.h.in
    src/version.h
    @ONLY
)

# Options
option(WITH_DAAS "Enable support for libdaas" OFF)
message(STATUS "WITH_DAAS = ${WITH_DAAS}")

# Directories & Sources files
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
#set( TARGET ${PROJECT_NAME} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin )

#set( SOURCE_PATH "${SRC_DIR}/src" )
#set( INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include" )

#file( GLOB_RECURSE HPPS "${INCLUDE_PATH}/*.h" )
file( GLOB_RECURSE HPPS "${SRC_DIR}/*.h" )
file( GLOB_RECURSE CPPS "${SRC_DIR}/*.cpp" )

add_executable(dsperf ${CPPS} ${HPPS} )

# Standard C/C++
set(CMAKE_CXX_FLAGS "-I c:/msys64/usr/include") 
set(CMAKE_C_FLAGS "-I c:/msys64/usr/include") 
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

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
#include( GNUInstallDirs )

#add_executable(dsperf src/main.cpp)

#target_include_directories( ${TARGET} PUBLIC ${INCLUDE_PATH} )
#target_compile_options( ${TARGET} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Wunused-parameter -Wno-long-long -pedantic )
#target_compile_features( ${TARGET} PRIVATE cxx_std_14 )

#install( TARGETS ${TARGET} RUNTIME DESTINATION )

#Link from installed packages
#target_link_libraries( phone software hardware )

#target_sources(dsperf PRIVATE "src/options.cpp")

#target_sources(dsperf PRIVATE ${SRC_FILES})
#target_precompile_headers(dsperf PRIVATE ${HDR_FILES})

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
   #PRIVATE pthread
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




