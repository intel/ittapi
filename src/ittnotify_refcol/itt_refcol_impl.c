/*
  Copyright (C) 2025 Intel Corporation

  SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INTEL_NO_MACRO_BODY
#define INTEL_ITTNOTIFY_API_PRIVATE
#include "ittnotify.h"
#include "ittnotify_config.h"

#define LOG_BUFFER_MAX_SIZE 256

static const char* env_log_dir = "INTEL_LIBITTNOTIFY_LOG_DIR";
static const char* log_level_str[] = {"INFO", "WARN", "ERROR", "FATAL_ERROR"};

enum {
    LOG_LVL_INFO,
    LOG_LVL_WARN,
    LOG_LVL_ERROR,
    LOG_LVL_FATAL
};

static struct ref_collector_logger {
    FILE* log_fp;
    uint8_t init_state;
} g_ref_collector_logger = {NULL, 0};

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

static char* log_file_name_generate()
{
    time_t time_now = time(NULL);
    struct tm* time_info = localtime(&time_now);
    char* log_file_name = malloc(sizeof(char) * (LOG_BUFFER_MAX_SIZE/2));

    sprintf(log_file_name,"libittnotify_refcol_%d%d%d%d%d%d.log",
            time_info->tm_year+1900, time_info->tm_mon+1, time_info->tm_mday,
            time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

    return log_file_name;
}

// This reference implementation opens a log file for recording ITT API calls.
// Custom collectors can replace this with their own initialization logic
// (e.g., opening trace files, connecting to profiler backends, allocating buffers).
static void ref_collector_init()
{
    if (!g_ref_collector_logger.init_state)
    {
        static char file_name_buffer[LOG_BUFFER_MAX_SIZE*2];
        char* log_dir = getenv(env_log_dir);
        char* log_file = log_file_name_generate();

        if (log_dir != NULL)
        {
            #ifdef _WIN32
                sprintf(file_name_buffer,"%s\\%s", log_dir, log_file);
            #else
                sprintf(file_name_buffer,"%s/%s", log_dir, log_file);
            #endif
        }
        else
        {
            #ifdef _WIN32
                char* temp_dir = getenv("TEMP");
                if (temp_dir != NULL)
                {
                    sprintf(file_name_buffer,"%s\\%s", temp_dir, log_file);
                }
            #else
                sprintf(file_name_buffer,"/tmp/%s", log_file);
            #endif
        }
        free(log_file);

        g_ref_collector_logger.log_fp = fopen(file_name_buffer, "a");
        if (!g_ref_collector_logger.log_fp)
        {
            printf("ERROR: Cannot open log file: %s\n", file_name_buffer);
            return;
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
static void ref_collector_release()
{
    static int released = 0;
    if (released) return;
    released = 1;

    if (g_ref_collector_logger.log_fp)
    {
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

    result_len += sprintf(log_buffer, "[%s] %s(...) - ", log_level_str[log_level] ,function_name);
    va_start(message_args, message_format);
    vsnprintf(log_buffer + result_len, LOG_BUFFER_MAX_SIZE - result_len, message_format, message_args);
    va_end(message_args);

    fprintf(g_ref_collector_logger.log_fp, "%s\n", log_buffer);
}

#define LOG_FUNC_CALL_INFO(...)  log_func_call(LOG_LVL_INFO, __FUNCTION__, __VA_ARGS__)
#define LOG_FUNC_CALL_WARN(...)  log_func_call(LOG_LVL_WARN, __FUNCTION__, __VA_ARGS__)
#define LOG_FUNC_CALL_ERROR(...) log_func_call(LOG_LVL_ERROR, __FUNCTION__, __VA_ARGS__)
#define LOG_FUNC_CALL_FATAL(...) log_func_call(LOG_LVL_FATAL, __FUNCTION__, __VA_ARGS__)

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
    *metadata_str = '\0';
    uint16_t offset = 0;

    switch (type)
    {
    case __itt_metadata_u64:
        for (uint16_t i = 0; i < size; i++)
            offset += sprintf(metadata_str + offset, "%lu;", ((uint64_t*)metadata)[i]);
        break;
    case __itt_metadata_s64:
        for (uint16_t i = 0; i < size; i++)
            offset += sprintf(metadata_str + offset, "%ld;", ((int64_t*)metadata)[i]);
        break;
    case __itt_metadata_u32:
        for (uint16_t i = 0; i < size; i++)
            offset += sprintf(metadata_str + offset, "%u;", ((uint32_t*)metadata)[i]);
        break;
    case __itt_metadata_s32:
        for (uint16_t i = 0; i < size; i++)
            offset += sprintf(metadata_str + offset, "%d;", ((int32_t*)metadata)[i]);
        break;
    case __itt_metadata_u16:
        for (uint16_t i = 0; i < size; i++)
            offset += sprintf(metadata_str + offset, "%u;", ((uint16_t*)metadata)[i]);
        break;
    case __itt_metadata_s16:
        for (uint16_t i = 0; i < size; i++)
            offset += sprintf(metadata_str + offset, "%d;", ((int16_t*)metadata)[i]);
        break;
    case __itt_metadata_float:
        for (uint16_t i = 0; i < size; i++)
            offset += sprintf(metadata_str + offset, "%f;", ((float*)metadata)[i]);
        break;
    case __itt_metadata_double:
        for (uint16_t i = 0; i < size; i++)
            offset += sprintf(metadata_str + offset, "%lf;", ((double*)metadata)[i]);
        break;
    default:
        printf("ERROR: Unknown metadata type\n");
        break;
    }

    return metadata_str;
}

// Remember to call free() after using get_context_metadata_element()
static char* get_context_metadata_element(__itt_context_type type, void* metadata)
{
    char* metadata_str = malloc(sizeof(char) * LOG_BUFFER_MAX_SIZE/4);
    *metadata_str = '\0';

    switch(type)
    {
        case __itt_context_name:
        case __itt_context_device:
        case __itt_context_units:
        case __itt_context_pci_addr:
            sprintf(metadata_str, "%s;", ((char*)metadata));
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
            sprintf(metadata_str, "%lu;", *(uint64_t*)metadata);
            break;
        default:
            printf("ERROR: Unknown context metadata type\n");
            break;
    }

    return metadata_str;
}

ITT_EXTERN_C __itt_domain* ITTAPI __itt_domain_create(const char *name)
{
    if (!g_ref_collector_global.mutex_initialized || name == NULL)
    {
        LOG_FUNC_CALL_WARN("Cannot create domain object");
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
        LOG_FUNC_CALL_INFO("function args: name=%s (created new domain)", name);
    }
    else
    {
        LOG_FUNC_CALL_INFO("function args: name=%s (domain already exists)", name);
    }

    __itt_mutex_unlock(&g_ref_collector_global.mutex);

    return h;
}

ITT_EXTERN_C __itt_string_handle* ITTAPI __itt_string_handle_create(const char* name)
{
    if (!g_ref_collector_global.mutex_initialized || name == NULL)
    {
        LOG_FUNC_CALL_WARN("Cannot create string handle object");
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
        LOG_FUNC_CALL_INFO("function args: name=%s (created new string handle)", name);
    }
    else
    {
        LOG_FUNC_CALL_INFO("function args: name=%s (string handle already exists)", name);
    }

    __itt_mutex_unlock(&g_ref_collector_global.mutex);

    return h;
}

ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_create(const char *name, const char *domain)
{
    LOG_FUNC_CALL_INFO("function call");
    return __itt_counter_create_typed(name, domain, __itt_metadata_u64);
}

ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_create_v3(
    const __itt_domain* domain, const char* name, __itt_metadata_type type)
{
    LOG_FUNC_CALL_INFO("function call");
    return __itt_counter_create_typed(name, domain->nameA, type);
}

ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_create_typed(
    const char *name, const char *domain, __itt_metadata_type type)
{
    if (!g_ref_collector_global.mutex_initialized || name == NULL || domain == NULL)
    {
        LOG_FUNC_CALL_WARN("Cannot create counter object");
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
        LOG_FUNC_CALL_INFO("function args: name=%s, domain=%s, type=%d (created new counter)",
                            name, domain, (int)type);
    }
    else
    {
        LOG_FUNC_CALL_INFO("function args: name=%s, domain=%s, type=%d (counter already exists)",
                            name, domain, (int)type);
    }

    __itt_mutex_unlock(&g_ref_collector_global.mutex);

    return (__itt_counter)h;
}

ITT_EXTERN_C __itt_histogram* ITTAPI __itt_histogram_create(
    const __itt_domain* domain, const char* name, __itt_metadata_type x_type, __itt_metadata_type y_type)
{
    if (!g_ref_collector_global.mutex_initialized || name == NULL || domain == NULL)
    {
        LOG_FUNC_CALL_WARN("Cannot create histogram object");
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
        LOG_FUNC_CALL_INFO("function args: domain=%s, name=%s, x_type=%d, y_type=%d (created new histogram)",
            domain->nameA, name, x_type, y_type);
    }
    else
    {
        LOG_FUNC_CALL_INFO("function args: domain=%s, name=%s, x_type=%d, y_type=%d (histogram already exists)",
            domain->nameA, name, x_type, y_type);
    }

    __itt_mutex_unlock(&g_ref_collector_global.mutex);

    return h;
}

ITT_EXTERN_C void ITTAPI __itt_pause(void)
{
    LOG_FUNC_CALL_INFO("function call");
}

ITT_EXTERN_C void ITTAPI __itt_pause_scoped(__itt_collection_scope scope)
{
    LOG_FUNC_CALL_INFO("function args: scope=%d", scope);
}

ITT_EXTERN_C void ITTAPI __itt_resume(void)
{
    LOG_FUNC_CALL_INFO("function call");
}

ITT_EXTERN_C void ITTAPI __itt_resume_scoped(__itt_collection_scope scope)
{
    LOG_FUNC_CALL_INFO("function args: scope=%d", scope);
}

ITT_EXTERN_C void ITTAPI __itt_detach(void)
{
    LOG_FUNC_CALL_INFO("function call");
}

ITT_EXTERN_C void ITTAPI __itt_frame_begin_v3(const __itt_domain *domain, __itt_id *id)
{
    if (domain != NULL)
    {
        (void)id;
        LOG_FUNC_CALL_INFO("function args: domain=%s", domain->nameA);
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}

ITT_EXTERN_C void ITTAPI __itt_frame_end_v3(const __itt_domain *domain, __itt_id *id)
{
    if (domain != NULL)
    {
        (void)id;
        LOG_FUNC_CALL_INFO("function args: domain=%s", domain->nameA);
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}

ITT_EXTERN_C void ITTAPI __itt_frame_submit_v3(const __itt_domain *domain, __itt_id *id,
    __itt_timestamp begin, __itt_timestamp end)
{
    if (domain != NULL)
    {
        (void)id;
        LOG_FUNC_CALL_INFO("function args: domain=%s, time_begin=%llu, time_end=%llu",
                        domain->nameA, begin, end);
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}

ITT_EXTERN_C void ITTAPI __itt_task_begin(
    const __itt_domain *domain, __itt_id taskid, __itt_id parentid, __itt_string_handle *name)
{
    if (domain != NULL && name != NULL)
    {
        LOG_FUNC_CALL_INFO("function args: domain=%s name=%s taskid=%llu,%llu,%llu parentid=%llu,%llu,%llu",
                            domain->nameA, name->strA,
                            taskid.d1, taskid.d2, taskid.d3,
                            parentid.d1, parentid.d2, parentid.d3);
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}

ITT_EXTERN_C void ITTAPI __itt_task_end(const __itt_domain *domain)
{
    if (domain != NULL)
    {
        LOG_FUNC_CALL_INFO("function args: domain=%s", domain->nameA);
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}

ITT_EXTERN_C void ITTAPI __itt_region_begin(
    const __itt_domain *domain, __itt_id id, __itt_id parentid, __itt_string_handle *name)
{
    if (domain != NULL && name != NULL)
    {
        LOG_FUNC_CALL_INFO("function args: domain=%s name=%s id=%llu,%llu,%llu parentid=%llu,%llu,%llu",
                            domain->nameA, name->strA,
                            id.d1, id.d2, id.d3,
                            parentid.d1, parentid.d2, parentid.d3);
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}

ITT_EXTERN_C void ITTAPI __itt_region_end(const __itt_domain *domain, __itt_id id)
{
    if (domain != NULL)
    {
        LOG_FUNC_CALL_INFO("function args: domain=%s id=%llu,%llu,%llu",
                            domain->nameA, id.d1, id.d2, id.d3);
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}

ITT_EXTERN_C void __itt_metadata_add(const __itt_domain *domain, __itt_id id,
    __itt_string_handle *key, __itt_metadata_type type, size_t count, void *data)
{
    if (domain != NULL && count != 0)
    {
        (void)id;
        (void)key;
        char* metadata_str = get_metadata_elements(count, type, data);
        LOG_FUNC_CALL_INFO("function args: domain=%s metadata_size=%lu metadata[]=%s",
                            domain->nameA, count, metadata_str);
        free(metadata_str);
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}

ITT_EXTERN_C void __itt_formatted_metadata_add(const __itt_domain *domain, __itt_string_handle *format, ...)
{
    if (domain == NULL || format == NULL)
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
        return;
    }

    va_list args;
    va_start(args, format);

    char formatted_metadata[LOG_BUFFER_MAX_SIZE];
    vsnprintf(formatted_metadata, LOG_BUFFER_MAX_SIZE, format->strA, args);

    LOG_FUNC_CALL_INFO("function args: domain=%s formatted_metadata=%s",
                        domain->nameA, formatted_metadata);

    va_end(args);
}

ITT_EXTERN_C void __itt_histogram_submit(__itt_histogram* hist, size_t length, void* x_data, void* y_data)
{
    if (hist == NULL)
    {
        LOG_FUNC_CALL_WARN("Histogram is NULL");
    }
    else if (hist->domain == NULL)
    {
        LOG_FUNC_CALL_WARN("Histogram domain is NULL");
    }
    else if (hist->domain->nameA != NULL && hist->nameA != NULL && length != 0 && y_data != NULL)
    {
        if (x_data != NULL)
        {
            char* x_data_str = get_metadata_elements(length, hist->x_type, x_data);
            char* y_data_str = get_metadata_elements(length, hist->y_type, y_data);
            LOG_FUNC_CALL_INFO("function args: domain=%s name=%s histogram_size=%lu x[]=%s y[]=%s",
                                hist->domain->nameA, hist->nameA, length, x_data_str, y_data_str);
            free(x_data_str);
            free(y_data_str);
        }
        else
        {
            char* y_data_str = get_metadata_elements(length, hist->y_type, y_data);
            LOG_FUNC_CALL_INFO("function args: domain=%s name=%s histogram_size=%lu y[]=%s",
                                hist->domain->nameA, hist->nameA, length, y_data_str);
            free(y_data_str);
        }
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}

ITT_EXTERN_C void __itt_bind_context_metadata_to_counter(__itt_counter counter, size_t length, __itt_context_metadata* metadata)
{
    if (counter != NULL && metadata != NULL && length != 0)
    {
        __itt_counter_info_t* counter_info = (__itt_counter_info_t*)counter;
        char context_metadata[LOG_BUFFER_MAX_SIZE];
        context_metadata[0] = '\0';
        uint16_t offset = 0;
        for(size_t i=0; i<length; i++)
        {
            char* context_metadata_element = get_context_metadata_element(metadata[i].type, metadata[i].value);
            offset += sprintf(context_metadata+ offset, "%s", context_metadata_element);
            free(context_metadata_element);
        }
        LOG_FUNC_CALL_INFO("function args: counter_name=%s context_metadata_size=%lu context_metadata[]=%s",
                            counter_info->nameA, length, context_metadata);
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}

ITT_EXTERN_C void __itt_counter_set_value_v3(__itt_counter counter, void* value_ptr)
{
    if (counter != NULL && value_ptr != NULL)
    {
        __itt_counter_info_t* counter_info = (__itt_counter_info_t*)counter;
        uint64_t value = *(uint64_t*)value_ptr;
        LOG_FUNC_CALL_INFO("function args: counter_name=%s counter_value=%lu",
                            counter_info->nameA, value);
    }
    else
    {
        LOG_FUNC_CALL_WARN("Incorrect function call");
    }
}
