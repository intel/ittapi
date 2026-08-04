/*
  Copyright (C) 2026 Intel Corporation

  SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
*/

#include "ittnotify.h"
#include "ittnotify_types.h"

/* Scenario 1: nested tasks carrying explicit ids, plain metadata and
   formatted metadata, driven by a matrix multiply. Exercises
   __itt_task_begin / __itt_task_end (nested, with populated task/parent ids so
   the collector emits flow events), __itt_metadata_add and
   __itt_formatted_metadata_add. */
static void scenario_tasks_with_metadata(__itt_domain* domain)
{
    __itt_string_handle* handle_outer = __itt_string_handle_create("smoke_test_outer");
    __itt_string_handle* handle_inner = __itt_string_handle_create("smoke_test_inner");
    __itt_string_handle* handle_meta = __itt_string_handle_create("smoke_test_metadata");
    __itt_string_handle* handle_fmt = __itt_string_handle_create("iter=%d sum=%d");

    const int n = 10;
    int a[10][10], b[10][10], mul[10][10], i, j, k, count = 0;

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
        {
            a[i][j] = i;
            b[i][j] = j;
            mul[i][j] = 0;
        }

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            for (k = 0; k < n; k++)
            {
                mul[i][j] += a[i][k] * b[k][j];
                count++;
                if (count % 100 == 0)
                {
                    /* Outer task with an explicit id (no parent). */
                    __itt_id outer_id = __itt_id_make(domain, (unsigned long long)count);
                    __itt_task_begin(domain, outer_id, __itt_null, handle_outer);

                    unsigned long long data[5] = { i, j, k, count, mul[i][j] };
                    __itt_metadata_add(domain, __itt_null, handle_meta,
                                       __itt_metadata_u64, 5, data);

                    /* Inner task whose parent is the outer task id, so the
                       collector emits both a flow start and a flow finish. */
                    __itt_id inner_id = __itt_id_make(domain, (unsigned long long)(count + 1));
                    __itt_task_begin(domain, inner_id, outer_id, handle_inner);
                    __itt_formatted_metadata_add(domain, handle_fmt, i, mul[i][j]);
                    __itt_task_end(domain);

                    __itt_task_end(domain);
                }
            }
}

/* Scenario 2: frames enclosing regions, with a counter updated per frame and
   an explicit frame submit. Exercises __itt_frame_begin_v3 /
   __itt_frame_end_v3 / __itt_frame_submit_v3, __itt_region_begin /
   __itt_region_end and __itt_counter_create_v3 / __itt_counter_set_value_v3. */
static void scenario_frames_regions_counter(__itt_domain* domain)
{
    __itt_string_handle* handle_region = __itt_string_handle_create("smoke_test_region");
    __itt_counter counter = __itt_counter_create_v3(domain, "smoke_test_counter", __itt_metadata_u64);

    const int frames = 4;
    int f;

    for (f = 0; f < frames; f++)
    {
        __itt_id frame_id = __itt_id_make(domain, (unsigned long long)f);
        __itt_frame_begin_v3(domain, &frame_id);

        __itt_id region_id = __itt_id_make(domain, (unsigned long long)(frames + f));
        __itt_region_begin(domain, region_id, __itt_null, handle_region);

        unsigned long long value = (unsigned long long)(f + 1);
        __itt_counter_set_value_v3(counter, &value);

        __itt_region_end(domain, region_id);
        __itt_frame_end_v3(domain, &frame_id);

        /* Report the same frame as a complete slice with an explicit span. */
        __itt_frame_submit_v3(domain, &frame_id,
                              (__itt_timestamp)(f * 1000),
                              (__itt_timestamp)(f * 1000 + 500));
    }
}

/* Scenario 3: overlapped (asynchronous) tasks that intentionally do not nest.
   taskA begins, taskB begins, then taskA ends before taskB - a non-LIFO order
   that only the overlapped API can represent. Exercises
   __itt_task_begin_overlapped / __itt_task_end_overlapped. */
static void scenario_overlapped_tasks(__itt_domain* domain)
{
    __itt_string_handle* handle_a = __itt_string_handle_create("smoke_test_overlapped_a");
    __itt_string_handle* handle_b = __itt_string_handle_create("smoke_test_overlapped_b");

    int anchor = 0;
    const int rounds = 3;
    int r;

    for (r = 0; r < rounds; r++)
    {
        __itt_id id_a = __itt_id_make(&anchor, (unsigned long long)(2 * r + 1));
        __itt_id id_b = __itt_id_make(&anchor, (unsigned long long)(2 * r + 2));

        __itt_task_begin_overlapped(domain, id_a, __itt_null, handle_a);
        __itt_task_begin_overlapped(domain, id_b, __itt_null, handle_b);
        /* End the first task before the second - the defining overlap case. */
        __itt_task_end_overlapped(domain, id_a);
        __itt_task_end_overlapped(domain, id_b);
    }
}

/* Scenario 4: collection controls and a histogram submit. Exercises
   __itt_thread_set_name, __itt_histogram_create / __itt_histogram_submit and
   __itt_pause / __itt_resume (the task issued while paused must be suppressed
   in the JSON trace). */
static void scenario_controls_and_histogram(__itt_domain* domain)
{
    __itt_string_handle* handle_ignored = __itt_string_handle_create("smoke_test_paused");

    __itt_thread_set_name("smoke_test_main");

    __itt_histogram* hist = __itt_histogram_create(domain, "smoke_test_histogram",
                                                   __itt_metadata_u64, __itt_metadata_u64);
    unsigned long long x_data[4] = { 0, 1, 2, 3 };
    unsigned long long y_data[4] = { 10, 20, 30, 40 };
    __itt_histogram_submit(hist, 4, x_data, y_data);

    /* Everything between pause and resume must not appear in the JSON trace. */
    __itt_pause();
    __itt_task_begin(domain, __itt_null, __itt_null, handle_ignored);
    __itt_task_end(domain);
    __itt_resume();
}

int main(void)
{
    __itt_domain* domain = __itt_domain_create("smoke_test_domain");

    scenario_tasks_with_metadata(domain);
    scenario_frames_regions_counter(domain);
    scenario_overlapped_tasks(domain);
    scenario_controls_and_histogram(domain);

    return 0;
}
