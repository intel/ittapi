/*
  Copyright (C) 2020-2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef _WIN32
#include <unistd.h>
#if defined(__linux__)
#include <sys/syscall.h>
#endif
#endif

#define INTEL_NO_MACRO_BODY
#define INTEL_ITTNOTIFY_API_PRIVATE
#include "ittnotify.h"
#include "ittnotify_config.h"

#define LOG_BUFFER_MAX_SIZE 256

static const char* env_log_dir  = "INTEL_LIBITTNOTIFY_LOG_DIR";

// The reference collector supports two output modes:
//   mode 1 (default): a plain-text log with one line per ITT API call.
//   mode 2:           a Perfetto / Chrome Trace Event trace in JSON.
// Mode 2 is enabled by setting EXP_LIBITTNOTIFY_GEN_JSON to a non-zero value.
static const char* env_gen_json = "EXP_LIBITTNOTIFY_GEN_JSON";

// Selected output mode: 0 = plain-text logging (default), 1 = JSON trace.
static int g_gen_json = 0;

// Plain-text logger (mode 1) log levels.
static const char* log_level_str[] = {"INFO", "WARN", "ERROR", "FATAL_ERROR"};

enum {
    LOG_LVL_INFO,
    LOG_LVL_WARN,
    LOG_LVL_ERROR,
    LOG_LVL_FATAL
};

static struct ref_collector_logger {
    FILE*   log_fp;
    uint8_t init_state;
    uint8_t first_event;  // JSON mode only: false once first event written (controls comma separators)
} g_ref_collector_logger = {NULL, 0, 1};

// Collector maintains its own object lists instead of relying on __itt_global*,
// because traced apps may contain multiple static ITT parts, each with its own __itt_global*.
// By keeping separate lists, the collector can aggregate and track all ITT objects
// across all static parts in a single unified view.
static struct ref_collector_global {
    mutex_t                mutex;
    volatile long          mutex_initialized;
    __itt_domain*          domain_list;
    __itt_string_handle*   string_list;
    __itt_counter_info_t*  counter_list;
    __itt_histogram*       histogram_list;
} g_ref_collector_global = {MUTEX_INITIALIZER, 0, NULL, NULL, NULL, NULL};

#ifdef _WIN32
    #define REFCOL_LOCALTIME(out_tm, in_time) (localtime_s((out_tm), (in_time)) == 0)
#else
    #define REFCOL_LOCALTIME(out_tm, in_time) (localtime_r((in_time), (out_tm)) != NULL)
#endif

static char* log_file_name_generate(void)
{
    time_t time_now = time(NULL);
    struct tm time_info;
    char* log_file_name = malloc(sizeof(char) * (LOG_BUFFER_MAX_SIZE/2));

    if (log_file_name == NULL)
    {
        printf("ERROR: Failed to allocate memory for log file name\n");
        return NULL;
    }

    if (!REFCOL_LOCALTIME(&time_info, &time_now))
    {
        printf("ERROR: Failed to get local time for log file name\n");
        free(log_file_name);
        return NULL;
    }

    // Zero-padded, ordered timestamp (YYYYMMDD_HHMMSS) so file names sort
    // chronologically and cannot collide between different dates/times.
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y%m%d_%H%M%S", &time_info);
    snprintf(log_file_name, LOG_BUFFER_MAX_SIZE/2, "libittnotify_refcol_%s.%s",
             time_str, g_gen_json ? "json" : "log");

    return log_file_name;
}

// This reference implementation opens an output file for recording ITT API calls.
// The output format (plain-text log vs JSON trace) is chosen from the
// EXP_LIBITTNOTIFY_GEN_JSON environment variable. Custom collectors can replace
// this with their own initialization logic (e.g., opening trace files,
// connecting to profiler backends, allocating buffers).
static void ref_collector_init()
{
    if (!g_ref_collector_logger.init_state)
    {
        static char file_name_buffer[LOG_BUFFER_MAX_SIZE*2];
        char* gen_json = getenv(env_gen_json);
        g_gen_json = (gen_json != NULL && atoi(gen_json) != 0);

        char* log_dir = getenv(env_log_dir);
        char* log_file = log_file_name_generate();
        if (log_file == NULL)
        {
            return;
        }

        if (log_dir != NULL)
        {
            #ifdef _WIN32
                snprintf(file_name_buffer, sizeof(file_name_buffer), "%s\\%s", log_dir, log_file);
            #else
                snprintf(file_name_buffer, sizeof(file_name_buffer), "%s/%s", log_dir, log_file);
            #endif
        }
        else
        {
            #ifdef _WIN32
                char* temp_dir = getenv("TEMP");
                if (temp_dir != NULL)
                {
                    snprintf(file_name_buffer, sizeof(file_name_buffer), "%s\\%s", temp_dir, log_file);
                }
                else
                {
                    snprintf(file_name_buffer, sizeof(file_name_buffer), "%s", log_file);
                }
            #else
                snprintf(file_name_buffer, sizeof(file_name_buffer), "/tmp/%s", log_file);
            #endif
        }
        free(log_file);

        // JSON traces are truncated per run ("w"); plain logs are appended ("a").
        g_ref_collector_logger.log_fp = fopen(file_name_buffer, g_gen_json ? "w" : "a");
        if (!g_ref_collector_logger.log_fp)
        {
            printf("ERROR: Cannot open output file: %s\n", file_name_buffer);
            return;
        }

        if (g_gen_json)
        {
            // Open the Chrome Trace Event JSON array.
            fprintf(g_ref_collector_logger.log_fp, "[\n");
            g_ref_collector_logger.first_event = 1;
        }
        g_ref_collector_logger.init_state = 1;
    }
}

// Required stub for ittnotify_config.h macros
// Implement actual error handling here if needed by your collector.
static void __itt_report_error(int code, ...)
{
    (void)code;
}

// Initialize the collector's mutex for thread-safe access to object lists.
// Must be called before any ITT API functions that access the lists.
static void ref_collector_init_mutex()
{
    if (!g_ref_collector_global.mutex_initialized)
    {
        __itt_mutex_init(&g_ref_collector_global.mutex);
        g_ref_collector_global.mutex_initialized = 1;
    }
}

// Cleanup hook called at program exit via atexit().
// Releases all resources: closes log file, frees all ITT objects.
static void ref_collector_release(void)
{
    static int released = 0;
    if (released) return;
    released = 1;

    if (g_ref_collector_logger.log_fp)
    {
        if (g_gen_json)
        {
            // Close the Chrome Trace Event JSON array.
            fprintf(g_ref_collector_logger.log_fp, "\n]\n");
        }
        fclose(g_ref_collector_logger.log_fp);
        g_ref_collector_logger.log_fp = NULL;
    }

    if (!g_ref_collector_global.mutex_initialized) return;

    __itt_mutex_lock(&g_ref_collector_global.mutex);

    __itt_domain *d = g_ref_collector_global.domain_list;
    while (d)
    {
        __itt_domain *next = d->next;
        if (d->nameA) free((char*)d->nameA);
        free(d);
        d = next;
    }
    g_ref_collector_global.domain_list = NULL;

    __itt_string_handle *s = g_ref_collector_global.string_list;
    while (s)
    {
        __itt_string_handle *next = s->next;
        if (s->strA) free((char*)s->strA);
        free(s);
        s = next;
    }
    g_ref_collector_global.string_list = NULL;

    __itt_counter_info_t *c = g_ref_collector_global.counter_list;
    while (c)
    {
        __itt_counter_info_t *next = c->next;
        if (c->nameA) free((char*)c->nameA);
        if (c->domainA) free((char*)c->domainA);
        free(c);
        c = next;
    }
    g_ref_collector_global.counter_list = NULL;

    __itt_histogram *h = g_ref_collector_global.histogram_list;
    while (h)
    {
        __itt_histogram *next = h->next;
        if (h->nameA) free((char*)h->nameA);
        free(h);
        h = next;
    }
    g_ref_collector_global.histogram_list = NULL;

    g_ref_collector_global.mutex_initialized = 0;
    __itt_mutex_unlock(&g_ref_collector_global.mutex);
    __itt_mutex_destroy(&g_ref_collector_global.mutex);
}

// Resolve and bind ITT API function pointers from collector
// to the static part's function pointer table
static void fill_func_ptr_per_lib(__itt_global* p)
{
    __itt_api_info* api_list = (__itt_api_info*)p->api_list_ptr;

    for (int i = 0; api_list[i].name != NULL; i++)
    {
        *(api_list[i].func_ptr) = (void*)__itt_get_proc(p->lib, api_list[i].name);
        if (*(api_list[i].func_ptr) == NULL)
        {
            *(api_list[i].func_ptr) = api_list[i].null_func;
        }
    }
}

// Spill and copy existing objects from static part lists to collector's lists
static void spill_static_part_lists(__itt_global* p)
{
    __itt_mutex_lock(&p->mutex);

    for (__itt_domain *d = p->domain_list; d != NULL; d = d->next)
    {
        (void)__itt_domain_create(d->nameA);
    }

    for (__itt_string_handle *sh = p->string_list; sh != NULL; sh = sh->next)
    {
        (void)__itt_string_handle_create(sh->strA);
    }

    for (__itt_counter_info_t *c = p->counter_list; c != NULL; c = c->next)
    {
        (void)__itt_counter_create_typed(c->nameA, c->domainA, c->type);
    }

    for (__itt_histogram *h = p->histogram_list; h != NULL; h = h->next)
    {
        (void)__itt_histogram_create(h->domain, h->nameA, h->x_type, h->y_type);
    }

    __itt_mutex_unlock(&p->mutex);
}

// Entry point called by each static ITT part during initialization.
// Binds collector's API implementations and spills pre-existing objects.
ITT_EXTERN_C void ITTAPI __itt_api_init(__itt_global* p, __itt_group_id init_groups)
{
    if (p != NULL)
    {
        (void)init_groups;
        fill_func_ptr_per_lib(p);
        ref_collector_init();
        ref_collector_init_mutex();

        spill_static_part_lists(p);

        atexit(ref_collector_release);
    }
    else
    {
        printf("ERROR: Failed to initialize dynamic library\n");
    }
}

// ----------------------------------------------------------------------------
// Mode 2 backend: Perfetto / Chrome Trace Event writer
//
// Every instrumented ITT API call is translated into one or more trace events
// and appended to the JSON array opened in ref_collector_init(). The resulting
// file loads directly in https://ui.perfetto.dev or chrome://tracing.
//
// Event phase mapping:
//   task begin / end     -> "B" / "E"  (synchronous, per-thread, nestable)
//   region begin / end   -> "b" / "e"  (asynchronous, matched by id)
//   frame begin / end    -> "b" / "e"  (asynchronous, matched by id)
//   frame submit         -> "X"        (complete event with explicit duration)
//   counter set value    -> "C"        (counter series)
//   everything else      -> "i"        (thread-scoped instant marker)
// ----------------------------------------------------------------------------

#ifdef _WIN32
static uint64_t get_timestamp_us(void)
{
    // QueryPerformanceFrequency is fixed at system boot, so query it only once
    // and reuse the cached value on every subsequent call.
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER cnt;
    if (freq.QuadPart == 0)
        QueryPerformanceFrequency(&freq);
    if (freq.QuadPart == 0) return 0;
    QueryPerformanceCounter(&cnt);
    return (uint64_t)((cnt.QuadPart * 1000000ULL) / (uint64_t)freq.QuadPart);
}
static int get_process_id(void) { return (int)GetCurrentProcessId(); }
#else
static uint64_t get_timestamp_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;
}
static int get_process_id(void) { return (int)getpid(); }
#endif

// Return a small OS-level thread id. Perfetto / chrome://tracing expect the
// numeric kernel thread id (as reported by perf, ftrace, gettid), not the
// large opaque pthread_t handle. Using the raw pthread_self() value here would
// make the UI collapse every thread into a single "main thread" track.
static unsigned long long get_thread_id(void)
{
#if defined(_WIN32)
    return (unsigned long long)GetCurrentThreadId();
#else
    return (unsigned long long)syscall(SYS_gettid);
#endif
}

// Escape a string so it can be safely embedded inside a JSON string literal.
static void json_escape(const char* src, char* dst, size_t dst_size)
{
    size_t j = 0;
    if (dst_size == 0) return;
    if (src == NULL) { dst[0] = '\0'; return; }

    for (size_t i = 0; src[i] != '\0' && j + 1 < dst_size; i++)
    {
        unsigned char c = (unsigned char)src[i];
        switch (c)
        {
            case '\"': if (j + 2 < dst_size) { dst[j++] = '\\'; dst[j++] = '\"'; } break;
            case '\\': if (j + 2 < dst_size) { dst[j++] = '\\'; dst[j++] = '\\'; } break;
            case '\n': if (j + 2 < dst_size) { dst[j++] = '\\'; dst[j++] = 'n';  } break;
            case '\r': if (j + 2 < dst_size) { dst[j++] = '\\'; dst[j++] = 'r';  } break;
            case '\t': if (j + 2 < dst_size) { dst[j++] = '\\'; dst[j++] = 't';  } break;
            default:
                if (c < 0x20)
                {
                    if (j + 6 < dst_size)
                        j += (size_t)snprintf(dst + j, dst_size - j, "\\u%04x", c);
                }
                else
                {
                    dst[j++] = (char)c;
                }
                break;
        }
    }
    dst[j] = '\0';
}

// Append a single trace event object to the JSON array. Thread-safe.
// `extra` (may be NULL) is raw JSON appended after the mandatory fields, e.g.
// ",\"dur\":123", ",\"id\":\"0x1\"", ",\"s\":\"t\"", ",\"args\":{...}".
static void json_write(const char* phase, const char* cat, const char* name,
                       uint64_t ts, const char* extra)
{
    if (!g_ref_collector_logger.init_state || !g_ref_collector_logger.log_fp)
        return;
    if (!g_ref_collector_global.mutex_initialized)
        return;

    char name_esc[LOG_BUFFER_MAX_SIZE];
    char cat_esc[LOG_BUFFER_MAX_SIZE];
    json_escape(name, name_esc, sizeof(name_esc));
    json_escape(cat, cat_esc, sizeof(cat_esc));

    __itt_mutex_lock(&g_ref_collector_global.mutex);

    fprintf(g_ref_collector_logger.log_fp,
            "%s{\"name\":\"%s\",\"cat\":\"%s\",\"ph\":\"%s\",\"ts\":%" PRIu64
            ",\"pid\":%d,\"tid\":%llu",
            g_ref_collector_logger.first_event ? "" : ",\n",
            name_esc, cat_esc, phase, ts, get_process_id(), get_thread_id());
    if (extra != NULL)
        fputs(extra, g_ref_collector_logger.log_fp);
    fputc('}', g_ref_collector_logger.log_fp);

    g_ref_collector_logger.first_event = 0;

    __itt_mutex_unlock(&g_ref_collector_global.mutex);
}

// ----------------------------------------------------------------------------
// The code below is a reference implementation of the ITT API functions.
// These functions are bound to static parts via fill_func_ptr_per_lib()
// and log all ITT API calls to a file for reference/debugging purposes.
// In a real collector, these functions could save trace data to a file,
// forward events to a profiler, or perform any other instrumentation work.
// ----------------------------------------------------------------------------

// Remember to call free() after using get_metadata_elements()
static char* get_metadata_elements(size_t size, __itt_metadata_type type, void* metadata)
{
    char* metadata_str = malloc(sizeof(char) * LOG_BUFFER_MAX_SIZE);
    if (metadata_str == NULL) return NULL;
    metadata_str[0] = '\0';
    size_t offset = 0;

    // Bounded append into metadata_str: snprintf returns the number of chars it
    // *would* have written, so clamp by the remaining space to make sure offset
    // never runs past the buffer (and stop on encoding errors / when full).
    #define APPEND_METADATA(fmt, val)                                              \
        do {                                                                       \
            if (offset >= LOG_BUFFER_MAX_SIZE - 1) break;                          \
            int _n = snprintf(metadata_str + offset, LOG_BUFFER_MAX_SIZE - offset, \
                              fmt ";", (val));                                     \
            if (_n < 0) break;                                                     \
            offset += ((size_t)_n < LOG_BUFFER_MAX_SIZE - offset)                  \
                          ? (size_t)_n : (LOG_BUFFER_MAX_SIZE - offset - 1);       \
        } while (0)

    switch (type)
    {
    case __itt_metadata_u64:
        for (size_t i = 0; i < size; i++) APPEND_METADATA("%" PRIu64, ((uint64_t*)metadata)[i]);
        break;
    case __itt_metadata_s64:
        for (size_t i = 0; i < size; i++) APPEND_METADATA("%" PRId64, ((int64_t*)metadata)[i]);
        break;
    case __itt_metadata_u32:
        for (size_t i = 0; i < size; i++) APPEND_METADATA("%u", ((uint32_t*)metadata)[i]);
        break;
    case __itt_metadata_s32:
        for (size_t i = 0; i < size; i++) APPEND_METADATA("%d", ((int32_t*)metadata)[i]);
        break;
    case __itt_metadata_u16:
        for (size_t i = 0; i < size; i++) APPEND_METADATA("%u", ((uint16_t*)metadata)[i]);
        break;
    case __itt_metadata_s16:
        for (size_t i = 0; i < size; i++) APPEND_METADATA("%d", ((int16_t*)metadata)[i]);
        break;
    case __itt_metadata_float:
        for (size_t i = 0; i < size; i++) APPEND_METADATA("%f", ((float*)metadata)[i]);
        break;
    case __itt_metadata_double:
        for (size_t i = 0; i < size; i++) APPEND_METADATA("%lf", ((double*)metadata)[i]);
        break;
    default:
        printf("ERROR: Unknown metadata type\n");
        break;
    }
    #undef APPEND_METADATA

    return metadata_str;
}

// Remember to call free() after using get_context_metadata_element()
static char* get_context_metadata_element(__itt_context_type type, void* metadata)
{
    char* metadata_str = malloc(sizeof(char) * (LOG_BUFFER_MAX_SIZE/4));
    if (metadata_str == NULL) return NULL;
    *metadata_str = '\0';

    switch(type)
    {
        case __itt_context_name:
        case __itt_context_device:
        case __itt_context_units:
        case __itt_context_pci_addr:
            snprintf(metadata_str, LOG_BUFFER_MAX_SIZE/4, "%s;", ((char*)metadata));
            break;
        case __itt_context_max_val:
        case __itt_context_tid:
        case __itt_context_on_thread_flag:
        case __itt_context_bandwidth_flag:
        case __itt_context_latency_flag:
        case __itt_context_occupancy_flag:
        case __itt_context_cpu_instructions_flag:
        case __itt_context_cpu_cycles_flag:
        case __itt_context_is_abs_val_flag:
            snprintf(metadata_str, LOG_BUFFER_MAX_SIZE/4, "%" PRIu64 ";", *(uint64_t*)metadata);
            break;
        default:
            printf("ERROR: Unknown context metadata type\n");
            break;
    }

    return metadata_str;
}

// Concatenate context-metadata elements into `buf` (bounded by `cap`). Shared by
// the JSON and plain-text handlers of __itt_bind_context_metadata_to_counter.
// snprintf's return value is clamped so `offset` never runs past the buffer.
static void build_context_metadata_str(char* buf, size_t cap, size_t length, __itt_context_metadata* metadata)
{
    if (cap == 0) return;
    buf[0] = '\0';
    size_t offset = 0;
    for (size_t i = 0; i < length && offset < cap - 1; i++)
    {
        char* elem = get_context_metadata_element(metadata[i].type, metadata[i].value);
        if (elem == NULL) break;
        int n = snprintf(buf + offset, cap - offset, "%s", elem);
        free(elem);
        if (n < 0) break;
        offset += ((size_t)n < cap - offset) ? (size_t)n : (cap - offset - 1);
    }
}

#ifdef _WIN32
static char* wchar2char(const wchar_t* wide_str)
{
    if (wide_str == NULL)
    {
        return NULL;
    }

    int narrow_size = WideCharToMultiByte(CP_UTF8, 0, wide_str, -1, NULL, 0, NULL, NULL);
    if (narrow_size <= 0)
    {
        return NULL;
    }

    char* narrow_str = (char*)malloc((size_t)narrow_size);
    if (narrow_str == NULL)
    {
        return NULL;
    }

    if (!WideCharToMultiByte(CP_UTF8, 0, wide_str, -1, narrow_str, narrow_size, NULL, NULL))
    {
        free(narrow_str);
        return NULL;
    }

    return narrow_str;
}
#endif

// ----------------------------------------------------------------------------
// Mode 1 backend: plain-text call logger
//
// Writes one human-readable line per instrumented ITT API call to a .log file.
// This is the original reference-collector behavior and is used by default
// (when EXP_LIBITTNOTIFY_GEN_JSON is unset or zero).
// ----------------------------------------------------------------------------

static void log_func_call(uint8_t log_level, const char* function_name, const char* message_format, ...)
{
    if (!g_ref_collector_logger.init_state || !g_ref_collector_logger.log_fp)
    {
        printf("ERROR: Failed to log function call\n");
        return;
    }

    char log_buffer[LOG_BUFFER_MAX_SIZE];
    uint32_t result_len = 0;
    va_list message_args;

    result_len += snprintf(log_buffer, LOG_BUFFER_MAX_SIZE, "[%s] %s(...) - ", log_level_str[log_level], function_name);
    if (result_len >= LOG_BUFFER_MAX_SIZE)
        result_len = LOG_BUFFER_MAX_SIZE - 1;
    va_start(message_args, message_format);
    vsnprintf(log_buffer + result_len, LOG_BUFFER_MAX_SIZE - result_len, message_format, message_args);
    va_end(message_args);

    fprintf(g_ref_collector_logger.log_fp, "%s\n", log_buffer);
}

// Log a successful ITT API call. The uniform "function call" / "function args:"
// wording lives here instead of at every call site: with no variadic arguments
// the line reads "<api>(...) - function call"; with a printf-style format
// (+args) it reads "<api>(...) - function args: <formatted>".
static void log_api_call(uint8_t log_level, const char* function_name, const char* args_format, ...)
{
    if (args_format == NULL || args_format[0] == '\0')
    {
        log_func_call(log_level, function_name, "function call");
        return;
    }

    char formatted[LOG_BUFFER_MAX_SIZE];
    va_list message_args;
    va_start(message_args, args_format);
    vsnprintf(formatted, sizeof(formatted), args_format, message_args);
    va_end(message_args);

    log_func_call(log_level, function_name, "function args: %s", formatted);
}

// INFO logs record a successful call and go through log_api_call(), so callers
// pass only the argument format (or nothing) - the "function call" /
// "function args:" boilerplate is added automatically. The empty-string default
// ("" __VA_ARGS__) makes the format optional for the argument-less case.
// WARN/ERROR/FATAL logs carry a custom message and go through log_func_call()
// verbatim.
#define LOG_FUNC_CALL_INFO(...)  log_api_call(LOG_LVL_INFO, __FUNCTION__, "" __VA_ARGS__)
#define LOG_FUNC_CALL_WARN(...)  log_func_call(LOG_LVL_WARN, __FUNCTION__, __VA_ARGS__)
#define LOG_FUNC_CALL_ERROR(...) log_func_call(LOG_LVL_ERROR, __FUNCTION__, __VA_ARGS__)
#define LOG_FUNC_CALL_FATAL(...) log_func_call(LOG_LVL_FATAL, __FUNCTION__, __VA_ARGS__)

// Same as LOG_FUNC_CALL_* but with an explicit ITT API name. Used by the grouped
// log_* handlers below, whose own __FUNCTION__ is not the traced ITT API name.
#define LOG_CALL_INFO(api, ...)  log_api_call(LOG_LVL_INFO, (api), "" __VA_ARGS__)
#define LOG_CALL_WARN(api, ...)  log_func_call(LOG_LVL_WARN, (api), __VA_ARGS__)

// ----------------------------------------------------------------------------
// ITT API entry points
//
// Each entry point validates its arguments, performs any shared bookkeeping
// (e.g. object-list management), then dispatches to the selected backend:
// the JSON writer (mode 2) when g_gen_json is set, otherwise the plain-text
// logger (mode 1).
// ----------------------------------------------------------------------------

#ifdef _WIN32
ITT_EXTERN_C __itt_domain* ITTAPI __itt_domain_createW(const wchar_t *name)
{
    char* name_a = wchar2char(name);
    __itt_domain* result = __itt_domain_createA(name_a);
    free(name_a);
    return result;
}
ITT_EXTERN_C __itt_domain* ITTAPI __itt_domain_createA(const char *name)
#else
ITT_EXTERN_C __itt_domain* ITTAPI __itt_domain_create(const char *name)
#endif
{
    if (!g_ref_collector_global.mutex_initialized || name == NULL)
    {
        if (!g_gen_json) LOG_FUNC_CALL_WARN("Cannot create domain object");
        return NULL;
    }

    __itt_domain *h_tail = NULL, *h = NULL;

    __itt_mutex_lock(&g_ref_collector_global.mutex);

    for (h_tail = NULL, h = g_ref_collector_global.domain_list; h != NULL; h_tail = h, h = h->next)
    {
        if (h->nameA != NULL && !strcmp(h->nameA, name)) break;
    }

    if (h == NULL)
    {
        NEW_DOMAIN_A(&g_ref_collector_global, h, h_tail, name);
        if (!g_gen_json) LOG_FUNC_CALL_INFO("name=%s (created new domain)", name);
    }
    else if (!g_gen_json)
    {
        LOG_FUNC_CALL_INFO("name=%s (domain already exists)", name);
    }

    __itt_mutex_unlock(&g_ref_collector_global.mutex);

    return h;
}

#ifdef _WIN32
ITT_EXTERN_C __itt_string_handle* ITTAPI __itt_string_handle_createW(const wchar_t* name)
{
    char* name_a = wchar2char(name);
    __itt_string_handle* result = __itt_string_handle_createA(name_a);
    free(name_a);
    return result;
}
ITT_EXTERN_C __itt_string_handle* ITTAPI __itt_string_handle_createA(const char* name)
#else
ITT_EXTERN_C __itt_string_handle* ITTAPI __itt_string_handle_create(const char* name)
#endif
{
    if (!g_ref_collector_global.mutex_initialized || name == NULL)
    {
        if (!g_gen_json) LOG_FUNC_CALL_WARN("Cannot create string handle object");
        return NULL;
    }

    __itt_string_handle *h_tail = NULL, *h = NULL;

    __itt_mutex_lock(&g_ref_collector_global.mutex);

    for (h_tail = NULL, h = g_ref_collector_global.string_list; h != NULL; h_tail = h, h = h->next)
    {
        if (h->strA != NULL && !strcmp(h->strA, name)) break;
    }

    if (h == NULL)
    {
        NEW_STRING_HANDLE_A(&g_ref_collector_global, h, h_tail, name);
        if (!g_gen_json) LOG_FUNC_CALL_INFO("name=%s (created new string handle)", name);
    }
    else if (!g_gen_json)
    {
        LOG_FUNC_CALL_INFO("name=%s (string handle already exists)", name);
    }

    __itt_mutex_unlock(&g_ref_collector_global.mutex);

    return h;
}

#ifdef _WIN32
ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_createW(const wchar_t *name, const wchar_t *domain)
{
    char* name_a = wchar2char(name);
    char* domain_a = wchar2char(domain);
    __itt_counter result = __itt_counter_createA(name_a, domain_a);
    free(name_a);
    free(domain_a);
    return result;
}
ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_createA(const char *name, const char *domain)
#else
ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_create(const char *name, const char *domain)
#endif
{
    // Thin wrapper: __itt_counter_create_typed() emits the single log line.
    return __itt_counter_create_typed(name, domain, __itt_metadata_u64);
}

#ifdef _WIN32
ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_createW_v3(
    const __itt_domain* domain, const wchar_t* name, __itt_metadata_type type)
{
    if (domain == NULL) return NULL;
    char* name_a = wchar2char(name);
    if (name_a == NULL) return NULL;
    __itt_counter result = __itt_counter_create_typed(name_a, domain->nameA, type);
    free(name_a);
    return result;
}
ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_createA_v3(
    const __itt_domain* domain, const char* name, __itt_metadata_type type)
#else
ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_create_v3(
    const __itt_domain* domain, const char* name, __itt_metadata_type type)
#endif
{
    if (domain == NULL)
    {
        if (!g_gen_json) LOG_FUNC_CALL_WARN("domain is NULL");
        return NULL;
    }
    // Thin wrapper: __itt_counter_create_typed() emits the single log line.
    return __itt_counter_create_typed(name, domain->nameA, type);
}

#ifdef _WIN32
ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_create_typedW(
    const wchar_t *name, const wchar_t *domain, __itt_metadata_type type)
{
    char* name_a = wchar2char(name);
    char* domain_a = wchar2char(domain);
    __itt_counter result = __itt_counter_create_typedA(name_a, domain_a, type);
    free(name_a);
    free(domain_a);
    return result;
}
ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_create_typedA(
    const char *name, const char *domain, __itt_metadata_type type)
#else
ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_create_typed(
    const char *name, const char *domain, __itt_metadata_type type)
#endif
{
    if (!g_ref_collector_global.mutex_initialized || name == NULL || domain == NULL)
    {
        if (!g_gen_json) LOG_FUNC_CALL_WARN("Cannot create counter object");
        return NULL;
    }

    __itt_counter_info_t *h_tail = NULL, *h = NULL;

    __itt_mutex_lock(&g_ref_collector_global.mutex);

    for (h_tail = NULL, h = g_ref_collector_global.counter_list; h != NULL; h_tail = h, h = h->next)
    {
        if (h->nameA && h->type == (int)type && !strcmp(h->nameA, name) &&
            ((h->domainA == NULL && domain == NULL) ||
            (h->domainA && domain && !strcmp(h->domainA, domain)))) break;
    }

    if (h == NULL)
    {
        NEW_COUNTER_A(&g_ref_collector_global, h, h_tail, name, domain, type);
        if (!g_gen_json) LOG_FUNC_CALL_INFO("name=%s, domain=%s, type=%d (created new counter)",
                            name, domain, (int)type);
    }
    else if (!g_gen_json)
    {
        LOG_FUNC_CALL_INFO("name=%s, domain=%s, type=%d (counter already exists)",
                            name, domain, (int)type);
    }

    __itt_mutex_unlock(&g_ref_collector_global.mutex);

    return (__itt_counter)h;
}

#ifdef _WIN32
ITT_EXTERN_C __itt_histogram* ITTAPI __itt_histogram_createW(
    const __itt_domain* domain, const wchar_t* name, __itt_metadata_type x_type, __itt_metadata_type y_type)
{
    char* name_a = wchar2char(name);
    __itt_histogram* result = __itt_histogram_createA(domain, name_a, x_type, y_type);
    free(name_a);
    return result;
}
ITT_EXTERN_C __itt_histogram* ITTAPI __itt_histogram_createA(
    const __itt_domain* domain, const char* name, __itt_metadata_type x_type, __itt_metadata_type y_type)
#else
ITT_EXTERN_C __itt_histogram* ITTAPI __itt_histogram_create(
    const __itt_domain* domain, const char* name, __itt_metadata_type x_type, __itt_metadata_type y_type)
#endif
{
    if (!g_ref_collector_global.mutex_initialized || name == NULL || domain == NULL)
    {
        if (!g_gen_json) LOG_FUNC_CALL_WARN("Cannot create histogram object");
        return NULL;
    }

    __itt_histogram *h_tail = NULL, *h = NULL;

    __itt_mutex_lock(&g_ref_collector_global.mutex);

    for (h_tail = NULL, h = g_ref_collector_global.histogram_list; h != NULL; h_tail = h, h = h->next)
    {
        if (h->domain == domain && h->nameA && !strcmp(h->nameA, name)) break;
    }

    if (h == NULL)
    {
        NEW_HISTOGRAM_A(&g_ref_collector_global, h, h_tail, domain, name, x_type, y_type);
        if (!g_gen_json) LOG_FUNC_CALL_INFO("domain=%s, name=%s, x_type=%d, y_type=%d (created new histogram)",
            domain->nameA, name, x_type, y_type);
    }
    else if (!g_gen_json)
    {
        LOG_FUNC_CALL_INFO("domain=%s, name=%s, x_type=%d, y_type=%d (histogram already exists)",
            domain->nameA, name, x_type, y_type);
    }

    __itt_mutex_unlock(&g_ref_collector_global.mutex);

    return h;
}

// ----------------------------------------------------------------------------
// Mode 2 event handlers (JSON)
//
// One json_* handler per traced ITT API call; every handler writes through
// json_write(). Argument validation lives inside each handler so the ITT entry
// points (further below) can stay as thin one-line dispatchers.
// ----------------------------------------------------------------------------

static void json_pause(void)
{
    json_write("i", "itt", "__itt_pause", get_timestamp_us(),
               ",\"s\":\"t\",\"args\":{\"api\":\"pause\"}");
}

static void json_pause_scoped(__itt_collection_scope scope)
{
    char extra[LOG_BUFFER_MAX_SIZE];
    snprintf(extra, sizeof(extra),
             ",\"s\":\"t\",\"args\":{\"api\":\"pause\",\"scope\":%d}", (int)scope);
    json_write("i", "itt", "__itt_pause_scoped", get_timestamp_us(), extra);
}

static void json_resume(void)
{
    json_write("i", "itt", "__itt_resume", get_timestamp_us(),
               ",\"s\":\"t\",\"args\":{\"api\":\"resume\"}");
}

static void json_resume_scoped(__itt_collection_scope scope)
{
    char extra[LOG_BUFFER_MAX_SIZE];
    snprintf(extra, sizeof(extra),
             ",\"s\":\"t\",\"args\":{\"api\":\"resume\",\"scope\":%d}", (int)scope);
    json_write("i", "itt", "__itt_resume_scoped", get_timestamp_us(), extra);
}

static void json_detach(void)
{
    json_write("i", "itt", "__itt_detach", get_timestamp_us(),
               ",\"s\":\"t\",\"args\":{\"api\":\"detach\"}");
}

// Emit a Chrome/Perfetto "thread_name" metadata (M) event so the calling
// thread's track is labeled in the UI.
static void json_thread_set_name(const char* name)
{
    if (name == NULL) return;

    char name_esc[LOG_BUFFER_MAX_SIZE];
    json_escape(name, name_esc, sizeof(name_esc));

    char extra[LOG_BUFFER_MAX_SIZE * 2];
    snprintf(extra, sizeof(extra), ",\"args\":{\"name\":\"%s\"}", name_esc);
    json_write("M", "__metadata", "thread_name", get_timestamp_us(), extra);
}

static void json_frame_begin_v3(const __itt_domain *domain, __itt_id *id)
{
    if (domain == NULL) return;
    unsigned long long fid = (id != NULL) ? id->d1 : 0;
    char extra[LOG_BUFFER_MAX_SIZE];
    snprintf(extra, sizeof(extra),
             ",\"id\":\"0x%llx\",\"args\":{\"api\":\"frame\"}", fid);
    json_write("b", domain->nameA, domain->nameA, get_timestamp_us(), extra);
}

static void json_frame_end_v3(const __itt_domain *domain, __itt_id *id)
{
    if (domain == NULL) return;
    unsigned long long fid = (id != NULL) ? id->d1 : 0;
    char extra[LOG_BUFFER_MAX_SIZE];
    snprintf(extra, sizeof(extra),
             ",\"id\":\"0x%llx\",\"args\":{\"api\":\"frame\"}", fid);
    json_write("e", domain->nameA, domain->nameA, get_timestamp_us(), extra);
}

static void json_frame_submit_v3(const __itt_domain *domain,
    __itt_timestamp begin, __itt_timestamp end)
{
    if (domain == NULL) return;
    uint64_t dur = (end > begin) ? (uint64_t)(end - begin) : 0;
    char extra[LOG_BUFFER_MAX_SIZE];
    snprintf(extra, sizeof(extra),
             ",\"dur\":%" PRIu64 ",\"args\":{\"api\":\"frame\","
             "\"time_begin\":%llu,\"time_end\":%llu}",
             dur, (unsigned long long)begin, (unsigned long long)end);
    json_write("X", domain->nameA, domain->nameA, (uint64_t)begin, extra);
}

static void json_task_begin(
    const __itt_domain *domain, __itt_id taskid, __itt_id parentid, __itt_string_handle *name)
{
    if (domain == NULL || name == NULL) return;
    char extra[LOG_BUFFER_MAX_SIZE];
    snprintf(extra, sizeof(extra),
             ",\"args\":{\"api\":\"task\","
             "\"taskid\":\"%llu,%llu,%llu\",\"parentid\":\"%llu,%llu,%llu\"}",
             taskid.d1, taskid.d2, taskid.d3,
             parentid.d1, parentid.d2, parentid.d3);
    json_write("B", domain->nameA, name->strA, get_timestamp_us(), extra);
}

static void json_task_end(const __itt_domain *domain)
{
    if (domain == NULL) return;
    json_write("E", domain->nameA, "", get_timestamp_us(),
               ",\"args\":{\"api\":\"task\"}");
}

static void json_region_begin(
    const __itt_domain *domain, __itt_id id, __itt_string_handle *name)
{
    if (domain == NULL || name == NULL) return;
    char extra[LOG_BUFFER_MAX_SIZE];
    snprintf(extra, sizeof(extra),
             ",\"id\":\"0x%llx\",\"args\":{\"api\":\"region\"}", id.d1);
    json_write("b", domain->nameA, name->strA, get_timestamp_us(), extra);
}

static void json_region_end(const __itt_domain *domain, __itt_id id)
{
    if (domain == NULL) return;
    char extra[LOG_BUFFER_MAX_SIZE];
    snprintf(extra, sizeof(extra),
             ",\"id\":\"0x%llx\",\"args\":{\"api\":\"region\"}", id.d1);
    json_write("e", domain->nameA, "", get_timestamp_us(), extra);
}

static void json_metadata_add(const __itt_domain *domain,
    __itt_string_handle *key, __itt_metadata_type type, size_t count, void *data)
{
    if (domain == NULL || count == 0) return;
    char* metadata_str = get_metadata_elements(count, type, data);
    char key_esc[LOG_BUFFER_MAX_SIZE];
    char data_esc[LOG_BUFFER_MAX_SIZE];
    json_escape((key != NULL) ? key->strA : "", key_esc, sizeof(key_esc));
    json_escape(metadata_str, data_esc, sizeof(data_esc));

    char extra[LOG_BUFFER_MAX_SIZE * 2];
    snprintf(extra, sizeof(extra),
             ",\"s\":\"t\",\"args\":{\"api\":\"metadata\","
             "\"key\":\"%s\",\"count\":%zu,\"data\":\"%s\"}",
             key_esc, count, data_esc);
    json_write("i", domain->nameA, (key != NULL) ? key->strA : "metadata",
               get_timestamp_us(), extra);
    free(metadata_str);
}

static void json_formatted_metadata_add(const __itt_domain *domain, const char* formatted_metadata)
{
    if (domain == NULL) return;
    char data_esc[LOG_BUFFER_MAX_SIZE];
    json_escape(formatted_metadata, data_esc, sizeof(data_esc));

    char extra[LOG_BUFFER_MAX_SIZE * 2];
    snprintf(extra, sizeof(extra),
             ",\"s\":\"t\",\"args\":{\"api\":\"metadata\",\"data\":\"%s\"}",
             data_esc);
    json_write("i", domain->nameA, "__itt_formatted_metadata_add", get_timestamp_us(), extra);
}

static void json_histogram_submit(__itt_histogram* hist, size_t length, void* x_data, void* y_data)
{
    if (hist == NULL || hist->domain == NULL || hist->domain->nameA == NULL ||
        hist->nameA == NULL || length == 0 || y_data == NULL)
    {
        return;
    }

    char* y_str = get_metadata_elements(length, hist->y_type, y_data);
    char y_esc[LOG_BUFFER_MAX_SIZE];
    json_escape(y_str, y_esc, sizeof(y_esc));
    free(y_str);

    char extra[LOG_BUFFER_MAX_SIZE * 3];
    if (x_data != NULL)
    {
        char* x_str = get_metadata_elements(length, hist->x_type, x_data);
        char x_esc[LOG_BUFFER_MAX_SIZE];
        json_escape(x_str, x_esc, sizeof(x_esc));
        free(x_str);
        snprintf(extra, sizeof(extra),
                 ",\"s\":\"t\",\"args\":{\"api\":\"histogram\","
                 "\"length\":%zu,\"x\":\"%s\",\"y\":\"%s\"}",
                 length, x_esc, y_esc);
    }
    else
    {
        snprintf(extra, sizeof(extra),
                 ",\"s\":\"t\",\"args\":{\"api\":\"histogram\","
                 "\"length\":%zu,\"y\":\"%s\"}",
                 length, y_esc);
    }
    json_write("i", hist->domain->nameA, hist->nameA, get_timestamp_us(), extra);
}

static void json_bind_context_metadata_to_counter(__itt_counter counter, size_t length, __itt_context_metadata* metadata)
{
    if (counter == NULL || metadata == NULL || length == 0) return;
    __itt_counter_info_t* counter_info = (__itt_counter_info_t*)counter;
    char context_metadata[LOG_BUFFER_MAX_SIZE];
    build_context_metadata_str(context_metadata, sizeof(context_metadata), length, metadata);

    char ctx_esc[LOG_BUFFER_MAX_SIZE];
    json_escape(context_metadata, ctx_esc, sizeof(ctx_esc));

    char extra[LOG_BUFFER_MAX_SIZE * 2];
    snprintf(extra, sizeof(extra),
             ",\"s\":\"t\",\"args\":{\"api\":\"counter\","
             "\"length\":%zu,\"context\":\"%s\"}",
             length, ctx_esc);
    json_write("i", "counter", (counter_info->nameA != NULL) ? counter_info->nameA : "",
               get_timestamp_us(), extra);
}

static void json_counter_set_value_v3(__itt_counter counter, void* value_ptr)
{
    if (counter == NULL || value_ptr == NULL) return;
    __itt_counter_info_t* counter_info = (__itt_counter_info_t*)counter;
    uint64_t value = *(uint64_t*)value_ptr;
    const char* series = (counter_info->nameA != NULL) ? counter_info->nameA : "value";

    char series_esc[LOG_BUFFER_MAX_SIZE];
    json_escape(series, series_esc, sizeof(series_esc));

    char extra[LOG_BUFFER_MAX_SIZE * 2];
    snprintf(extra, sizeof(extra), ",\"args\":{\"%s\":%" PRIu64 "}", series_esc, value);
    json_write("C", "counter", series, get_timestamp_us(), extra);
}

// ----------------------------------------------------------------------------
// Mode 1 event handlers (plain-text log)
//
// One log_* handler per traced ITT API call, mirroring the json_* handlers
// above. Each writes a single human-readable line via the LOG_CALL_* macros
// (which take the explicit ITT API name). Argument validation lives here too.
// ----------------------------------------------------------------------------

static void log_pause(void)
{
    LOG_CALL_INFO("__itt_pause");
}

static void log_pause_scoped(__itt_collection_scope scope)
{
    LOG_CALL_INFO("__itt_pause_scoped", "scope=%d", scope);
}

static void log_resume(void)
{
    LOG_CALL_INFO("__itt_resume");
}

static void log_resume_scoped(__itt_collection_scope scope)
{
    LOG_CALL_INFO("__itt_resume_scoped", "scope=%d", scope);
}

static void log_detach(void)
{
    LOG_CALL_INFO("__itt_detach");
}

static void log_frame_begin_v3(const __itt_domain *domain, __itt_id *id)
{
    (void)id;
    if (domain == NULL) { LOG_CALL_WARN("__itt_frame_begin_v3", "Incorrect function call"); return; }
    LOG_CALL_INFO("__itt_frame_begin_v3", "domain=%s", domain->nameA);
}

static void log_frame_end_v3(const __itt_domain *domain, __itt_id *id)
{
    (void)id;
    if (domain == NULL) { LOG_CALL_WARN("__itt_frame_end_v3", "Incorrect function call"); return; }
    LOG_CALL_INFO("__itt_frame_end_v3", "domain=%s", domain->nameA);
}

static void log_frame_submit_v3(const __itt_domain *domain, __itt_timestamp begin, __itt_timestamp end)
{
    if (domain == NULL) { LOG_CALL_WARN("__itt_frame_submit_v3", "Incorrect function call"); return; }
    LOG_CALL_INFO("__itt_frame_submit_v3", "domain=%s, time_begin=%llu, time_end=%llu",
                  domain->nameA, begin, end);
}

static void log_task_begin(const __itt_domain *domain, __itt_id taskid, __itt_id parentid, __itt_string_handle *name)
{
    if (domain == NULL || name == NULL) { LOG_CALL_WARN("__itt_task_begin", "Incorrect function call"); return; }
    LOG_CALL_INFO("__itt_task_begin", "domain=%s name=%s taskid=%llu,%llu,%llu parentid=%llu,%llu,%llu",
                  domain->nameA, name->strA,
                  taskid.d1, taskid.d2, taskid.d3,
                  parentid.d1, parentid.d2, parentid.d3);
}

static void log_task_end(const __itt_domain *domain)
{
    if (domain == NULL) { LOG_CALL_WARN("__itt_task_end", "Incorrect function call"); return; }
    LOG_CALL_INFO("__itt_task_end", "domain=%s", domain->nameA);
}

static void log_region_begin(const __itt_domain *domain, __itt_id id, __itt_id parentid, __itt_string_handle *name)
{
    if (domain == NULL || name == NULL) { LOG_CALL_WARN("__itt_region_begin", "Incorrect function call"); return; }
    LOG_CALL_INFO("__itt_region_begin", "domain=%s name=%s id=%llu,%llu,%llu parentid=%llu,%llu,%llu",
                  domain->nameA, name->strA,
                  id.d1, id.d2, id.d3,
                  parentid.d1, parentid.d2, parentid.d3);
}

static void log_region_end(const __itt_domain *domain, __itt_id id)
{
    if (domain == NULL) { LOG_CALL_WARN("__itt_region_end", "Incorrect function call"); return; }
    LOG_CALL_INFO("__itt_region_end", "domain=%s id=%llu,%llu,%llu",
                  domain->nameA, id.d1, id.d2, id.d3);
}

static void log_metadata_add(const __itt_domain *domain, __itt_metadata_type type, size_t count, void *data)
{
    if (domain == NULL || count == 0) { LOG_CALL_WARN("__itt_metadata_add", "Incorrect function call"); return; }
    char* metadata_str = get_metadata_elements(count, type, data);
    LOG_CALL_INFO("__itt_metadata_add", "domain=%s metadata_size=%zu metadata[]=%s",
                  domain->nameA, count, metadata_str);
    free(metadata_str);
}

static void log_formatted_metadata_add(const __itt_domain *domain, const char* formatted_metadata)
{
    LOG_CALL_INFO("__itt_formatted_metadata_add", "domain=%s formatted_metadata=%s",
                  domain->nameA, formatted_metadata);
}

static void log_histogram_submit(__itt_histogram* hist, size_t length, void* x_data, void* y_data)
{
    if (hist == NULL) { LOG_CALL_WARN("__itt_histogram_submit", "Histogram is NULL"); return; }
    if (hist->domain == NULL) { LOG_CALL_WARN("__itt_histogram_submit", "Histogram domain is NULL"); return; }
    if (hist->domain->nameA == NULL || hist->nameA == NULL || length == 0 || y_data == NULL)
    {
        LOG_CALL_WARN("__itt_histogram_submit", "Incorrect function call");
        return;
    }
    char* y_str = get_metadata_elements(length, hist->y_type, y_data);
    if (x_data != NULL)
    {
        char* x_str = get_metadata_elements(length, hist->x_type, x_data);
        LOG_CALL_INFO("__itt_histogram_submit", "histogram_name=%s length=%zu x_data[]=%s y_data[]=%s",
                      hist->nameA, length, x_str, y_str);
        free(x_str);
    }
    else
    {
        LOG_CALL_INFO("__itt_histogram_submit", "histogram_name=%s length=%zu y_data[]=%s",
                      hist->nameA, length, y_str);
    }
    free(y_str);
}

static void log_bind_context_metadata_to_counter(__itt_counter counter, size_t length, __itt_context_metadata* metadata)
{
    if (counter == NULL || metadata == NULL || length == 0)
    {
        LOG_CALL_WARN("__itt_bind_context_metadata_to_counter", "Incorrect function call");
        return;
    }
    __itt_counter_info_t* counter_info = (__itt_counter_info_t*)counter;
    char context_metadata[LOG_BUFFER_MAX_SIZE];
    build_context_metadata_str(context_metadata, sizeof(context_metadata), length, metadata);
    LOG_CALL_INFO("__itt_bind_context_metadata_to_counter",
                  "counter_name=%s context_metadata_size=%zu context_metadata[]=%s",
                  (counter_info->nameA != NULL) ? counter_info->nameA : "", length, context_metadata);
}

static void log_counter_set_value_v3(__itt_counter counter, void* value_ptr)
{
    if (counter == NULL || value_ptr == NULL)
    {
        LOG_CALL_WARN("__itt_counter_set_value_v3", "Incorrect function call");
        return;
    }
    __itt_counter_info_t* counter_info = (__itt_counter_info_t*)counter;
    uint64_t value = *(uint64_t*)value_ptr;
    LOG_CALL_INFO("__itt_counter_set_value_v3", "counter_name=%s counter_value=%" PRIu64,
                  (counter_info->nameA != NULL) ? counter_info->nameA : "", value);
}

// ----------------------------------------------------------------------------
// ITT API entry points (traced events)
//
// Thin dispatchers: select the plain-text logger (mode 1) or the JSON writer
// (mode 2) based on g_gen_json. All validation and formatting lives in the
// matching log_* / json_* handlers above.
// ----------------------------------------------------------------------------

ITT_EXTERN_C void ITTAPI __itt_pause(void)
{
    if (!g_gen_json) log_pause();
    else             json_pause();
}

ITT_EXTERN_C void ITTAPI __itt_pause_scoped(__itt_collection_scope scope)
{
    if (!g_gen_json) log_pause_scoped(scope);
    else             json_pause_scoped(scope);
}

ITT_EXTERN_C void ITTAPI __itt_resume(void)
{
    if (!g_gen_json) log_resume();
    else             json_resume();
}

ITT_EXTERN_C void ITTAPI __itt_resume_scoped(__itt_collection_scope scope)
{
    if (!g_gen_json) log_resume_scoped(scope);
    else             json_resume_scoped(scope);
}

ITT_EXTERN_C void ITTAPI __itt_detach(void)
{
    if (!g_gen_json) log_detach();
    else             json_detach();
}

// __itt_thread_set_name has no plain-text log realization: the original
// reference collector never logged it. In JSON mode it labels the thread track.
#ifdef _WIN32
ITT_EXTERN_C void ITTAPI __itt_thread_set_nameW(const wchar_t *name)
{
    if (!g_gen_json) return;
    char* name_a = wchar2char(name);
    json_thread_set_name(name_a);
    free(name_a);
}
ITT_EXTERN_C void ITTAPI __itt_thread_set_nameA(const char *name)
#else
ITT_EXTERN_C void ITTAPI __itt_thread_set_name(const char *name)
#endif
{
    if (g_gen_json) json_thread_set_name(name);
}

ITT_EXTERN_C void ITTAPI __itt_frame_begin_v3(const __itt_domain *domain, __itt_id *id)
{
    if (!g_gen_json) log_frame_begin_v3(domain, id);
    else             json_frame_begin_v3(domain, id);
}

ITT_EXTERN_C void ITTAPI __itt_frame_end_v3(const __itt_domain *domain, __itt_id *id)
{
    if (!g_gen_json) log_frame_end_v3(domain, id);
    else             json_frame_end_v3(domain, id);
}

ITT_EXTERN_C void ITTAPI __itt_frame_submit_v3(const __itt_domain *domain, __itt_id *id,
    __itt_timestamp begin, __itt_timestamp end)
{
    (void)id;
    if (!g_gen_json) log_frame_submit_v3(domain, begin, end);
    else             json_frame_submit_v3(domain, begin, end);
}

ITT_EXTERN_C void ITTAPI __itt_task_begin(
    const __itt_domain *domain, __itt_id taskid, __itt_id parentid, __itt_string_handle *name)
{
    if (!g_gen_json) log_task_begin(domain, taskid, parentid, name);
    else             json_task_begin(domain, taskid, parentid, name);
}

ITT_EXTERN_C void ITTAPI __itt_task_end(const __itt_domain *domain)
{
    if (!g_gen_json) log_task_end(domain);
    else             json_task_end(domain);
}

ITT_EXTERN_C void ITTAPI __itt_region_begin(
    const __itt_domain *domain, __itt_id id, __itt_id parentid, __itt_string_handle *name)
{
    if (!g_gen_json) log_region_begin(domain, id, parentid, name);
    else             json_region_begin(domain, id, name);
}

ITT_EXTERN_C void ITTAPI __itt_region_end(const __itt_domain *domain, __itt_id id)
{
    if (!g_gen_json) log_region_end(domain, id);
    else             json_region_end(domain, id);
}

ITT_EXTERN_C void __itt_metadata_add(const __itt_domain *domain, __itt_id id,
    __itt_string_handle *key, __itt_metadata_type type, size_t count, void *data)
{
    (void)id;
    if (!g_gen_json) log_metadata_add(domain, type, count, data);
    else             json_metadata_add(domain, key, type, count, data);
}

ITT_EXTERN_C void __itt_formatted_metadata_add(const __itt_domain *domain, __itt_string_handle *format, ...)
{
    // Formatting the variadic args must happen here (in the entry point) before
    // dispatching, so validate up front too.
    if (domain == NULL || format == NULL)
    {
        if (!g_gen_json) LOG_CALL_WARN("__itt_formatted_metadata_add", "Incorrect function call");
        return;
    }

    va_list args;
    va_start(args, format);
    char formatted_metadata[LOG_BUFFER_MAX_SIZE];
    vsnprintf(formatted_metadata, LOG_BUFFER_MAX_SIZE, format->strA, args);
    va_end(args);

    if (!g_gen_json) log_formatted_metadata_add(domain, formatted_metadata);
    else             json_formatted_metadata_add(domain, formatted_metadata);
}

ITT_EXTERN_C void __itt_histogram_submit(__itt_histogram* hist, size_t length, void* x_data, void* y_data)
{
    if (!g_gen_json) log_histogram_submit(hist, length, x_data, y_data);
    else             json_histogram_submit(hist, length, x_data, y_data);
}

ITT_EXTERN_C void __itt_bind_context_metadata_to_counter(__itt_counter counter, size_t length, __itt_context_metadata* metadata)
{
    if (!g_gen_json) log_bind_context_metadata_to_counter(counter, length, metadata);
    else             json_bind_context_metadata_to_counter(counter, length, metadata);
}

ITT_EXTERN_C void __itt_counter_set_value_v3(__itt_counter counter, void* value_ptr)
{
    if (!g_gen_json) log_counter_set_value_v3(counter, value_ptr);
    else             json_counter_set_value_v3(counter, value_ptr);
}
