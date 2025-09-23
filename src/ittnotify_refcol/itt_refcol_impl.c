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

static __itt_global* g_itt_global = NULL;

char* log_file_name_generate()
{
    time_t time_now = time(NULL);
    struct tm* time_info = localtime(&time_now);
    char* log_file_name = malloc(sizeof(char) * (LOG_BUFFER_MAX_SIZE/2));

    sprintf(log_file_name,"libittnotify_refcol_%d%d%d%d%d%d.log",
            time_info->tm_year+1900, time_info->tm_mon+1, time_info->tm_mday,
            time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

    return log_file_name;
}

void ref_col_init()
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

void ref_col_release()
{
    printf("Releasing ITT API reference collector\n");
    if (g_ref_collector_logger.log_fp)
    {
        fclose(g_ref_collector_logger.log_fp);
        g_ref_collector_logger.log_fp = NULL;
    }

    if (g_itt_global == NULL) return;

    __itt_mutex_lock(&(g_itt_global->mutex));

    __itt_domain *d = g_itt_global->domain_list;
    while (d)
    {
        __itt_domain *next = d->next;
        if (d->nameA) free((char*)d->nameA);
        free(d);
        d = next;
    }
    g_itt_global->domain_list = NULL;

    __itt_string_handle *s = g_itt_global->string_list;
    while (s)
    {
        __itt_string_handle *next = s->next;
        if (s->strA) free((char*)s->strA);
        free(s);
        s = next;
    }
    g_itt_global->string_list = NULL;

    __itt_counter_info_t *c = g_itt_global->counter_list;
    while (c)
    {
        __itt_counter_info_t *next = c->next;
        if (c->nameA) free((char*)c->nameA);
        if (c->domainA) free((char*)c->domainA);
        free(c);
        c = next;
    }
    g_itt_global->counter_list = NULL;

    __itt_histogram *h = g_itt_global->histogram_list;
    while (h)
    {
        __itt_histogram *next = h->next;
        if (h->nameA) free((char*)h->nameA);
        free(h);
        h = next;
    }
    g_itt_global->histogram_list = NULL;

    __itt_mutex_unlock(&(g_itt_global->mutex));
}

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

ITT_EXTERN_C void ITTAPI __itt_api_init(__itt_global* p, __itt_group_id init_groups)
{
    if (p != NULL)
    {
        (void)init_groups;
        fill_func_ptr_per_lib(p);
        ref_col_init();
        g_itt_global = p;
        atexit(ref_col_release);
    }
    else
    {
        printf("ERROR: Failed to initialize dynamic library\n");
    }
}

void log_func_call(uint8_t log_level, const char* function_name, const char* message_format, ...)
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
// The code below is a reference implementation of the
// Instrumentation and Tracing Technology API (ITT API) dynamic collector.
// This implementation is designed to log ITT API functions calls.
// ----------------------------------------------------------------------------

// Remember to call free() after using get_metadata_elements()
char* get_metadata_elements(size_t size, __itt_metadata_type type, void* metadata)
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
        printf("ERROR: Unknow metadata type\n");
        break;
    }

    return metadata_str;
}

// Remember to call free() after using get_context_metadata_element()
char* get_context_metadata_element(__itt_context_type type, void* metadata)
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
            printf("ERROR: Unknown context metadata type");
            break;
    }

    return metadata_str;
}

ITT_EXTERN_C __itt_domain* ITTAPI __itt_domain_create(const char *name)
{
    if (g_itt_global == NULL || name == NULL)
    {
        LOG_FUNC_CALL_WARN("Cannot create domain object");
        return NULL;
    }

    __itt_domain *h_tail = NULL, *h = NULL;

    __itt_mutex_lock(&(g_itt_global->mutex));

    for (h_tail = NULL, h = g_itt_global->domain_list; h != NULL; h_tail = h, h = h->next)
    {
        if (h->nameA != NULL && !strcmp(h->nameA, name)) break;
    }

    if (h == NULL)
    {
        NEW_DOMAIN_A(g_itt_global, h, h_tail, name);
        LOG_FUNC_CALL_INFO("function args: name=%s (created new domain)", name);
    }
    else
    {
        LOG_FUNC_CALL_INFO("function args: name=%s (domain already exists)", name);
    }

    __itt_mutex_unlock(&(g_itt_global->mutex));

    return h;
}

