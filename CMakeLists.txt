cmake_minimum_required(VERSION 3.0)
project(COW CXX)
set(COW_VERSION_MAJOR 0)
set(COW_VERSION_MINOR 1)
set(COW_VERSION_PATCH 1)

if(${CMAKE_BUILD_TYPE})
  if( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
    message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
  endif( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
  if( ${CMAKE_BUILD_TYPE} STREQUAL "Release" )
    message("CMAKE_CXX_FLAGS_RELEASE is ${CMAKE_CXX_FLAGS_RELEASE}")
  endif( ${CMAKE_BUILD_TYPE} STREQUAL "Release" )
else(${CMAKE_BUILD_TYPE})
  set(CMAKE_BUILD_TYPE Debug cache string "default to debug" force)
  message("Default build type is: Debug")
  message("CMAKE_CXX_FLAGS_DEBUG is ${CMAKE_CXX_FLAGS_DEBUG}")
endif(${CMAKE_BUILD_TYPE})

set(MAIN_INC_DIR ${CMAKE_INSTALL_PREFIX}/include/cow)
set(MAIN_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(MAIN_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)

message("install prefix: ${MAIN_INC_DIR}")

configure_file(
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_SOURCE_DIR}/src/config.h")

add_subdirectory(tests)
add_subdirectory(src)
add_definitions(-std=c++11 -Wall -Werror)
