#
# Copyright (C) 2005-2023 Intel Corporation
#
# SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
#

cmake_minimum_required(VERSION 3.5)

if(POLICY CMP0048)
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

option(FORCE_32 "Force a 32-bit compile on 64-bit" OFF)
option(ITT_API_IPT_SUPPORT "ptmarks support" OFF)
option(ITT_API_FORTRAN_SUPPORT "fortran support" OFF)

if(FORCE_32 AND UNIX)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif()

if(CMAKE_SIZEOF_VOID_P MATCHES "8" AND NOT(FORCE_32))
    set(ARCH_64 ON)
endif()

if(FORCE_32 AND ITT_API_FORTRAN_SUPPORT)
    # ifx dropped 32-bit support
    message(WARNING "Fortran support for 32-bit has been discontinued")
    set(ITT_API_FORTRAN_SUPPORT OFF)
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

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${LIBRARY_OUTPUT_PATH})
endforeach()

set(ITT_PUBLIC_HDRS
    include/ittnotify.h
    include/jitprofiling.h
    include/libittnotify.h
)

file(GLOB ITT_SRCS "src/ittnotify/*.c" "src/ittnotify/*.h")

if(ITT_API_IPT_SUPPORT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DITT_API_IPT_SUPPORT")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DITT_API_IPT_SUPPORT")
    if(NOT WIN32)
        enable_language(ASM)
        if(ARCH_64)
            set(ITT_PT src/ittnotify/ittptmark64.S)
        else()
            set(ASM_OPTIONS "-m32")
            set(ITT_PT src/ittnotify/ittptmark32.S)
        endif()
        set(CMAKE_ASM_FLAGS "${CFLAGS} ${ASM_OPTIONS}")
    else()
        enable_language(ASM_MASM)
        if(ARCH_64)
            set(ITT_PT src/ittnotify/ittptmark64.asm)
	    else()
            set(ITT_PT src/ittnotify/ittptmark32.asm)
        endif()
    endif()
endif()

if(NOT WIN32)
    set(PLATFORM_PATH "posix")
    set(PLATFORM_EXT "o")
else()
    set(PLATFORM_PATH "win32")
    set(PLATFORM_EXT "obj")
endif()

if(ITT_API_FORTRAN_SUPPORT)
    enable_language(Fortran)

    set(FORTRAN_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/fortran)
    file(MAKE_DIRECTORY ${FORTRAN_BINARY_DIR})

    set(ITT_FORTRAN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/include/fortran/${PLATFORM_PATH}/ittnotify.f90)
    set(ADVISOR_ANNOTATION_SRC ${CMAKE_CURRENT_SOURCE_DIR}/include/fortran/advisor_annotate.f90)
    set(ITT_FORTRAN ${FORTRAN_BINARY_DIR}/ittfortran.${PLATFORM_EXT})
    set(ADVISOR_ANNOTATION ${FORTRAN_BINARY_DIR}/advisor_annotate.${PLATFORM_EXT})

    if(WIN32)
        set(FORTRAN_BUILD_CMD ${CMAKE_Fortran_COMPILER} /Z7 /nologo /libdir:noauto /c /O2 /module:${FORTRAN_BINARY_DIR})
        set(ITT_FORTRAN_BUILD_CMD ${FORTRAN_BUILD_CMD} /object:${ITT_FORTRAN} ${ITT_FORTRAN_SRC})
        set(ADVISOR_ANNOTATION_BUILD_CMD ${FORTRAN_BUILD_CMD} /object:${ADVISOR_ANNOTATION} ${ADVISOR_ANNOTATION_SRC})
    else()
        set(FORTRAN_BUILD_CMD ${CMAKE_Fortran_COMPILER} -g -c -fPIC -O2 -module ${FORTRAN_BINARY_DIR})
        set(ITT_FORTRAN_BUILD_CMD ${FORTRAN_BUILD_CMD} -o ${ITT_FORTRAN} ${ITT_FORTRAN_SRC})
        set(ADVISOR_ANNOTATION_BUILD_CMD ${FORTRAN_BUILD_CMD} -o ${ADVISOR_ANNOTATION} ${ADVISOR_ANNOTATION_SRC})
    endif()

    add_custom_command(OUTPUT  ${ITT_FORTRAN}
                       COMMAND ${ITT_FORTRAN_BUILD_CMD}
                       DEPENDS ${ITT_FORTRAN_SRC}
                       COMMENT "Building ITT Fortran")

    add_custom_command(OUTPUT  ${ADVISOR_ANNOTATION}
                       COMMAND ${ADVISOR_ANNOTATION_BUILD_CMD}
                       DEPENDS ${ADVISOR_ANNOTATION_SRC}
                       COMMENT "Building Advisor Annotation")

    add_library(ittnotify STATIC ${ITT_SRCS} ${ITT_PUBLIC_HDRS} ${ITT_PT} ${ITT_FORTRAN})
    add_library(advisor STATIC ${ADVISOR_ANNOTATION})
else()
    add_library(ittnotify STATIC ${ITT_SRCS} ${ITT_PUBLIC_HDRS} ${ITT_PT})
endif()

set(JITPROFILING_SRC "src/ittnotify/jitprofiling.c")
add_library(jitprofiling STATIC ${JITPROFILING_SRC})

if(WIN32)
    set_target_properties(ittnotify PROPERTIES OUTPUT_NAME libittnotify)
    set_target_properties(jitprofiling PROPERTIES OUTPUT_NAME libjitprofiling)
    if(ITT_API_FORTRAN_SUPPORT)
        set_target_properties(advisor PROPERTIES OUTPUT_NAME libadvisor)
    endif()
else()
    set_target_properties(ittnotify PROPERTIES OUTPUT_NAME ittnotify)
    set_target_properties(jitprofiling PROPERTIES OUTPUT_NAME jitprofiling)
    if(ITT_API_FORTRAN_SUPPORT)
        set_target_properties(advisor PROPERTIES OUTPUT_NAME advisor)
    endif()
endif()

target_link_libraries(ittnotify PRIVATE ${CMAKE_DL_LIBS})

set_target_properties(ittnotify PROPERTIES LINKER_LANGUAGE C)
if(ITT_API_FORTRAN_SUPPORT)
    set_target_properties(advisor PROPERTIES LINKER_LANGUAGE C)
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
