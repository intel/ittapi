/*
  Copyright (C) 2026 Intel Corporation

  SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
*/

#include "ittnotify.h"
#include "ittnotify_types.h"

/* Scenario 1: nested tasks with metadata attached, driven by a matrix
   multiply. Exercises __itt_task_begin / __itt_metadata_add / __itt_task_end. */
static void scenario_tasks_with_metadata(__itt_domain* domain)
{
    __itt_string_handle* handle = __itt_string_handle_create("smoke_test_handler");
    __itt_string_handle* handle_work = __itt_string_handle_create("smoke_test_worker");

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
                    __itt_task_begin(domain, __itt_null, __itt_null, handle_work);
                    unsigned long long data[5] = { i, j, k, count, mul[i][j] };
                    __itt_metadata_add(domain, __itt_null, handle, __itt_metadata_u64, 5, data);
                    __itt_task_end(domain);
                }
            }
}

/* Scenario 2: frames enclosing regions, with a counter updated per frame.
   Exercises __itt_frame_begin_v3 / __itt_frame_end_v3, __itt_region_begin /
   __itt_region_end and __itt_counter_create_v3 / __itt_counter_set_value_v3. */
static void scenario_frames_regions_counter(__itt_domain* domain)
{
    __itt_string_handle* handle_region = __itt_string_handle_create("smoke_test_region");
    __itt_counter counter = __itt_counter_create_v3(domain, "smoke_test_counter", __itt_metadata_u64);

    const int frames = 4;
    int f;

    for (f = 0; f < frames; f++)
    {
        __itt_id frame_id = __itt_id_make(domain, f);
        __itt_frame_begin_v3(domain, &frame_id);

        __itt_id region_id = __itt_id_make(domain, frames + f);
        __itt_region_begin(domain, region_id, __itt_null, handle_region);

        unsigned long long value = (unsigned long long)(f + 1);
        __itt_counter_set_value_v3(counter, &value);

        __itt_region_end(domain, region_id);
        __itt_frame_end_v3(domain, &frame_id);
    }
}

int main(void)
{
    __itt_domain* domain = __itt_domain_create("smoke_test_domain");

    scenario_tasks_with_metadata(domain);
    scenario_frames_regions_counter(domain);

    return 0;
}
