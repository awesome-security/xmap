/*
 *
 *      Filename: xm_rwlock.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-10 13:25:44
 * Last Modified: 2017-02-10 13:25:44
 */

#ifndef XM_RWLOCK_H
#define XM_RWLOCK_H

#include "xm_constants.h"
#include "xm_atomic.h"



/**
 * The xm_rwlock_t type.
 *
 * cnt is -1 when write lock is held, and > 0 when read locks are held.
 */
typedef struct {
	volatile int32_t cnt; /**< -1 when W lock held, > 0 when R locks held. */
} xm_rwlock_t;

/**
 * A static rwlock initializer.
 */
#define XM_RWLOCK_INITIALIZER { 0 }

/**
 * Initialize the rwlock to an unlocked state.
 *
 * @param rwl
 *   A pointer to the rwlock structure.
 */
static inline void
xm_rwlock_init(xm_rwlock_t *rwl)
{
	rwl->cnt = 0;
}

/**
 * Take a read lock. Loop until the lock is held.
 *
 * @param rwl
 *   A pointer to a rwlock structure.
 */
static inline void
xm_rwlock_read_lock(xm_rwlock_t *rwl)
{
	int32_t x;
	int success = 0;

	while (success == 0) {
		x = rwl->cnt;
		/* write lock is held */
		if (x < 0) {
			xm_pause();
			continue;
		}
		success = xm_atomic32_cmpset((volatile uint32_t *)&rwl->cnt,
					      x, x + 1);
	}
}

/**
 * Release a read lock.
 *
 * @param rwl
 *   A pointer to the rwlock structure.
 */
static inline void
xm_rwlock_read_unlock(xm_rwlock_t *rwl)
{
	xm_atomic32_dec((xm_atomic32_t *)(intptr_t)&rwl->cnt);
}

/**
 * Take a write lock. Loop until the lock is held.
 *
 * @param rwl
 *   A pointer to a rwlock structure.
 */
static inline void
xm_rwlock_write_lock(xm_rwlock_t *rwl)
{
	int32_t x;
	int success = 0;

	while (success == 0) {
		x = rwl->cnt;
		/* a lock is held */
		if (x != 0) {
			xm_pause();
			continue;
		}
		success = xm_atomic32_cmpset((volatile uint32_t *)&rwl->cnt,
					      0, -1);
	}
}

/**
 * Release a write lock.
 *
 * @param rwl
 *   A pointer to a rwlock structure.
 */
static inline void
xm_rwlock_write_unlock(xm_rwlock_t *rwl)
{
	xm_atomic32_inc((xm_atomic32_t *)(intptr_t)&rwl->cnt);
}



#endif /*XM_RWLOCK_H*/
