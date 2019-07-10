/*
 *
 *      Filename: xm_time.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 14:26:54
 * Last Modified: 2017-01-11 14:26:54
 */

#ifndef XM_TIME_H
#define XM_TIME_H

#include "xm_constants.h"
#include "xm_mpool.h"
/** month names */
extern const char xm_month_snames[12][4];
/** day names */
extern const char xm_day_snames[7][4];


/** number of microseconds since 00:00:00 January 1, 1970 UTC */
typedef int64_t xm_time_t;


/** mechanism to properly type xm_time_t literals */
#define XM_TIME_C(val) (val ## L)

/** mechanism to properly print xm_time_t values */
#define XM_TIME_T_FMT XM_INT64_T_FMT

/** intervals for I/O timeouts, in microseconds */
typedef int64_t xm_interval_time_t;
/** short interval for I/O timeouts, in microseconds */
typedef int32_t xm_short_interval_time_t;

/** number of microseconds per second */
#define XM_USEC_PER_SEC 1000000L

/** @return xm_time_t as a second */
#define xm_time_sec(time) ((time) / XM_USEC_PER_SEC)

/** @return xm_time_t as a usec */
#define xm_time_usec(time) ((time) % XM_USEC_PER_SEC)

/** @return xm_time_t as a msec */
#define xm_time_msec(time) (((time) / 1000) % 1000)

/** @return xm_time_t as a msec */
#define xm_time_as_msec(time) ((time) / 1000)

/** @return milliseconds as an xm_time_t */
#define xm_time_from_msec(msec) ((xm_time_t)(msec) * 1000)

/** @return seconds as an xm_time_t */
#define xm_time_from_sec(sec) ((xm_time_t)(sec) * XM_USEC_PER_SEC)

/** @return a second and usec combination as an xm_time_t */
#define xm_time_make(sec, usec) ((xm_time_t)(sec) * XM_USEC_PER_SEC \
                                + (xm_time_t)(usec))

/**
 * @return the current time
 */
xm_time_t xm_time_now(void);

/** @see xm_time_exp_t */
typedef struct xm_time_exp_t xm_time_exp_t;

/**
 * a structure similar to ANSI struct tm with the following differences:
 *  - tm_usec isn't an ANSI field
 *  - tm_gmtoff isn't an ANSI field (it's a BSDism)
 */
struct xm_time_exp_t {
    /** microseconds past tm_sec */
    int32_t tm_usec;
    /** (0-61) seconds past tm_min */
    int32_t tm_sec;
    /** (0-59) minutes past tm_hour */
    int32_t tm_min;
    /** (0-23) hours past midnight */
    int32_t tm_hour;
    /** (1-31) day of the month */
    int32_t tm_mday;
    /** (0-11) month of the year */
    int32_t tm_mon;
    /** year since 1900 */
    int32_t tm_year;
    /** (0-6) days since Sunday */
    int32_t tm_wday;
    /** (0-365) days since January 1 */
    int32_t tm_yday;
    /** daylight saving time */
    int32_t tm_isdst;
    /** seconds east of UTC */
    int32_t tm_gmtoff;
};

/**
 * Convert an ansi time_t to an xm_time_t
 * @param result the resulting xm_time_t
 * @param input the time_t to convert
 */
int xm_time_ansi_put(xm_time_t *result, 
                                                    time_t input);

/**
 * Convert a time to its human readable components using an offset
 * from GMT.
 * @param result the exploded time
 * @param input the time to explode
 * @param offs the number of seconds offset to apply
 */
int xm_time_exp_tz(xm_time_exp_t *result,
                                          xm_time_t input,
                                          int32_t offs);

/**
 * Convert a time to its human readable components (GMT).
 * @param result the exploded time
 * @param input the time to explode
 */
int xm_time_exp_gmt(xm_time_exp_t *result, 
                                           xm_time_t input);

/**
 * Convert a time to its human readable components in the local timezone.
 * @param result the exploded time
 * @param input the time to explode
 */
int xm_time_exp_lt(xm_time_exp_t *result, 
                                          xm_time_t input);

/**
 * Convert time value from human readable format to a numeric xm_time_t
 * (elapsed microseconds since the epoch).
 * @param result the resulting imploded time
 * @param input the input exploded time
 */
int xm_time_exp_get(xm_time_t *result, 
                                           xm_time_exp_t *input);

/**
 * Convert time value from human readable format to a numeric xm_time_t that
 * always represents GMT.
 * @param result the resulting imploded time
 * @param input the input exploded time
 */
int xm_time_exp_gmt_get(xm_time_t *result, 
                                               xm_time_exp_t *input);

/**
 * Sleep for the specified number of micro-seconds.
 * @param t desired amount of time to sleep.
 * @warning May sleep for longer than the specified time. 
 */
void xm_sleep(xm_interval_time_t t);

/** length of a RFC822 Date */
#define XM_RFC822_DATE_LEN (30)
/**
 * xm_rfc822_date formats dates in the RFC822
 * format in an efficient manner.  It is a fixed length
 * format which requires XM_RFC822_DATA_LEN bytes of storage,
 * including the trailing NUL terminator.
 * @param date_str String to write to.
 * @param t the time to convert 
 */
int xm_rfc822_date(char *date_str, xm_time_t t);

/** length of a CTIME date */
#define XM_CTIME_LEN (25)
/**
 * xm_ctime formats dates in the ctime() format
 * in an efficient manner.  It is a fixed length format
 * and requires XM_CTIME_LEN bytes of storage including
 * the trailing NUL terminator.
 * Unlike ANSI/ISO C ctime(), xm_ctime() does not include
 * a \\n at the end of the string.
 * @param date_str String to write to.
 * @param t the time to convert 
 */
int xm_ctime(char *date_str, xm_time_t t);

/**
 * Formats the exploded time according to the format specified
 * @param s string to write to
 * @param retsize The length of the returned string
 * @param max The maximum length of the string
 * @param format The format for the time string
 * @param tm The time to convert
 */
int xm_strftime(char *s, size_t *retsize, 
                                       size_t max, const char *format, 
                                       xm_time_exp_t *tm);

/**
 * Improve the clock resolution for the lifetime of the given pool.
 * Generally this is only desirable on benchmarking and other very
 * time-sensitive applications, and has no impact on most platforms.
 * @param p The pool to associate the finer clock resolution 
 */
void xm_time_clock_hires(xm_pool_t *p);


#endif /* XM_TIME_H */
