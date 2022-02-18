/*
  Copyright (C) 2005-2019 Intel Corporation

  SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
*/

/**
 * Zero Cost Annotations (ZCA)
 * 
 * Intel Compiler supports two intrinsics that could be used for code annotations
 * without incurring significant run-time costs when the tools are not in use.
 * Each annotation is more than a mere mark in the instruction stream.
 * It can accept an expression argument like a call to a routine does.
 * There are two forms of the intrinsic, with the following signatures:
 * 
 *      extern "C" void __notify_intrinsic( const char *annotation, const volatile void *tag);
 *      extern "C" void __notify_zc_intrinsic(const char *annotation, const volatile void *tag);
 * 
 * The string annotation must be a compile-time constant. It specifies the type of the annotation.
 * The pointer tag is computed at run time. It specifies the data associated with the annotation.
 * Each intrinsic implies a compiler fence: the compiler must not move any memory
 * operation across it. The reason for this restriction is that annotation might denote an
 * event that must be precisely placed with respect to memory operations.
 * 
 * The difference between the two intrinsics is that __notify_intrinsic must leave a
 * probe-ready instruction sequence in the instruction stream where the instrinsic
 * occurs. The __notify_zc_intrinsic does not leave such a sequence, and hence is closer to "zero cost".
 **/

#pragma once
#include "ittnotify.h"

#ifndef INTEL_NO_ITTNOTIFY_API
#if (defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)) && (ITT_PLATFORM == ITT_PLATFORM_WIN || ITT_PLATFORM == ITT_PLATFORM_POSIX)
#define ITT_ENABLE_LOW_OVERHEAD_ANNOTATIONS
#else
#error Zero cost (low overhead) annotations are not supported on this platform
#endif
#endif

/**
 * Zero cost annotations for memory allocation and deallocation
 **/
#ifdef ITT_ENABLE_LOW_OVERHEAD_ANNOTATIONS
#pragma pack(push, 1)
typedef struct ___itt_zca_allocation_info {
    size_t size; /*!< Size of allocated memory */ 
    void** ptr; /*!< Pointer to allocated memory pointer */
    int initialized; /*!< Is allocated memory initialized */
} __itt_zca_allocation_info;
#pragma pack(pop)

#define __itt_zca_mem_allocate_begin() __notify_intrinsic((char*)"mem_allocate_begin", 0)
#define __itt_zca_mem_allocate_end(ptr, size, init) { __itt_zca_allocation_info __itt_zca_alloc_info = { size, ptr, init }; __notify_intrinsic((char*)"mem_allocate_end", (void*)&__itt_zca_alloc_info); }
#define __itt_zca_mem_free_begin(ptr) __notify_intrinsic((char*)"mem_free_begin", (void*)ptr)
#define __itt_zca_mem_free_end() __notify_intrinsic((char*)"mem_free_end", 0)
#else
#define __itt_zca_mem_allocate_begin()
#define __itt_zca_mem_allocate_end(ptr, size, init)
#define __itt_zca_mem_free_begin(ptr)
#define __itt_zca_mem_free_end()
#endif

/**
 * Zero cost annotations for threading
 **/
#ifdef ITT_ENABLE_LOW_OVERHEAD_ANNOTATIONS
#define __itt_zca_suppress_push(id) __notify_zc_intrinsic((char*)"__itt_suppress_push", (void*)id);
#define __itt_zca_suppress_pop(id) __notify_zc_intrinsic((char*)"__itt_suppress_pop", (void*)id);
#define __itt_zca_sync_create(id) __notify_zc_intrinsic((char*)"__itt_sync_create", (void*)id)
#define __itt_zca_sync_acquired(id) __notify_zc_intrinsic((char*)"__itt_sync_acquired", (void*)id)
#define __itt_zca_sync_releasing(id) __notify_zc_intrinsic((char*)"__itt_sync_releasing", (void*)id)
#define __itt_zca_sync_destroy(id) __notify_zc_intrinsic((char*)"__itt_sync_destroy", (void*)id)
#else
#define __itt_zca_suppress_push(id)
#define __itt_zca_suppress_pop(id)
#define __itt_zca_sync_create(id)
#define __itt_zca_sync_acquired(id)
#define __itt_zca_sync_releasing(id)
#define __itt_zca_sync_destroy(id)
#endif
