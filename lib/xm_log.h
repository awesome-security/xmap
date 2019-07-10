/*
 *
 *      Filename: xm_log.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-18 10:14:56
 * Last Modified: 2017-01-18 10:14:56
 */

#ifndef XM_LOG_H
#define XM_LOG_H

#include "xm_file.h"
#include "xm_errno.h"

#define XM_LOG_STDERR            0
#define XM_LOG_EMERG             1
#define XM_LOG_ALERT             2
#define XM_LOG_CRIT              3
#define XM_LOG_ERR               4
#define XM_LOG_WARN              5
#define XM_LOG_NOTICE            6
#define XM_LOG_INFO              7
#define XM_LOG_DEBUG             8

typedef struct {
    int       log_level;
    xm_file_t     *file;
    xm_pool_t     *mp;
}xm_log_t;


#define XM_MAX_ERROR_STR   2048

#define xm_log(level,fmt,...) xm_log_error_core(level,0,fmt,##__VA_ARGS__)

#define XM_DEBUG_LOG(fmt, ...) xm_log_error_core(XM_LOG_DEBUG,0,fmt,##__VA_ARGS__)

#define XM_INFO_LOG(fmt, ...) xm_log_error_core(XM_LOG_INFO,0,fmt,##__VA_ARGS__)

#define XM_NOTICE_LOG(fmt, ...) xm_log_error_core(XM_LOG_NOTICE,0,fmt,##__VA_ARGS__)

#define XM_WARN_LOG(fmt, ...) xm_log_error_core(XM_LOG_WARN,0,fmt,##__VA_ARGS__)

#define XM_ERR_LOG(fmt, ...) xm_log_error_core(XM_LOG_ERR,0,fmt,##__VA_ARGS__)

#define XM_CRIT_LOG(fmt, ...) xm_log_error_core(XM_LOG_CRIT,0,fmt,##__VA_ARGS__)

#define XM_ALERT_LOG(fmt, ...) xm_log_error_core(XM_LOG_ALERT,0,fmt,##__VA_ARGS__)

#define XM_EMERG_LOG(fmt, ...) xm_log_error_core(XM_LOG_EMERG,0,fmt,##__VA_ARGS__)

void xm_log_error_core(int level,int err,
    const char *fmt, ...);


void xm_log_init(xm_pool_t *mp,const char *name,int level);

static inline void
xm_write_stderr(char *text)
{
    fprintf(stderr, text, strlen(text));
}


#endif /* XM_LOG_H */
