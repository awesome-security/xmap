/*
 *
 *      Filename: xm_object_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-27 13:52:28
 * Last Modified: 2017-12-27 13:52:28
 */

#ifndef XM_OBJECT_POOL_H
#define XM_OBJECT_POOL_H

typedef struct xm_object_pool_t xm_object_pool_t;
typedef struct xm_object_mem_t xm_object_mem_t;
typedef struct xm_object_item_t xm_object_item_t;

#include "xm_mpool.h"
#include "xm_list.h"

#define OBJECT_MEM_SIZE 64*1024

struct xm_object_pool_t {

	xm_object_mem_t *mm_list;

	struct list_head free_list;

	void *priv_data;

	void (*obj_init)(void *obj,void *priv_data);

	size_t object_limits;
	size_t object_size;

	size_t n_objects;

	size_t n_mms;

	size_t n_frees;
	
	size_t n_using;
};

struct xm_object_mem_t {

	xm_object_mem_t *next;
};

struct xm_object_item_t {

	struct list_head node;
};


extern xm_object_pool_t * xm_object_pool_create(xm_pool_t *mp,size_t object_limits,
	size_t object_size,
	void *priv_data,
	void (*obj_init)(void *obj,void *priv_data));

extern void xm_object_pool_destroy(xm_object_pool_t *omp);

extern void * xm_object_pool_get(xm_object_pool_t *omp);

extern void  xm_object_pool_put(xm_object_pool_t *omp,void *obj);

extern void xm_object_pool_dump(xm_object_pool_t *omp,FILE *fp);

#endif /*XM_OBJECT_POOL_H*/
