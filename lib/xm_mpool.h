/*
 *
 *      Filename: xm_mpool.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 10:07:10
 * Last Modified: 2017-01-09 10:07:10
 */

#ifndef XM_MPOOL_H
#define XM_MPOOL_H

typedef void (*xm_pool_cleanup_pt)(void *data);
typedef struct xm_pool_cleanup_t  xm_pool_cleanup_t;
typedef struct xm_pool_large_t xm_pool_large_t;
typedef struct xm_pool_t xm_pool_t;
typedef struct xm_pool_data_t xm_pool_data_t;

#include "xm_constants.h"
#include "xm_list.h"

#define XM_MAX_ALLOC_FROM_POOL  (4096 - 1)

#define XM_DEFAULT_POOL_SIZE    (16 * 1024)

#define XM_POOL_ALIGNMENT       16
#define XM_MIN_POOL_SIZE                                                     \
    xm_align((sizeof(xm_pool_t) + 2 * sizeof(xm_pool_large_t)),            \
              XM_POOL_ALIGNMENT)



struct xm_pool_cleanup_t {
    xm_pool_cleanup_pt   handler;
    void                 *data;
    xm_pool_cleanup_t   *next;
};


struct xm_pool_large_t {
    xm_pool_large_t     *next;
    void                 *alloc;
};


struct xm_pool_data_t {
    void               *last;
    void               *end;
    xm_pool_t           *next;
    size_t           failed;
};


struct xm_pool_t {

    //struct list_head node;
    xm_pool_data_t       d;
    size_t                max;
    xm_pool_t           *current;
    xm_pool_large_t     *large;
    xm_pool_cleanup_t   *cleanup;
};

extern xm_pool_t *
xm_pool_create(size_t size);

extern void 
xm_pool_destroy(xm_pool_t *pool);

extern void 
xm_pool_reset(xm_pool_t *pool);

extern void *
xm_palloc(xm_pool_t *pool, size_t size);

extern void *
xm_pnalloc(xm_pool_t *pool, size_t size);

extern void *
xm_pcalloc(xm_pool_t *pool, size_t size);

extern void *
xm_pmemalign(xm_pool_t *pool, size_t size, size_t alignment);

extern int 
xm_pfree(xm_pool_t *pool, void *p);


extern xm_pool_cleanup_t *
xm_pool_cleanup_add(xm_pool_t *p, size_t size);

#endif /* XM_MPOOL_H */
