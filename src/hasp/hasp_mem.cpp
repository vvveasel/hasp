
/* MIT License - Copyright (c) 2019-2022 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#include <stdlib.h>
#include "hasplib.h"
#include "hasp_mem.h"


void* hasp_calloc(size_t num, size_t size)
{
    return calloc(num, size);
}

void* hasp_malloc(size_t size)
{
    return malloc(size);
}

/* NOTE: when realloc returns NULL, it leaves the original memory untouched */
void* hasp_realloc(void* ptr, size_t new_size)
{
    return realloc(ptr, new_size);
}

void hasp_free(void* ptr)
{
    free(ptr);
}

#ifdef LODEPNG_NO_COMPILE_ALLOCATORS
void* lodepng_malloc(size_t size)
{
#ifdef LODEPNG_MAX_ALLOC
    if(size > LODEPNG_MAX_ALLOC) return 0;
#endif

    return hasp_malloc(size);
}

/* NOTE: when realloc returns NULL, it leaves the original memory untouched */
void* lodepng_realloc(void* ptr, size_t new_size)
{
#ifdef LODEPNG_MAX_ALLOC
    if(new_size > LODEPNG_MAX_ALLOC) return 0;
#endif

    return hasp_realloc(ptr, new_size);
}

void lodepng_free(void* ptr)
{
    hasp_free(ptr);
}
#endif // LODEPNG_NO_COMPILE_ALLOCATORS