ITT_EXTERN_C __itt_string_handle* ITTAPI __itt_string_handle_create(const char* name)
{
    if (g_itt_global == NULL || name == NULL)
    {
        LOG_FUNC_CALL_WARN("Cannot create string handle object");
        return NULL;
    }

    __itt_string_handle *h_tail = NULL, *h = NULL;

    __itt_mutex_lock(&(g_itt_global->mutex));

    for (h_tail = NULL, h = g_itt_global->string_list; h != NULL; h_tail = h, h = h->next)
    {
        if (h->strA != NULL && !strcmp(h->strA, name)) break;
    }

    if (h == NULL)
    {
        NEW_STRING_HANDLE_A(g_itt_global, h, h_tail, name);
        LOG_FUNC_CALL_INFO("function args: name=%s (created new string handle)", name);
    }
    else
    {
        LOG_FUNC_CALL_INFO("function args: name=%s (string handle already exists)", name);
    }

    __itt_mutex_unlock(&(g_itt_global->mutex));

    return h;
}

ITT_EXTERN_C __itt_counter ITTAPI __itt_counter_create_v3(
    const __itt_domain* domain, const char* name, __itt_metadata_type type)
{
    if (g_itt_global == NULL || name == NULL || domain == NULL)
    {
        LOG_FUNC_CALL_WARN("Cannot create counter object");
        return NULL;
    }

    __itt_counter_info_t *h_tail = NULL, *h = NULL;

    __itt_mutex_lock(&(g_itt_global->mutex));

    for (h_tail = NULL, h = g_itt_global->counter_list; h != NULL; h_tail = h, h = h->next)
    {
        if (h->nameA && h->type == (int)type && !strcmp(h->nameA, name) &&
            ((h->domainA == NULL && domain->nameA == NULL) ||
            (h->domainA && domain->nameA && !strcmp(h->domainA, domain->nameA))))
            break;
    }

    if (h == NULL)
    {
        NEW_COUNTER_A(g_itt_global, h, h_tail, name, domain->nameA, type);
        LOG_FUNC_CALL_INFO("function args: name=%s, domain=%s, type=%d (created new counter)",
                            name, domain->nameA, (int)type);
    }
    else
    {
        LOG_FUNC_CALL_INFO("function args: name=%s, domain=%s, type=%d (counter already exists)",
                            name, domain->nameA, (int)type);
    }

    __itt_mutex_unlock(&(g_itt_global->mutex));

    return (__itt_counter)h;
}

ITT_EXTERN_C __itt_histogram* ITTAPI __itt_histogram_create(
    const __itt_domain* domain, const char* name, __itt_metadata_type x_type, __itt_metadata_type y_type)
{
    if (g_itt_global == NULL || name == NULL || domain == NULL)
    {
        LOG_FUNC_CALL_WARN("Cannot create histogram object");
        return NULL;
    }

    __itt_histogram *h_tail = NULL, *h = NULL;

    __itt_mutex_lock(&(g_itt_global->mutex));

    for (h_tail = NULL, h = g_itt_global->histogram_list; h != NULL; h_tail = h, h = h->next)
    {
        if (h->domain == domain && h->nameA && !strcmp(h->nameA, name)) break;
    }

    if (h == NULL)
    {
        NEW_HISTOGRAM_A(g_itt_global, h, h_tail, domain, name, x_type, y_type);
        LOG_FUNC_CALL_INFO("function args: domain=%s, name=%s, x_type=%d, y_type=%d (created new histogram)",
            domain->nameA, name, x_type, y_type);
    }
    else
    {
        LOG_FUNC_CALL_INFO("function args: domain=%s, name=%s, x_type=%d, y_type=%d (histogram already exists)",
            domain->nameA, name, x_type, y_type);
    }

    __itt_mutex_unlock(&(g_itt_global->mutex));

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
        (void)taskid;
        (void)parentid;
        LOG_FUNC_CALL_INFO("function args: domain=%s handle=%s", domain->nameA, name->strA);
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
