#
# Copyright (C) 2005-2019 Intel Corporation
#
# SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
#

cmake_minimum_required(VERSION 2.8.12)

if (POLICY CMP0048)
    # The `project()` command manages `VERSION` variables
    cmake_policy(SET CMP0048 NEW)
endif()

project(ittapi)

OPTION(FORCE_32 "Force a 32bit compile on 64bit" OFF)
OPTION(ITT_API_IPT_SUPPORT "ptmarks support")

IF(FORCE_32 AND UNIX)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
ENDIF()

if(CMAKE_SIZEOF_VOID_P MATCHES "8" AND NOT(FORCE_32))
    set(ARCH_64 1)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
    if (NOT WIN32)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    endif()
else()
    if (NOT WIN32)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
    endif()
    add_definitions(-DNDEBUG)
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIBRARY_OUTPUT_PATH} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set(ITT_PUBLIC_HDRS
    include/ittnotify.h
    include/jitprofiling.h
    include/libittnotify.h
)

file(GLOB ITT_SRCS "src/ittnotify/*.c" "src/ittnotify/*.h")

if (ITT_API_IPT_SUPPORT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DITT_API_IPT_SUPPORT")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DITT_API_IPT_SUPPORT")
    if (NOT WIN32)
        enable_language(ASM)
        if (ARCH_64)
            set(ITT_PT src/ittnotify/ittptmark64.S)
        else()
            set(ASM_OPTIONS "-m32")
            set(ITT_PT src/ittnotify/ittptmark32.S)
        endif()
        set(CMAKE_ASM_FLAGS "${CFLAGS} ${ASM_OPTIONS}" )
    else()
        enable_language(ASM_MASM)
        if (ARCH_64)
            set(ITT_PT src/ittnotify/ittptmark64.asm)
	    else()
            set(ITT_PT src/ittnotify/ittptmark32.asm)
        endif()
    endif()
endif()

if (NOT WIN32)
    set(PLATFORM_PATH "posix")
    set(PLATFORM_EXT "o")
else()
    set(PLATFORM_PATH "win32")
    set(PLATFORM_EXT "obj")
endif()

if (ARCH_64)
    set(ARCH_PATH "x86_64")
else()
    set(ARCH_PATH "x86")
endif()

set(ITT_FORTRAN include/fortran/${PLATFORM_PATH}/${ARCH_PATH}/ittfortran.${PLATFORM_EXT})
set(ADVISOR_ANNOTATION include/fortran/${PLATFORM_PATH}/${ARCH_PATH}/advisor_annotate.${PLATFORM_EXT})
set(JITPROFILING_SRC "src/ittnotify/jitprofiling.c")

add_library(ittnotify STATIC ${ITT_SRCS} ${ITT_PUBLIC_HDRS} ${ITT_PT} ${ITT_FORTRAN})
add_library(advisor STATIC ${ADVISOR_ANNOTATION})
add_library(jitprofiling STATIC ${JITPROFILING_SRC})

if(WIN32)
    SET_TARGET_PROPERTIES(ittnotify PROPERTIES OUTPUT_NAME libittnotify)
    SET_TARGET_PROPERTIES(advisor PROPERTIES OUTPUT_NAME libadvisor)
    SET_TARGET_PROPERTIES(jitprofiling PROPERTIES OUTPUT_NAME libjitprofiling)
else()
    SET_TARGET_PROPERTIES(ittnotify PROPERTIES OUTPUT_NAME ittnotify)
    SET_TARGET_PROPERTIES(advisor PROPERTIES OUTPUT_NAME advisor)
    SET_TARGET_PROPERTIES(jitprofiling PROPERTIES OUTPUT_NAME jitprofiling)
endif()

if (NOT WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    TARGET_LINK_LIBRARIES(ittnotify dl)
endif()

SET_TARGET_PROPERTIES(ittnotify PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(advisor PROPERTIES LINKER_LANGUAGE C)

target_include_directories(ittnotify
    PUBLIC include src/ittnotify
)

target_include_directories(jitprofiling
    PUBLIC include src/ittnotify
)

set(CMAKE_SUPPRESS_REGENERATION true)
