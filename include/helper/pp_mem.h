/*************************************************************************
	> File Name: pp_mem.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Sat Jun  6 13:44:52 2026
	> Description: High-reliability industrial memory manager with header guards.
 ************************************************************************/

#ifndef PP_MEM_H
#define PP_MEM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PP_MEM_INFO
#include <stdio.h>
#define PP_MEM_INFO(fmt,...)  fprintf(stdout, "[MEM INFO]  %s %d: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#define PP_MEM_ERROR(fmt,...) fprintf(stderr, "[MEM ERROR] %s %d: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#endif // PP_MEM_INFO

/*oOoOoOoOoOoOoOoOoOoO
 * GLOBAL PROTOTYPES
 *oOoOoOoOoOoOoOoOoOoO*/

void * pp_malloc(size_t size);
void * pp_malloc_zeroed(size_t size);
void * pp_calloc(size_t num, size_t size);
void * pp_zalloc(size_t num, size_t size);
void   pp_free(void * data);
void * pp_realloc(void * data_p, size_t new_size);

#ifdef __cplusplus
}
#endif

#endif // PP_MEM_H


/*oOoOoOoOoOoOoOoOoOoO
 * IMPLEMENTATION PART
 *oOoOoOoOoOoOoOoOoOoO*/
#if defined(PP_MEM_IMPLEMENTATION) && !defined(PP_MEM_IMPLEMENTATION_DOWN)
#define PP_MEM_IMPLEMENTATION_DOWN

#include <stdlib.h>
#include <string.h>

/* Data alignment adjustment for ARM/MIPS hardware architectures */
#define PP_MEM_ALIGN_SIZE (sizeof(void *))
#define PP_MEM_ALIGN(size) (((size) + (PP_MEM_ALIGN_SIZE - 1)) & ~(PP_MEM_ALIGN_SIZE - 1))

/* Memory block header for tracking and overflow detection */
typedef struct {
    size_t size;        
    uint32_t magic;     
} pp_mem_header_t;

#define PP_MEM_MAGIC 0xDEADC0DE

/* Explicitly align the header size to guarantee that 
 * the user payload pointer (data_ptr) always remains perfectly aligned.
 */
#define PP_MEM_HEADER_SIZE PP_MEM_ALIGN(sizeof(pp_mem_header_t))

/**
 * @brief Basic memory allocation
 */
void * pp_malloc(size_t size)
{
    if (size == 0) return NULL;

    size_t aligned_size = PP_MEM_ALIGN(size);
    size_t total_size = aligned_size + PP_MEM_HEADER_SIZE;
    
    pp_mem_header_t * p = (pp_mem_header_t *)malloc(total_size);
    if (p == NULL) {
        PP_MEM_ERROR("OOM! Failed to allocate %zu bytes", size);
        return NULL;
    }

    p->size = aligned_size;
    p->magic = PP_MEM_MAGIC;

    /* Offset by fully-aligned header size to secure application layer structure */
    void * data_ptr = (void *)((uint8_t *)p + PP_MEM_HEADER_SIZE);
    return data_ptr;
}

/**
 * @brief Allocation with clearing payload to zero
 */
void * pp_malloc_zeroed(size_t size)
{
    void * data = pp_malloc(size);
    if (data) {
        pp_mem_header_t * p = (pp_mem_header_t *)((uint8_t *)data - PP_MEM_HEADER_SIZE);
        memset(data, 0, p->size);
    }
    return data;
}

/**
 * @brief Standard calloc wrapper
 */
void * pp_calloc(size_t num, size_t size)
{
    size_t total = num * size;
    return pp_malloc_zeroed(total);
}

/**
 * @brief Alias for zalloc
 */
void * pp_zalloc(size_t num, size_t size)
{
    return pp_calloc(num, size);
}

/**
 * @brief Safe memory deallocation
 */
void pp_free(void * data)
{
    if (data == NULL) return;

    /* Move back by explicitly aligned padding distance */
    pp_mem_header_t * p = (pp_mem_header_t *)((uint8_t *)data - PP_MEM_HEADER_SIZE);

    if (p->magic != PP_MEM_MAGIC) {
        PP_MEM_ERROR("CRITICAL: Memory corruption detected at %p! Magic number is broken.", data);
        return;
    }

    p->magic = 0;
    free(p);
}

/**
 * @brief Dynamic memory reallocation
 */
void * pp_realloc(void * data_p, size_t new_size)
{
    if (data_p == NULL) return pp_malloc(new_size);
    
    if (new_size == 0) {
        pp_free(data_p);
        return NULL;
    }

    pp_mem_header_t * old_p = (pp_mem_header_t *)((uint8_t *)data_p - PP_MEM_HEADER_SIZE);
    if (old_p->magic != PP_MEM_MAGIC) {
        PP_MEM_ERROR("ERROR: realloc failed. Source pointer %p is corrupted or freed.", data_p);
        return NULL;
    }

    size_t aligned_new_size = PP_MEM_ALIGN(new_size);
    
    if (old_p->size == aligned_new_size) {
        return data_p;
    }

    void * new_data_ptr = pp_malloc(aligned_new_size);
    if (new_data_ptr == NULL) return NULL;

    size_t copy_size = old_p->size < aligned_new_size ? old_p->size : aligned_new_size;
    memcpy(new_data_ptr, data_p, copy_size);

    pp_free(data_p);
    return new_data_ptr;
}

#endif /* PP_MEM_IMPLEMENTATION */
