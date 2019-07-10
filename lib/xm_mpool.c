/*
 *
 *      Filename: xm_mpool.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 11:04:58
 * Last Modified: 2018-05-04 12:08:59
 */

#include "xm_mpool.h"

xm_pool_t *
xm_pool_create(size_t size)
{
    xm_pool_t  *p;

    p = (xm_pool_t*)memalign(XM_POOL_ALIGNMENT, size);

    if (p == NULL) {
        return NULL;
    }

    p->d.last = (void *) p + sizeof(xm_pool_t);
    p->d.end = (void *) p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(xm_pool_t);
    p->max = (size < XM_MAX_ALLOC_FROM_POOL) ? size : XM_MAX_ALLOC_FROM_POOL;

    p->current = p;
    p->large = NULL;
    p->cleanup = NULL;

    return p;
}

void
xm_pool_destroy(xm_pool_t *pool)
{
    xm_pool_t          *p, *n;
    xm_pool_large_t    *l;
    xm_pool_cleanup_t  *c;

    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            c->handler(c->data);
        }
    }

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        free(p);

        if (n == NULL) {
            break;
        }
    }
}


void
xm_pool_reset(xm_pool_t *pool)
{
    xm_pool_t        *p;
    xm_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }

    for (p = pool; p; p = p->d.next) {
        p->d.last = (void *) p + sizeof(xm_pool_t);
        p->d.failed = 0;
    }

    pool->current = pool;
    pool->large = NULL;
}

static void *
xm_palloc_block(xm_pool_t *pool, size_t size)
{
    void      *m;
    size_t       psize;
    xm_pool_t  *p, *new;

    psize = (size_t) (pool->d.end - (void *) pool);

    m = memalign(XM_POOL_ALIGNMENT, psize);

    if (m == NULL) {
        return NULL;
    }

    new = (xm_pool_t *) m;

    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;

    m += sizeof(xm_pool_data_t);
    m = xm_align_ptr(m, XM_ALIGNMENT);
    new->d.last = m + size;

    for (p = pool->current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            pool->current = p->d.next;
        }
    }

    p->d.next = new;

    return m;
}

static xm_inline void *
xm_palloc_small(xm_pool_t *pool, size_t size, unsigned int align)
{
    void      *m;
    xm_pool_t  *p;

    p = pool->current;

    do {
        m = p->d.last;

        if (align) {
            m = xm_align_ptr(m, XM_ALIGNMENT);
        }

        if ((size_t) (p->d.end - m) >= size) {
            p->d.last = m + size;

            return m;
        }

        p = p->d.next;

    } while (p);

    return xm_palloc_block(pool, size);
}




static void *
xm_palloc_large(xm_pool_t *pool, size_t size)
{
    void              *p;
    unsigned int         n;
    xm_pool_large_t  *large;

    p = malloc(size);
    if (p == NULL) {
        return NULL;
    }

    n = 0;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    large = xm_palloc_small(pool, sizeof(xm_pool_large_t), 1);
    if (large == NULL) {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

void *
xm_palloc(xm_pool_t *pool, size_t size)
{
    if (size <= pool->max) {
        return xm_palloc_small(pool, size, 1);
    }

    return xm_palloc_large(pool, size);
}


void *
xm_pnalloc(xm_pool_t *pool, size_t size)
{
    if (size <= pool->max) {
        return xm_palloc_small(pool, size, 0);
    }

    return xm_palloc_large(pool, size);
}

void *
xm_pmemalign(xm_pool_t *pool, size_t size, size_t alignment)
{
    void              *p;
    xm_pool_large_t  *large;

    p = memalign(alignment, size);
    if (p == NULL) {
        return NULL;
    }

    large = xm_palloc_small(pool, sizeof(xm_pool_large_t), 1);
    if (large == NULL) {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


int
xm_pfree(xm_pool_t *pool, void *p)
{
    xm_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            free(l->alloc);
            l->alloc = NULL;

            return 0;
        }
    }

    return -1;
}


void *
xm_pcalloc(xm_pool_t *pool, size_t size)
{
    void *p;

    p = xm_palloc(pool, size);
    if (p) {
        memset(p, 0,size);
    }

    return p;
}


xm_pool_cleanup_t *
xm_pool_cleanup_add(xm_pool_t *p, size_t size)
{
    xm_pool_cleanup_t  *c;

    c = xm_palloc(p, sizeof(xm_pool_cleanup_t));
    if (c == NULL) {
        return NULL;
    }

    if (size) {
        c->data = xm_palloc(p, size);
        if (c->data == NULL) {
            return NULL;
        }

    } else {
        c->data = NULL;
    }

    c->handler = NULL;
    c->next = p->cleanup;

    p->cleanup = c;

    return c;
}

