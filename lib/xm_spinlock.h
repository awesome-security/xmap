/*
 *
 *      Filename: xm_spinlock.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-10 10:42:49
 * Last Modified: 2017-02-10 10:42:49
 */

#ifndef XM_SPINLOCK_H
#define XM_SPINLOCK_H

#include "xm_constants.h"
/**
 * The xm_spinlock_t type.
 */
typedef struct {
	volatile int locked; /**< lock status 0 = unlocked, 1 = locked */
} xm_spinlock_t;

/**
 * A static spinlock initializer.
 */
#define RTE_SPINLOCK_INITIALIZER { 0 }

/**
 * Initialize the spinlock to an unlocked state.
 *
 * @param sl
 *   A pointer to the spinlock.
 */
static inline void
xm_spinlock_init(xm_spinlock_t *sl)
{
	sl->locked = 0;
}

/**
 * Take the spinlock.
 *
 * @param sl
 *   A pointer to the spinlock.
 */
static inline void
xm_spinlock_lock(xm_spinlock_t *sl){
	int lock_val = 1;
	asm volatile (
			"1:\n"
			"xchg %[locked], %[lv]\n"
			"test %[lv], %[lv]\n"
			"jz 3f\n"
			"2:\n"
			"pause\n"
			"cmpl $0, %[locked]\n"
			"jnz 2b\n"
			"jmp 1b\n"
			"3:\n"
			: [locked] "=m" (sl->locked), [lv] "=q" (lock_val)
			: "[lv]" (lock_val)
			: "memory");
}

/**
 * Release the spinlock.
 *
 * @param sl
 *   A pointer to the spinlock.
 */
static inline void
xm_spinlock_unlock (xm_spinlock_t *sl){

	int unlock_val = 0;
	asm volatile (
			"xchg %[locked], %[ulv]\n"
			: [locked] "=m" (sl->locked), [ulv] "=q" (unlock_val)
			: "[ulv]" (unlock_val)
			: "memory");

}
/**
 * Try to take the lock.
 *
 * @param sl
 *   A pointer to the spinlock.
 * @return
 *   1 if the lock is successfully taken; 0 otherwise.
 */
static inline int
xm_spinlock_trylock (xm_spinlock_t *sl){

	int lockval = 1;

	asm volatile (
			"xchg %[locked], %[lockval]"
			: [locked] "=m" (sl->locked), [lockval] "=q" (lockval)
			: "[lockval]" (lockval)
			: "memory");

	return lockval == 0;
}

/**
 * Test if the lock is taken.
 *
 * @param sl
 *   A pointer to the spinlock.
 * @return
 *   1 if the lock is currently taken; 0 otherwise.
 */
static inline int xm_spinlock_is_locked (xm_spinlock_t *sl)
{
	return sl->locked;
}

#endif /* XM_SPINLOCK_H */
