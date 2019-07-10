/*
 *
 *      Filename: xm_atomic.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-02-10 11:44:16
 * Last Modified: 2017-02-10 11:44:16
 */

#ifndef XM_ATOMIC_H
#define XM_ATOMIC_H

#include "xm_constants.h"

#define MPLOCKED        "lock ; "       /**< Insert MP lock prefix. */



/**
 * Compiler barrier.
 *
 * Guarantees that operation reordering does not occur at compile time
 * for operations directly before and after the barrier.
 */
#define	xm_compiler_barrier() do {		\
	asm volatile ("" : : : "memory");	\
} while(0)

#define	xm_mb() _mm_mfence()

#define	xm_wmb() _mm_sfence()

#define	xm_rmb() _mm_lfence()

#define xm_smp_mb() xm_mb()

#define xm_smp_wmb() xm_compiler_barrier()

#define xm_smp_rmb() xm_compiler_barrier()

/*------------------------- 16 bit atomic operations -------------------------*/

/**
 * Atomic compare and set.
 *
 * (atomic) equivalent to:
 *   if (*dst == exp)
 *     *dst = src (all 16-bit words)
 *
 * @param dst
 *   The destination location into which the value will be written.
 * @param exp
 *   The expected value.
 * @param src
 *   The new value.
 * @return
 *   Non-zero on success; 0 on failure.
 */
static inline int
xm_atomic16_cmpset(volatile uint16_t *dst, uint16_t exp, uint16_t src){

	uint8_t res;

	asm volatile(
			MPLOCKED
			"cmpxchgw %[src], %[dst];"
			"sete %[res];"
			: [res] "=a" (res),     /* output */
			  [dst] "=m" (*dst)
			: [src] "r" (src),      /* input */
			  "a" (exp),
			  "m" (*dst)
			: "memory");            /* no-clobber list */
	return res;
}



/**
 * The atomic counter structure.
 */
typedef struct {
	volatile int16_t cnt; /**< An internal counter value. */
} xm_atomic16_t;

/**
 * Static initializer for an atomic counter.
 */
#define XM_ATOMIC16_INIT(val) { (val) }

/**
 * Initialize an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
xm_atomic16_init(xm_atomic16_t *v)
{
	v->cnt = 0;
}

static inline int xm_atomic16_test_and_set(xm_atomic16_t *v)
{
	return xm_atomic16_cmpset((volatile uint16_t *)&v->cnt, 0, 1);
}
/**
 * Atomically read a 16-bit value from a counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   The value of the counter.
 */
static inline int16_t
xm_atomic16_read(const xm_atomic16_t *v)
{
	return v->cnt;
}

/**
 * Atomically set a counter to a 16-bit value.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param new_value
 *   The new value for the counter.
 */
static inline void
xm_atomic16_set(xm_atomic16_t *v, int16_t new_value)
{
	v->cnt = new_value;
}

/**
 * Atomically add a 16-bit value to an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 */
static inline void
xm_atomic16_add(xm_atomic16_t *v, int16_t inc)
{
	__sync_fetxm_and_add(&v->cnt, inc);
}

/**
 * Atomically subtract a 16-bit value from an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 */
static inline void
xm_atomic16_sub(xm_atomic16_t *v, int16_t dec)
{
	__sync_fetxm_and_sub(&v->cnt, dec);
}

/**
 * Atomically increment a counter by one.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
xm_atomic16_inc(xm_atomic16_t *v){

	asm volatile(
			MPLOCKED
			"incw %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

/**
 * Atomically decrement a counter by one.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
xm_atomic16_dec(xm_atomic16_t *v){

	asm volatile(
			MPLOCKED
			"decw %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);

}

/**
 * Atomically add a 16-bit value to a counter and return the result.
 *
 * Atomically adds the 16-bits value (inc) to the atomic counter (v) and
 * returns the value of v after addition.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 * @return
 *   The value of v after the addition.
 */
static inline int16_t
xm_atomic16_add_return(xm_atomic16_t *v, int16_t inc)
{
	return __sync_add_and_fetch(&v->cnt, inc);
}

/**
 * Atomically subtract a 16-bit value from a counter and return
 * the result.
 *
 * Atomically subtracts the 16-bit value (inc) from the atomic counter
 * (v) and returns the value of v after the subtraction.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 * @return
 *   The value of v after the subtraction.
 */
