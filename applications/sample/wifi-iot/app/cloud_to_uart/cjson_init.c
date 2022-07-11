/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <cJSON.h>
#include <hi_mem.h>

static void *cJsonMalloc(size_t sz)
{
    return hi_malloc(0, sz);
}

static void cJsonFree(const char *p)
{
    hi_free(0, p);
}

void cJsonInit(void)
{
    cJSON_Hooks  hooks;
    hooks.malloc_fn = cJsonMalloc;
    hooks.free_fn = cJsonFree;
    cJSON_InitHooks(&hooks);

    return;
}