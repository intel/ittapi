/*
  Copyright (C) 2026 Intel Corporation

  SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
*/

#include "ittnotify.h"
#include "ittnotify_types.h"

int main(void)
{
    __itt_domain* domain = __itt_domain_create("smoke_test_domain");
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

    return 0;
}
