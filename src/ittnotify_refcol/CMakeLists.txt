#
#  Copyright (C) 2026 Intel Corporation
#  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
#

# This CMake file builds ITT API reference collector shared library in two ways:
#
# 1. As part of the ittapi repository, pulled in by the root CMakeLists.txt
#    via add_subdirectory() when -DITT_API_REFERENCE_COLLECTOR=ON.
#
# 2. Standalone, with a single one-line build command:
#    cmake -B build && cmake --build build --config Release

cmake_minimum_required(VERSION 3.10)

project(ittnotify_refcol C)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(ITT_REFCOL_STANDALONE ON)
else()
    set(ITT_REFCOL_STANDALONE OFF)
endif()

set(ITT_REFCOL_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/../../include")
set(ITT_REFCOL_ITTNOTIFY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../ittnotify")

add_library(ittnotify_refcol SHARED itt_refcol_impl.c)

set_target_properties(ittnotify_refcol PROPERTIES
    LINKER_LANGUAGE C
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME ittnotify_refcol)

target_include_directories(ittnotify_refcol PRIVATE
    ${ITT_REFCOL_INCLUDE_DIR}
    ${ITT_REFCOL_ITTNOTIFY_DIR})

target_link_libraries(ittnotify_refcol PRIVATE ${CMAKE_DL_LIBS})

if(WIN32)
    target_compile_definitions(ittnotify_refcol PRIVATE _CRT_SECURE_NO_WARNINGS)
    set_target_properties(ittnotify_refcol PROPERTIES
        OUTPUT_NAME libittnotify_refcol
        PREFIX ""
        WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(DEFINED LIBRARY_OUTPUT_PATH AND NOT ITT_REFCOL_STANDALONE)
    set(ITT_REFCOL_OUTPUT_DIR "${LIBRARY_OUTPUT_PATH}")
else()
    set(ITT_REFCOL_OUTPUT_DIR "${CMAKE_BINARY_DIR}")
endif()

set_target_properties(ittnotify_refcol PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${ITT_REFCOL_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${ITT_REFCOL_OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${ITT_REFCOL_OUTPUT_DIR}")

option(ITT_API_REFCOL_SMOKE_TESTS "Build reference collector smoke tests" OFF)
if(ITT_API_REFCOL_SMOKE_TESTS AND NOT ITT_REFCOL_STANDALONE)
    add_executable(refcol_smoke_test tests/smoke_test.c)
    target_include_directories(refcol_smoke_test PRIVATE
        ${ITT_REFCOL_INCLUDE_DIR}
        ${ITT_REFCOL_ITTNOTIFY_DIR})
    target_link_libraries(refcol_smoke_test PRIVATE ittnotify ${CMAKE_DL_LIBS})
    set_target_properties(refcol_smoke_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${ITT_REFCOL_OUTPUT_DIR}")
endif()
