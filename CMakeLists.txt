#
# Copyright (C) 2005-2023 Intel Corporation
#
# SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
#

cmake_minimum_required(VERSION 2.8.12)

if (POLICY CMP0048)
    # The `project()` command manages `VERSION` variables
    cmake_policy(SET CMP0048 NEW)
endif()

get_property(ITT_GENERATOR_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(CMAKE_GENERATOR STREQUAL "Ninja Multi-Config")
    # 'Ninja Multi-Config' specific, see:
    # https://cmake.org/cmake/help/latest/variable/CMAKE_DEFAULT_BUILD_TYPE.html
    set(CMAKE_DEFAULT_BUILD_TYPE "Release" CACHE STRING "CMake default build type")
elseif(NOT ITT_GENERATOR_MULTI_CONFIG)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "CMake build type")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release;Debug;RelWithDebInfo;MinSizeRel")
endif()

project(ittapi)

OPTION(FORCE_32 "Force a 32bit compile on 64bit" OFF)
OPTION(ITT_API_IPT_SUPPORT "ptmarks support" OFF)
OPTION(ITT_API_FORTRAN_SUPPORT "fortran support" OFF)

IF(FORCE_32 AND UNIX)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
ENDIF()

if(CMAKE_SIZEOF_VOID_P MATCHES "8" AND NOT(FORCE_32))
    set(ARCH_64 ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
    # override default -O3
    string(REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
endif()

if(NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIBRARY_OUTPUT_PATH} )
endforeach( )

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

if(ITT_API_FORTRAN_SUPPORT)
    set(ITT_FORTRAN include/fortran/${PLATFORM_PATH}/${ARCH_PATH}/ittfortran.${PLATFORM_EXT})
    set(ADVISOR_ANNOTATION include/fortran/${PLATFORM_PATH}/${ARCH_PATH}/advisor_annotate.${PLATFORM_EXT})

    add_library(ittnotify STATIC ${ITT_SRCS} ${ITT_PUBLIC_HDRS} ${ITT_PT} ${ITT_FORTRAN})
    add_library(advisor STATIC ${ADVISOR_ANNOTATION})
else()
    add_library(ittnotify STATIC ${ITT_SRCS} ${ITT_PUBLIC_HDRS} ${ITT_PT})
endif()

set(JITPROFILING_SRC "src/ittnotify/jitprofiling.c")
add_library(jitprofiling STATIC ${JITPROFILING_SRC})

if(WIN32)
    SET_TARGET_PROPERTIES(ittnotify PROPERTIES OUTPUT_NAME libittnotify)
    SET_TARGET_PROPERTIES(jitprofiling PROPERTIES OUTPUT_NAME libjitprofiling)
    if(ITT_API_FORTRAN_SUPPORT)
        SET_TARGET_PROPERTIES(advisor PROPERTIES OUTPUT_NAME libadvisor)
    endif()
else()
    SET_TARGET_PROPERTIES(ittnotify PROPERTIES OUTPUT_NAME ittnotify)
    SET_TARGET_PROPERTIES(jitprofiling PROPERTIES OUTPUT_NAME jitprofiling)
    if(ITT_API_FORTRAN_SUPPORT)
        SET_TARGET_PROPERTIES(advisor PROPERTIES OUTPUT_NAME advisor)
    endif()
endif()

TARGET_LINK_LIBRARIES(ittnotify PRIVATE ${CMAKE_DL_LIBS})

SET_TARGET_PROPERTIES(ittnotify PROPERTIES LINKER_LANGUAGE C)
if(ITT_API_FORTRAN_SUPPORT)
    SET_TARGET_PROPERTIES(advisor PROPERTIES LINKER_LANGUAGE C)
endif()

target_include_directories(ittnotify
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE src/ittnotify
)

target_include_directories(jitprofiling
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE src/ittnotify
)

# install

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(TARGETS ittnotify EXPORT ittapi-targets INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT ittapi-targets NAMESPACE ittapi:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ittapi)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
                   PATTERN "fortran" EXCLUDE)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ittapiConfig.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/share/ittapiConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ittapi
    NO_SET_AND_CHECK_MACRO)

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/ittnotify.h" ITT_VERSION_PARTS REGEX "#define ITT_[A-Z]+[ ]+" )
string(REGEX REPLACE ".+ITT_MAJOR[ ]+([0-9]+).*" "\\1" ITT_MAJOR "${ITT_VERSION_PARTS}")
string(REGEX REPLACE ".+ITT_MINOR[ ]+([0-9]+).*" "\\1" ITT_MINOR "${ITT_VERSION_PARTS}")
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/share/ittapiConfigVersion.cmake
    VERSION ${ITT_MAJOR}.${ITT_MINOR}.0
    COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/ittapiConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/share/ittapiConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ittapi)