static inline int16_t
xm_atomic16_sub_return(xm_atomic16_t *v, int16_t dec)
{
	return __sync_sub_and_fetch(&v->cnt, dec);
}

static inline int xm_atomic16_inc_and_test(xm_atomic16_t *v)
{
	uint8_t ret;

	asm volatile(
			MPLOCKED
			"incw %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt),  /* output */
			  [ret] "=qm" (ret)
			);
	return ret != 0;
}

static inline int xm_atomic16_dec_and_test(xm_atomic16_t *v)
{
	uint8_t ret;

	asm volatile(MPLOCKED
			"decw %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt),  /* output */
			  [ret] "=qm" (ret)
			);
	return ret != 0;
}


/**
 * Atomically set a 16-bit counter to 0.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void xm_atomic16_clear(xm_atomic16_t *v)
{
	v->cnt = 0;
}

/*------------------------- 32 bit atomic operations -------------------------*/

/**
 * Atomic compare and set.
 *
 * (atomic) equivalent to:
 *   if (*dst == exp)
 *     *dst = src (all 32-bit words)
 *
 * @param dst
 *   The destination location into which the value will be written.
 * @param exp
 *   The expected value.
 * @param src
 *   The new value.
 * @return
 *   Non-zero on success; 0 on failure.
 */
static inline int
xm_atomic32_cmpset(volatile uint32_t *dst, uint32_t exp, uint32_t src){

	uint8_t res;

	asm volatile(
			MPLOCKED
			"cmpxchgl %[src], %[dst];"
			"sete %[res];"
			: [res] "=a" (res),     /* output */
			  [dst] "=m" (*dst)
			: [src] "r" (src),      /* input */
			  "a" (exp),
			  "m" (*dst)
			: "memory");            /* no-clobber list */
	return res;
}

/**
 * The atomic counter structure.
 */
typedef struct {
	volatile int32_t cnt; /**< An internal counter value. */
} xm_atomic32_t;

/**
 * Static initializer for an atomic counter.
 */
#define XM_ATOMIC32_INIT(val) { (val) }

/**
 * Initialize an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
xm_atomic32_init(xm_atomic32_t *v)
{
	v->cnt = 0;
}

/**
 * Atomically read a 32-bit value from a counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   The value of the counter.
 */
static inline int32_t
xm_atomic32_read(const xm_atomic32_t *v)
{
	return v->cnt;
}

/**
 * Atomically set a counter to a 32-bit value.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param new_value
 *   The new value for the counter.
 */
static inline void
xm_atomic32_set(xm_atomic32_t *v, int32_t new_value)
{
	v->cnt = new_value;
}

/**
 * Atomically add a 32-bit value to an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 */
static inline void
xm_atomic32_add(xm_atomic32_t *v, int32_t inc)
{
	__sync_fetxm_and_add(&v->cnt, inc);
}

/**
 * Atomically subtract a 32-bit value from an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 */
static inline void
xm_atomic32_sub(xm_atomic32_t *v, int32_t dec)
{
	__sync_fetxm_and_sub(&v->cnt, dec);
}

