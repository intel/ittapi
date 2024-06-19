// Copyright (C) 2012-2017 Intel Corporation
// SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause

// This file defines functions used by the Intel(R) Advisor XE
// "Dependencies Modeling" and "Suitability Modeling" analysis, which
// are described in the "Annotations" section of the help.

// INTEL_ADVISOR_ANNOTATION_VERSION 1.0

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace AdvisorAnnotate
{
    public static class Annotate
    {
        // User annotations.  While inlining is desirable for performance, reliable
        // call stack reporting requires they not be inlined, or have frame pointers
        // omitted
        //
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void SiteBegin(string name)     { __itt_model_site_beginW(name); }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void SiteEnd()                  { __itt_model_site_end_2();      }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void TaskBegin(string name)     { __itt_model_task_beginW(name); }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void TaskEnd()                  { __itt_model_task_end_2();      }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void IterationTask(string name) { __itt_model_iteration_taskW(name);  }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void LockAcquire(long i)        { __itt_model_lock_acquire_2(i); }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void LockAcquire()              { __itt_model_lock_acquire_2(0); }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void LockRelease(long i)        { __itt_model_lock_release_2(i); }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void LockRelease()              { __itt_model_lock_release_2(0); }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void DisableObservationPush()   { __itt_model_disable_push(0);   }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void DisableObservationPop()    { __itt_model_disable_pop();     }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void DisableCollectionPush()    { __itt_model_disable_push(1);   }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void DisableCollectionPop()     { __itt_model_disable_pop();     }
        [MethodImpl(MethodImplOptions.NoInlining | MethodImplOptions.NoOptimization)]
        public static void AggregateTask(long i)      { __itt_model_aggregate_task(i); }

        // function signatures and corresponding no-op functions
        private delegate void sig_none();
        private static   void sig_none_nop() {}
        private delegate void sig_name([MarshalAs(UnmanagedType.LPWStr)]string name);
        private static   void sig_name_nop([MarshalAs(UnmanagedType.LPWStr)]string name) {}
        private delegate void sig_long(long i);
        private static   void sig_long_nop(long i) {}
        private delegate void sig_int(int i);
        private static   void sig_int_nop(int i) {}

        // external functions implementing annotations, defaulting to no-op functions
        private static sig_name __itt_model_site_beginW    = sig_name_nop;
        private static sig_none __itt_model_site_end_2     = sig_none_nop;
        private static sig_name __itt_model_task_beginW    = sig_name_nop;
        private static sig_none __itt_model_task_end_2     = sig_none_nop;
        private static sig_name __itt_model_iteration_taskW = sig_name_nop;
        private static sig_long __itt_model_lock_acquire_2 = sig_long_nop;
        private static sig_long __itt_model_lock_release_2 = sig_long_nop;
        private static sig_int  __itt_model_disable_push   = sig_int_nop;
        private static sig_none __itt_model_disable_pop    = sig_none_nop;
        private static sig_long __itt_model_aggregate_task = sig_long_nop;
        
        [DllImport("kernel32.dll")]
        private static extern IntPtr LoadLibrary(string dll);

        [DllImport("kernel32.dll")]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string procedureName);

        // Static constructor performs initialization of the delegates (function pointers)
        // for the case when data collection is actually occurring.
        static Annotate()
        {
            string libname;

            if (Environment.Is64BitOperatingSystem)
            {
                libname = Environment.GetEnvironmentVariable("INTEL_LIBITTNOTIFY64");
            }
            else
            {
                libname = Environment.GetEnvironmentVariable("INTEL_LIBITTNOTIFY32");
            }

            if (String.IsNullOrEmpty(libname))
            {
                libname = "libittnotify.dll";
            }

            IntPtr ittnotify = LoadLibrary(libname);

            if (ittnotify != IntPtr.Zero)
            {
                IntPtr p = GetProcAddress(ittnotify, "__itt_model_site_beginW");
                if (p != IntPtr.Zero) __itt_model_site_beginW =
                    (sig_name)Marshal.GetDelegateForFunctionPointer(p, typeof(sig_name));

                p = GetProcAddress(ittnotify, "__itt_model_site_end_2");
                if (p != IntPtr.Zero) __itt_model_site_end_2 =
                    (sig_none)Marshal.GetDelegateForFunctionPointer(p, typeof(sig_none));

                p = GetProcAddress(ittnotify, "__itt_model_task_beginW");
                if (p != IntPtr.Zero) __itt_model_task_beginW =
                    (sig_name)Marshal.GetDelegateForFunctionPointer(p, typeof(sig_name));

                p = GetProcAddress(ittnotify, "__itt_model_task_end_2");
                if (p != IntPtr.Zero) __itt_model_task_end_2 =
                    (sig_none)Marshal.GetDelegateForFunctionPointer(p, typeof(sig_none));

                p = GetProcAddress(ittnotify, "__itt_model_iteration_taskW");
                if (p != IntPtr.Zero) __itt_model_iteration_taskW =
                    (sig_name)Marshal.GetDelegateForFunctionPointer(p, typeof(sig_name));

                p = GetProcAddress(ittnotify, "__itt_model_lock_acquire_2");
                if (p != IntPtr.Zero) __itt_model_lock_acquire_2 =
                    (sig_long)Marshal.GetDelegateForFunctionPointer(p, typeof(sig_long));

                p = GetProcAddress(ittnotify, "__itt_model_lock_release_2");
                if (p != IntPtr.Zero) __itt_model_lock_release_2 =
                    (sig_long)Marshal.GetDelegateForFunctionPointer(p, typeof(sig_long));

                p = GetProcAddress(ittnotify, "__itt_model_disable_push");
                if (p != IntPtr.Zero) __itt_model_disable_push =
                    (sig_int) Marshal.GetDelegateForFunctionPointer(p, typeof(sig_int));

                p = GetProcAddress(ittnotify, "__itt_model_disable_pop");
                if (p != IntPtr.Zero) __itt_model_disable_pop =
                    (sig_none)Marshal.GetDelegateForFunctionPointer(p, typeof(sig_none));

                p = GetProcAddress(ittnotify, "__itt_model_aggregate_task");
                if (p != IntPtr.Zero) __itt_model_aggregate_task =
                    (sig_long)Marshal.GetDelegateForFunctionPointer(p, typeof(sig_long));
            }

        }

    }
}
