/*
 *
 *      Filename: xm_log.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-18 10:19:29
 * Last Modified: 2017-01-18 11:02:24
 */

#include "xm_log.h"
#include "xm_util.h"

static xm_log_t log_s,*log_ptr=&log_s;

static const char* err_levels[] = {
    "stderr",
    "emerg",
    "alert",
    "crit",
    "error",
    "warn",
    "notich",
    "info",
    "debug"
};


void
xm_log_error_core(int level,int err,
    const char *fmt, ...)

{
    va_list  args;
    char   errstr1[XM_MAX_ERROR_STR]={0};
    char   errstr2[XM_MAX_ERROR_STR]={0};
	char   errstr3[XM_MAX_ERROR_STR]={0};

    char   tstr[100]={0};
    size_t nbytes_written,nbytes;

    if(log_ptr->log_level<level||log_ptr->file == NULL) {
        return;
    }

    va_start(args, fmt);
    xm_vsnprintf(errstr1,sizeof(errstr1), fmt, args);
    va_end(args);


    xm_snprintf(errstr2,sizeof(errstr2),"[%s][%d][%s][%s] %s\n",
            xm_current_logtime_with_buf(tstr,100),
            getpid(),
            err_levels[level],
            err?xm_strerror(err,errstr3,XM_MAX_ERROR_STR):"--",
            errstr1
            );

    nbytes = strlen(errstr2);
    xm_file_write_full(log_ptr->file,errstr2,nbytes,&nbytes_written);
}

void
xm_log_init(xm_pool_t *mp,const char *name,int level)
{

    int rc;

    if(level<XM_LOG_STDERR||level>XM_LOG_DEBUG)
        level = XM_LOG_NOTICE;

    log_ptr->log_level = level;
    log_ptr->mp = mp;

    rc = xm_file_open(&log_ptr->file,name,
                   XM_WRITE | XM_APPEND | XM_CREATE | XM_BINARY,
                   CREATEMODE, mp);

    if (rc != 0) {
        printf("Failed to open  log file: %s\n",
            name);
        exit(-1);
    }
}