static inline void
xm_atomic32_inc(xm_atomic32_t *v)
{
	asm volatile(
			MPLOCKED
			"incl %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

static inline void
xm_atomic32_dec(xm_atomic32_t *v)
{
	asm volatile(
			MPLOCKED
			"decl %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

static inline int xm_atomic32_inc_and_test(xm_atomic32_t *v)
{
	uint8_t ret;

	asm volatile(
			MPLOCKED
			"incl %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt),  /* output */
			  [ret] "=qm" (ret)
			);
	return ret != 0;
}

static inline int xm_atomic32_dec_and_test(xm_atomic32_t *v)
{
	uint8_t ret;

	asm volatile(MPLOCKED
			"decl %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt),  /* output */
			  [ret] "=qm" (ret)
			);
	return ret != 0;
}

/**
 * Atomically add a 32-bit value to a counter and return the result.
 *
 * Atomically adds the 32-bits value (inc) to the atomic counter (v) and
 * returns the value of v after addition.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 * @return
 *   The value of v after the addition.
 */
static inline int32_t
xm_atomic32_add_return(xm_atomic32_t *v, int32_t inc)
{
	return __sync_add_and_fetch(&v->cnt, inc);
}

/**
 * Atomically subtract a 32-bit value from a counter and return
 * the result.
 *
 * Atomically subtracts the 32-bit value (inc) from the atomic counter
 * (v) and returns the value of v after the subtraction.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 * @return
 *   The value of v after the subtraction.
 */
static inline int32_t
xm_atomic32_sub_return(xm_atomic32_t *v, int32_t dec)
{
	return __sync_sub_and_fetch(&v->cnt, dec);
}



/**
 * Atomically set a 32-bit counter to 0.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void xm_atomic32_clear(xm_atomic32_t *v)
{
	v->cnt = 0;
}

/*------------------------- 64 bit atomic operations -------------------------*/


/**
 * The atomic counter structure.
 */
typedef struct {
	volatile int64_t cnt;  /**< Internal counter value. */
} xm_atomic64_t;

/**
 * Static initializer for an atomic counter.
 */
#define XM_ATOMIC64_INIT(val) { (val) }


static inline int
xm_atomic64_cmpset(volatile uint64_t *dst, uint64_t exp, uint64_t src)
{
	uint8_t res;


	asm volatile(
			MPLOCKED
			"cmpxchgq %[src], %[dst];"
			"sete %[res];"
			: [res] "=a" (res),     /* output */
			  [dst] "=m" (*dst)
			: [src] "r" (src),      /* input */
			  "a" (exp),
			  "m" (*dst)
			: "memory");            /* no-clobber list */

	return res;
}

static inline void
xm_atomic64_init(xm_atomic64_t *v)
{
	v->cnt = 0;
}

static inline int64_t
xm_atomic64_read(xm_atomic64_t *v)
{
	return v->cnt;
}

static inline void
xm_atomic64_set(xm_atomic64_t *v, int64_t new_value)
{
	v->cnt = new_value;
}

static inline void
xm_atomic64_add(xm_atomic64_t *v, int64_t inc)
{
	asm volatile(
			MPLOCKED
			"addq %[inc], %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: [inc] "ir" (inc),     /* input */
			  "m" (v->cnt)
			);
}

static inline void
xm_atomic64_sub(xm_atomic64_t *v, int64_t dec)
{
	asm volatile(
			MPLOCKED
			"subq %[dec], %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: [dec] "ir" (dec),     /* input */
			  "m" (v->cnt)
			);
}

static inline void
xm_atomic64_inc(xm_atomic64_t *v)
{
	asm volatile(
			MPLOCKED
			"incq %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

static inline void
xm_atomic64_dec(xm_atomic64_t *v)
{
	asm volatile(
			MPLOCKED
			"decq %[cnt]"
			: [cnt] "=m" (v->cnt)   /* output */
			: "m" (v->cnt)          /* input */
			);
}

static inline int64_t
xm_atomic64_add_return(xm_atomic64_t *v, int64_t inc)
{
	int64_t prev = inc;

	asm volatile(
			MPLOCKED
			"xaddq %[prev], %[cnt]"
			: [prev] "+r" (prev),   /* output */
			  [cnt] "=m" (v->cnt)
			: "m" (v->cnt)          /* input */
			);
	return prev + inc;
}

static inline int64_t
xm_atomic64_sub_return(xm_atomic64_t *v, int64_t dec)
{
	return xm_atomic64_add_return(v, -dec);
}

static inline int xm_atomic64_inc_and_test(xm_atomic64_t *v)
{
	uint8_t ret;

	asm volatile(
			MPLOCKED
			"incq %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt), /* output */
			  [ret] "=qm" (ret)
			);

	return ret != 0;
}

static inline int xm_atomic64_dec_and_test(xm_atomic64_t *v)
{
	uint8_t ret;

	asm volatile(
			MPLOCKED
			"decq %[cnt] ; "
			"sete %[ret]"
			: [cnt] "+m" (v->cnt),  /* output */
			  [ret] "=qm" (ret)
			);
	return ret != 0;
}

static inline int xm_atomic64_test_and_set(xm_atomic64_t *v)
{
	return xm_atomic64_cmpset((volatile uint64_t *)&v->cnt, 0, 1);
}

static inline void xm_atomic64_clear(xm_atomic64_t *v)
{
	v->cnt = 0;
}

#endif /* XM_ATOMIC_H */
