/*
 *
 *      Filename: xm_errno.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 10:08:19
 * Last Modified: 2017-01-11 10:08:19
 */

#ifndef XM_ERRNO_H
#define XM_ERRNO_H

#include <errno.h>
#include "xm_constants.h"

extern char * xm_strerror(int statcode, char *buf,
                                 size_t bufsize);

/**
 * XM_OS_START_ERROR is where the GW specific error values start.
 */
#define XM_OS_START_ERROR     20000
/**
 * XM_OS_ERRSPACE_SIZE is the maximum number of errors you can fit
 *    into one of the error/status ranges below -- except for
 *    XM_OS_START_USERERR, which see.
 */
#define XM_OS_ERRSPACE_SIZE 50000
/**
 * XM_UTIL_ERRSPACE_SIZE is the size of the space that is reserved for
 * use within gw-util. This space is reserved above that used by GW
 * internally.
 * @note This number MUST be smaller than XM_OS_ERRSPACE_SIZE by a
 *       large enough amount that GW has sufficient room for its
 *       codes.
 */
#define XM_UTIL_ERRSPACE_SIZE 20000
/**
 * XM_OS_START_STATUS is where the GW specific status codes start.
 */
#define XM_OS_START_STATUS    (XM_OS_START_ERROR + XM_OS_ERRSPACE_SIZE)
/**
 * XM_UTIL_START_STATUS is where GW-Util starts defining its
 * status codes.
 */
#define XM_UTIL_START_STATUS   (XM_OS_START_STATUS + \
                           (XM_OS_ERRSPACE_SIZE - XM_UTIL_ERRSPACE_SIZE))
/**
 * XM_OS_START_USERERR are reserved for applications that use GW that
 *     layer their own error codes along with GW's.  Note that the
 *     error immediately following this one is set ten times farther
 *     away than usual, so that users of gw have a lot of room in
 *     which to declare custom error codes.
 *
 * In general applications should try and create unique error codes. To try
 * and assist in finding suitable ranges of numbers to use, the following
 * ranges are known to be used by the listed applications. If your
 * application defines error codes please advise the range of numbers it
 * uses to dev@gw.apache.org for inclusion in this list.
 *
 * Ranges shown are in relation to XM_OS_START_USERERR
 *
 * Subversion - Defined ranges, of less than 100, at intervals of 5000
 *              starting at an offset of 5000, e.g.
 *               +5000 to 5100,  +10000 to 10100
 *
 * Apache HTTPD - +2000 to 2999
 */
#define XM_OS_START_USERERR    (XM_OS_START_STATUS + XM_OS_ERRSPACE_SIZE)
/**
 * XM_OS_START_USEERR is obsolete, defined for compatibility only.
 * Use XM_OS_START_USERERR instead.
 */
#define XM_OS_START_USEERR     XM_OS_START_USERERR
/**
 * XM_OS_START_CANONERR is where GW versions of errno values are defined
 *     on systems which don't have the corresponding errno.
 */
#define XM_OS_START_CANONERR  (XM_OS_START_USERERR \
                                 + (XM_OS_ERRSPACE_SIZE * 10))
/**
 * XM_OS_START_EAIERR folds EAI_ error codes from getaddrinfo() into
 *     xm_status_t values.
 */
#define XM_OS_START_EAIERR    (XM_OS_START_CANONERR + XM_OS_ERRSPACE_SIZE)
/**
 * XM_OS_START_SYSERR folds platform-specific system error values into
 *     xm_status_t values.
 */
#define XM_OS_START_SYSERR    (XM_OS_START_EAIERR + XM_OS_ERRSPACE_SIZE)

/**
 * @defgroup XM_ERROR_map GW Error Space
 * <PRE>
 * The following attempts to show the relation of the various constants
 * used for mapping GW Status codes.
 *
 *       0
 *
 *  20,000     XM_OS_START_ERROR
 *
 *         + XM_OS_ERRSPACE_SIZE (50,000)
 *
 *  70,000      XM_OS_START_STATUS
 *
 *         + XM_OS_ERRSPACE_SIZE - XM_UTIL_ERRSPACE_SIZE (30,000)
 *
 * 100,000      XM_UTIL_START_STATUS
 *
 *         + XM_UTIL_ERRSPACE_SIZE (20,000)
 *
 * 120,000      XM_OS_START_USERERR
 *
 *         + 10 x XM_OS_ERRSPACE_SIZE (50,000 * 10)
 *
 * 620,000      XM_OS_START_CANONERR
 *
 *         + XM_OS_ERRSPACE_SIZE (50,000)
 *
 * 670,000      XM_OS_START_EAIERR
 *
 *         + XM_OS_ERRSPACE_SIZE (50,000)
 *
 * 720,000      XM_OS_START_SYSERR
 *
 * </PRE>
 */

/** no error. */
#define XM_OK 0

/**
 * @defgroup XM_Error GW Error Values
 * <PRE>
 * <b>GW ERROR VALUES</b>
 * XM_ENOSTAT      GW was unable to perform a stat on the file
 * XM_ENOPOOL      GW was not provided a pool with which to allocate memory
 * XM_EBADDATE     GW was given an invalid date
 * XM_EINVALSOCK   GW was given an invalid socket
 * XM_ENOPROC      GW was not given a process structure
 * XM_ENOTIME      GW was not given a time structure
 * XM_ENODIR       GW was not given a directory structure
 * XM_ENOLOCK      GW was not given a lock structure
 * XM_ENOPOLL      GW was not given a poll structure
 * XM_ENOSOCKET    GW was not given a socket
 * XM_ENOTHREAD    GW was not given a thread structure
 * XM_ENOTHDKEY    GW was not given a thread key structure
 * XM_ENOSHMAVAIL  There is no more shared memory available
 * XM_EDSOOPEN     GW was unable to open the dso object.  For more
 *                  information call xm_dso_error().
 * XM_EGENERAL     General failure (specific information not available)
 * XM_EBADIP       The specified IP address is invalid
 * XM_EBADMASK     The specified netmask is invalid
 * XM_ESYMNOTFOUND Could not find the requested symbol
 * XM_ENOTENOUGHENTROPY Not enough entropy to continue
 * </PRE>
 *
 * <PRE>
 * <b>GW STATUS VALUES</b>
 * XM_INCHILD        Program is currently executing in the child
 * XM_INPARENT       Program is currently executing in the parent
 * XM_DETACH         The thread is detached
 * XM_NOTDETACH      The thread is not detached
 * XM_CHILD_DONE     The child has finished executing
 * XM_CHILD_NOTDONE  The child has not finished executing
 * XM_TIMEUP         The operation did not finish before the timeout
 * XM_INCOMPLETE     The operation was incomplete although some processing
 *                    was performed and the results are partially valid
 * XM_BADCH          Getopt found an option not in the option string
 * XM_BADARG         Getopt found an option that is missing an argument
 *                    and an argument was specified in the option string
 * XM_EOF            GW has encountered the end of the file
 * XM_NOTFOUND       GW was unable to find the socket in the poll structure
 * XM_ANONYMOUS      GW is using anonymous shared memory
 * XM_FILEBASED      GW is using a file name as the key to the shared memory
 * XM_KEYBASED       GW is using a shared key as the key to the shared memory
 * XM_EINIT          Ininitalizer value.  If no option has been found, but
 *                    the status variable requires a value, this should be used
 * XM_ENOTIMPL       The GW function has not been implemented on this
 *                    platform, either because nobody has gotten to it yet,
 *                    or the function is impossible on this platform.
 * XM_EMISMATCH      Two passwords do not match.
 * XM_EABSOLUTE      The given path was absolute.
 * XM_ERELATIVE      The given path was relative.
 * XM_EINCOMPLETE    The given path was neither relative nor absolute.
 * XM_EABOVEROOT     The given path was above the root path.
 * XM_EBUSY          The given lock was busy.
 * XM_EPROC_UNKNOWN  The given process wasn't recognized by GW
 * </PRE>
 * @{
 */
/** @see XM_STATUS_IS_ENOSTAT */
#define XM_ENOSTAT        (XM_OS_START_ERROR + 1)
/** @see XM_STATUS_IS_ENOPOOL */
#define XM_ENOPOOL        (XM_OS_START_ERROR + 2)
/* empty slot: +3 */
/** @see XM_STATUS_IS_EBADDATE */
#define XM_EBADDATE       (XM_OS_START_ERROR + 4)
/** @see XM_STATUS_IS_EINVALSOCK */
#define XM_EINVALSOCK     (XM_OS_START_ERROR + 5)
/** @see XM_STATUS_IS_ENOPROC */
#define XM_ENOPROC        (XM_OS_START_ERROR + 6)
/** @see XM_STATUS_IS_ENOTIME */
#define XM_ENOTIME        (XM_OS_START_ERROR + 7)
/** @see XM_STATUS_IS_ENODIR */
#define XM_ENODIR         (XM_OS_START_ERROR + 8)
/** @see XM_STATUS_IS_ENOLOCK */
#define XM_ENOLOCK        (XM_OS_START_ERROR + 9)
/** @see XM_STATUS_IS_ENOPOLL */
#define XM_ENOPOLL        (XM_OS_START_ERROR + 10)
/** @see XM_STATUS_IS_ENOSOCKET */
#define XM_ENOSOCKET      (XM_OS_START_ERROR + 11)
/** @see XM_STATUS_IS_ENOTHREAD */
#define XM_ENOTHREAD      (XM_OS_START_ERROR + 12)
/** @see XM_STATUS_IS_ENOTHDKEY */
#define XM_ENOTHDKEY      (XM_OS_START_ERROR + 13)
/** @see XM_STATUS_IS_EGENERAL */
#define XM_EGENERAL       (XM_OS_START_ERROR + 14)
/** @see XM_STATUS_IS_ENOSHMAVAIL */
#define XM_ENOSHMAVAIL    (XM_OS_START_ERROR + 15)
/** @see XM_STATUS_IS_EBADIP */
#define XM_EBADIP         (XM_OS_START_ERROR + 16)
/** @see XM_STATUS_IS_EBADMASK */
#define XM_EBADMASK       (XM_OS_START_ERROR + 17)
/* empty slot: +18 */
/** @see XM_STATUS_IS_EDSOPEN */
#define XM_EDSOOPEN       (XM_OS_START_ERROR + 19)
/** @see XM_STATUS_IS_EABSOLUTE */
#define XM_EABSOLUTE      (XM_OS_START_ERROR + 20)
/** @see XM_STATUS_IS_ERELATIVE */
#define XM_ERELATIVE      (XM_OS_START_ERROR + 21)
/** @see XM_STATUS_IS_EINCOMPLETE */
#define XM_EINCOMPLETE    (XM_OS_START_ERROR + 22)
/** @see XM_STATUS_IS_EABOVEROOT */
#define XM_EABOVEROOT     (XM_OS_START_ERROR + 23)
/** @see XM_STATUS_IS_EBADPATH */
#define XM_EBADPATH       (XM_OS_START_ERROR + 24)
/** @see XM_STATUS_IS_EPATHWILD */
#define XM_EPATHWILD      (XM_OS_START_ERROR + 25)
/** @see XM_STATUS_IS_ESYMNOTFOUND */
#define XM_ESYMNOTFOUND   (XM_OS_START_ERROR + 26)
/** @see XM_STATUS_IS_EPROC_UNKNOWN */
#define XM_EPROC_UNKNOWN  (XM_OS_START_ERROR + 27)
/** @see XM_STATUS_IS_ENOTENOUGHENTROPY */
#define XM_ENOTENOUGHENTROPY (XM_OS_START_ERROR + 28)
/** @} */

/**
 * @defgroup XM_STATUS_IS Status Value Tests
 * @warning For any particular error condition, more than one of these tests
 *      may match. This is because platform-specific error codes may not
 *      always match the semantics of the POSIX codes these tests (and the
 *      corresponding GW error codes) are named after. A notable example
 *      are the XM_STATUS_IS_ENOENT and XM_STATUS_IS_ENOTDIR tests on
 *      Win32 platforms. The programmer should always be aware of this and
 *      adjust the order of the tests accordingly.
 * @{
 */
/**
 * GW was unable to perform a stat on the file
 * @warning always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_ENOSTAT(s)        ((s) == XM_ENOSTAT)
/**
 * GW was not provided a pool with which to allocate memory
 * @warning always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_ENOPOOL(s)        ((s) == XM_ENOPOOL)
/** GW was given an invalid date  */
#define XM_STATUS_IS_EBADDATE(s)       ((s) == XM_EBADDATE)
/** GW was given an invalid socket */
#define XM_STATUS_IS_EINVALSOCK(s)     ((s) == XM_EINVALSOCK)
/** GW was not given a process structure */
#define XM_STATUS_IS_ENOPROC(s)        ((s) == XM_ENOPROC)
/** GW was not given a time structure */
#define XM_STATUS_IS_ENOTIME(s)        ((s) == XM_ENOTIME)
/** GW was not given a directory structure */
#define XM_STATUS_IS_ENODIR(s)         ((s) == XM_ENODIR)
/** GW was not given a lock structure */
#define XM_STATUS_IS_ENOLOCK(s)        ((s) == XM_ENOLOCK)
/** GW was not given a poll structure */
#define XM_STATUS_IS_ENOPOLL(s)        ((s) == XM_ENOPOLL)
/** GW was not given a socket */
#define XM_STATUS_IS_ENOSOCKET(s)      ((s) == XM_ENOSOCKET)
/** GW was not given a thread structure */
#define XM_STATUS_IS_ENOTHREAD(s)      ((s) == XM_ENOTHREAD)
/** GW was not given a thread key structure */
#define XM_STATUS_IS_ENOTHDKEY(s)      ((s) == XM_ENOTHDKEY)
/** Generic Error which can not be put into another spot */
#define XM_STATUS_IS_EGENERAL(s)       ((s) == XM_EGENERAL)
/** There is no more shared memory available */
#define XM_STATUS_IS_ENOSHMAVAIL(s)    ((s) == XM_ENOSHMAVAIL)
/** The specified IP address is invalid */
#define XM_STATUS_IS_EBADIP(s)         ((s) == XM_EBADIP)
/** The specified netmask is invalid */
#define XM_STATUS_IS_EBADMASK(s)       ((s) == XM_EBADMASK)
/* empty slot: +18 */
/**
 * GW was unable to open the dso object.
 * For more information call xm_dso_error().
 */
#define XM_STATUS_IS_EDSOOPEN(s)       ((s) == XM_EDSOOPEN)
/** The given path was absolute. */
#define XM_STATUS_IS_EABSOLUTE(s)      ((s) == XM_EABSOLUTE)
/** The given path was relative. */
#define XM_STATUS_IS_ERELATIVE(s)      ((s) == XM_ERELATIVE)
/** The given path was neither relative nor absolute. */
#define XM_STATUS_IS_EINCOMPLETE(s)    ((s) == XM_EINCOMPLETE)
/** The given path was above the root path. */
#define XM_STATUS_IS_EABOVEROOT(s)     ((s) == XM_EABOVEROOT)
/** The given path was bad. */
#define XM_STATUS_IS_EBADPATH(s)       ((s) == XM_EBADPATH)
/** The given path contained wildcards. */
#define XM_STATUS_IS_EPATHWILD(s)      ((s) == XM_EPATHWILD)
/** Could not find the requested symbol.
 * For more information call xm_dso_error().
 */
#define XM_STATUS_IS_ESYMNOTFOUND(s)   ((s) == XM_ESYMNOTFOUND)
/** The given process was not recognized by GW. */
#define XM_STATUS_IS_EPROC_UNKNOWN(s)  ((s) == XM_EPROC_UNKNOWN)
/** GW could not gather enough entropy to continue. */
#define XM_STATUS_IS_ENOTENOUGHENTROPY(s) ((s) == XM_ENOTENOUGHENTROPY)

/** @} */

/**
 * @addtogroup XM_Error
 * @{
 */
/** @see XM_STATUS_IS_INCHILD */
#define XM_INCHILD        (XM_OS_START_STATUS + 1)
/** @see XM_STATUS_IS_INPARENT */
#define XM_INPARENT       (XM_OS_START_STATUS + 2)
/** @see XM_STATUS_IS_DETACH */
#define XM_DETACH         (XM_OS_START_STATUS + 3)
/** @see XM_STATUS_IS_NOTDETACH */
#define XM_NOTDETACH      (XM_OS_START_STATUS + 4)
/** @see XM_STATUS_IS_CHILD_DONE */
#define XM_CHILD_DONE     (XM_OS_START_STATUS + 5)
/** @see XM_STATUS_IS_CHILD_NOTDONE */
#define XM_CHILD_NOTDONE  (XM_OS_START_STATUS + 6)
/** @see XM_STATUS_IS_TIMEUP */
#define XM_TIMEUP         (XM_OS_START_STATUS + 7)
/** @see XM_STATUS_IS_INCOMPLETE */
#define XM_INCOMPLETE     (XM_OS_START_STATUS + 8)
/* empty slot: +9 */
/* empty slot: +10 */
/* empty slot: +11 */
/** @see XM_STATUS_IS_BADCH */
#define XM_BADCH          (XM_OS_START_STATUS + 12)
/** @see XM_STATUS_IS_BADARG */
#define XM_BADARG         (XM_OS_START_STATUS + 13)
/** @see XM_STATUS_IS_EOF */
#define XM_EOF            (XM_OS_START_STATUS + 14)
/** @see XM_STATUS_IS_NOTFOUND */
#define XM_NOTFOUND       (XM_OS_START_STATUS + 15)
/* empty slot: +16 */
/* empty slot: +17 */
/* empty slot: +18 */
/** @see XM_STATUS_IS_ANONYMOUS */
#define XM_ANONYMOUS      (XM_OS_START_STATUS + 19)
/** @see XM_STATUS_IS_FILEBASED */
#define XM_FILEBASED      (XM_OS_START_STATUS + 20)
/** @see XM_STATUS_IS_KEYBASED */
#define XM_KEYBASED       (XM_OS_START_STATUS + 21)
/** @see XM_STATUS_IS_EINIT */
#define XM_EINIT          (XM_OS_START_STATUS + 22)
/** @see XM_STATUS_IS_ENOTIMPL */
#define XM_ENOTIMPL       (XM_OS_START_STATUS + 23)
/** @see XM_STATUS_IS_EMISMATCH */
#define XM_EMISMATCH      (XM_OS_START_STATUS + 24)
/** @see XM_STATUS_IS_EBUSY */
#define XM_EBUSY          (XM_OS_START_STATUS + 25)
/** @} */

/**
 * @addtogroup XM_STATUS_IS
 * @{
 */
/**
 * Program is currently executing in the child
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code */
#define XM_STATUS_IS_INCHILD(s)        ((s) == XM_INCHILD)
/**
 * Program is currently executing in the parent
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_INPARENT(s)       ((s) == XM_INPARENT)
/**
 * The thread is detached
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_DETACH(s)         ((s) == XM_DETACH)
/**
 * The thread is not detached
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_NOTDETACH(s)      ((s) == XM_NOTDETACH)
/**
 * The child has finished executing
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_CHILD_DONE(s)     ((s) == XM_CHILD_DONE)
/**
 * The child has not finished executing
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_CHILD_NOTDONE(s)  ((s) == XM_CHILD_NOTDONE)
/**
 * The operation did not finish before the timeout
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_TIMEUP(s)         ((s) == XM_TIMEUP)
/**
 * The operation was incomplete although some processing was performed
 * and the results are partially valid.
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_INCOMPLETE(s)     ((s) == XM_INCOMPLETE)
/* empty slot: +9 */
/* empty slot: +10 */
/* empty slot: +11 */
/**
 * Getopt found an option not in the option string
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_BADCH(s)          ((s) == XM_BADCH)
/**
 * Getopt found an option not in the option string and an argument was
 * specified in the option string
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_BADARG(s)         ((s) == XM_BADARG)
/**
 * GW has encountered the end of the file
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_EOF(s)            ((s) == XM_EOF)
/**
 * GW was unable to find the socket in the poll structure
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_NOTFOUND(s)       ((s) == XM_NOTFOUND)
/* empty slot: +16 */
/* empty slot: +17 */
/* empty slot: +18 */
/**
 * GW is using anonymous shared memory
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_ANONYMOUS(s)      ((s) == XM_ANONYMOUS)
/**
 * GW is using a file name as the key to the shared memory
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_FILEBASED(s)      ((s) == XM_FILEBASED)
/**
 * GW is using a shared key as the key to the shared memory
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_KEYBASED(s)       ((s) == XM_KEYBASED)
/**
 * Ininitalizer value.  If no option has been found, but
 * the status variable requires a value, this should be used
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_EINIT(s)          ((s) == XM_EINIT)
/**
 * The GW function has not been implemented on this
 * platform, either because nobody has gotten to it yet,
 * or the function is impossible on this platform.
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_ENOTIMPL(s)       ((s) == XM_ENOTIMPL)
/**
 * Two passwords do not match.
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_EMISMATCH(s)      ((s) == XM_EMISMATCH)
/**
 * The given lock was busy
 * @warning always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define XM_STATUS_IS_EBUSY(s)          ((s) == XM_EBUSY)

/** @} */

/**
 * @addtogroup XM_Error GW Error Values
 * @{
 */
/* GW CANONICAL ERROR VALUES */
/** @see XM_STATUS_IS_EACCES */
#ifdef EACCES
#define XM_EACCES EACCES
#else
#define XM_EACCES         (XM_OS_START_CANONERR + 1)
#endif

/** @see XM_STATUS_IS_EEXIST */
#ifdef EEXIST
#define XM_EEXIST EEXIST
#else
#define XM_EEXIST         (XM_OS_START_CANONERR + 2)
#endif

/** @see XM_STATUS_IS_ENAMETOOLONG */
#ifdef ENAMETOOLONG
#define XM_ENAMETOOLONG ENAMETOOLONG
#else
#define XM_ENAMETOOLONG   (XM_OS_START_CANONERR + 3)
#endif

/** @see XM_STATUS_IS_ENOENT */
#ifdef ENOENT
#define XM_ENOENT ENOENT
#else
#define XM_ENOENT         (XM_OS_START_CANONERR + 4)
#endif

/** @see XM_STATUS_IS_ENOTDIR */
#ifdef ENOTDIR
#define XM_ENOTDIR ENOTDIR
#else
#define XM_ENOTDIR        (XM_OS_START_CANONERR + 5)
#endif

/** @see XM_STATUS_IS_ENOSPC */
#ifdef ENOSPC
#define XM_ENOSPC ENOSPC
#else
#define XM_ENOSPC         (XM_OS_START_CANONERR + 6)
#endif

/** @see XM_STATUS_IS_ENOMEM */
#ifdef ENOMEM
#define XM_ENOMEM ENOMEM
#else
#define XM_ENOMEM         (XM_OS_START_CANONERR + 7)
#endif

/** @see XM_STATUS_IS_EMFILE */
#ifdef EMFILE
#define XM_EMFILE EMFILE
#else
#define XM_EMFILE         (XM_OS_START_CANONERR + 8)
#endif

/** @see XM_STATUS_IS_ENFILE */
#ifdef ENFILE
#define XM_ENFILE ENFILE
#else
#define XM_ENFILE         (XM_OS_START_CANONERR + 9)
#endif

/** @see XM_STATUS_IS_EBADF */
#ifdef EBADF
#define XM_EBADF EBADF
#else
#define XM_EBADF          (XM_OS_START_CANONERR + 10)
#endif

/** @see XM_STATUS_IS_EINVAL */
#ifdef EINVAL
#define XM_EINVAL EINVAL
#else
#define XM_EINVAL         (XM_OS_START_CANONERR + 11)
#endif

/** @see XM_STATUS_IS_ESPIPE */
#ifdef ESPIPE
#define XM_ESPIPE ESPIPE
#else
#define XM_ESPIPE         (XM_OS_START_CANONERR + 12)
#endif

/**
 * @see XM_STATUS_IS_EAGAIN
 * @warning use XM_STATUS_IS_EAGAIN instead of just testing this value
 */
#ifdef EAGAIN
#define XM_EAGAIN EAGAIN
#elif defined(EWOULDBLOCK)
#define XM_EAGAIN EWOULDBLOCK
#else
#define XM_EAGAIN         (XM_OS_START_CANONERR + 13)
#endif

/** @see XM_STATUS_IS_EINTR */
#ifdef EINTR
#define XM_EINTR EINTR
#else
#define XM_EINTR          (XM_OS_START_CANONERR + 14)
#endif

/** @see XM_STATUS_IS_ENOTSOCK */
#ifdef ENOTSOCK
#define XM_ENOTSOCK ENOTSOCK
#else
#define XM_ENOTSOCK       (XM_OS_START_CANONERR + 15)
#endif

/** @see XM_STATUS_IS_ECONNREFUSED */
#ifdef ECONNREFUSED
#define XM_ECONNREFUSED ECONNREFUSED
#else
#define XM_ECONNREFUSED   (XM_OS_START_CANONERR + 16)
#endif

/** @see XM_STATUS_IS_EINPROGRESS */
#ifdef EINPROGRESS
#define XM_EINPROGRESS EINPROGRESS
#else
#define XM_EINPROGRESS    (XM_OS_START_CANONERR + 17)
#endif

/**
 * @see XM_STATUS_IS_ECONNABORTED
 * @warning use XM_STATUS_IS_ECONNABORTED instead of just testing this value
 */

#ifdef ECONNABORTED
#define XM_ECONNABORTED ECONNABORTED
#else
#define XM_ECONNABORTED   (XM_OS_START_CANONERR + 18)
#endif

/** @see XM_STATUS_IS_ECONNRESET */
#ifdef ECONNRESET
#define XM_ECONNRESET ECONNRESET
#else
#define XM_ECONNRESET     (XM_OS_START_CANONERR + 19)
#endif

/** @see XM_STATUS_IS_ETIMEDOUT
 *  @deprecated */
#ifdef ETIMEDOUT
#define XM_ETIMEDOUT ETIMEDOUT
#else
#define XM_ETIMEDOUT      (XM_OS_START_CANONERR + 20)
#endif

/** @see XM_STATUS_IS_EHOSTUNREACH */
#ifdef EHOSTUNREACH
#define XM_EHOSTUNREACH EHOSTUNREACH
#else
#define XM_EHOSTUNREACH   (XM_OS_START_CANONERR + 21)
#endif

/** @see XM_STATUS_IS_ENETUNREACH */
#ifdef ENETUNREACH
#define XM_ENETUNREACH ENETUNREACH
#else
#define XM_ENETUNREACH    (XM_OS_START_CANONERR + 22)
#endif

/** @see XM_STATUS_IS_EFTYPE */
#ifdef EFTYPE
#define XM_EFTYPE EFTYPE
#else
#define XM_EFTYPE        (XM_OS_START_CANONERR + 23)
#endif

/** @see XM_STATUS_IS_EPIPE */
#ifdef EPIPE
#define XM_EPIPE EPIPE
#else
#define XM_EPIPE         (XM_OS_START_CANONERR + 24)
#endif

/** @see XM_STATUS_IS_EXDEV */
#ifdef EXDEV
#define XM_EXDEV EXDEV
#else
#define XM_EXDEV         (XM_OS_START_CANONERR + 25)
#endif

/** @see XM_STATUS_IS_ENOTEMPTY */
#ifdef ENOTEMPTY
#define XM_ENOTEMPTY ENOTEMPTY
#else
#define XM_ENOTEMPTY     (XM_OS_START_CANONERR + 26)
#endif

/** @see XM_STATUS_IS_EAFNOSUPPORT */
#ifdef EAFNOSUPPORT
#define XM_EAFNOSUPPORT EAFNOSUPPORT
#else
#define XM_EAFNOSUPPORT  (XM_OS_START_CANONERR + 27)
#endif

/** @} */

#define XM_FROM_OS_ERROR(e)  (e)
#define XM_TO_OS_ERROR(e)    (e)

#define xm_get_os_error()    (errno)
#define xm_set_os_error(e)   (errno = (e))

/* A special case, only socket calls require this:
 */
#define xm_get_netos_error() (errno)
#define xm_set_netos_error(e) (errno = (e))

/**
 * @addtogroup XM_STATUS_IS
 * @{
 */

/** permission denied */
#define XM_STATUS_IS_EACCES(s)         ((s) == XM_EACCES)
/** file exists */
#define XM_STATUS_IS_EEXIST(s)         ((s) == XM_EEXIST)
/** path name is too long */
#define XM_STATUS_IS_ENAMETOOLONG(s)   ((s) == XM_ENAMETOOLONG)
/**
 * no such file or directory
 * @remark
 * EMVSCATLG can be returned by the automounter on z/OS for
 * paths which do not exist.
 */
#ifdef EMVSCATLG
#define XM_STATUS_IS_ENOENT(s)         ((s) == XM_ENOENT \
                                      || (s) == EMVSCATLG)
#else
#define XM_STATUS_IS_ENOENT(s)         ((s) == XM_ENOENT)
#endif
/** not a directory */
#define XM_STATUS_IS_ENOTDIR(s)        ((s) == XM_ENOTDIR)
/** no space left on device */
#ifdef EDQUOT
#define XM_STATUS_IS_ENOSPC(s)         ((s) == XM_ENOSPC \
                                      || (s) == EDQUOT)
#else
#define XM_STATUS_IS_ENOSPC(s)         ((s) == XM_ENOSPC)
#endif
/** not enough memory */
#define XM_STATUS_IS_ENOMEM(s)         ((s) == XM_ENOMEM)
/** too many open files */
#define XM_STATUS_IS_EMFILE(s)         ((s) == XM_EMFILE)
/** file table overflow */
#define XM_STATUS_IS_ENFILE(s)         ((s) == XM_ENFILE)
/** bad file # */
#define XM_STATUS_IS_EBADF(s)          ((s) == XM_EBADF)
/** invalid argument */
#define XM_STATUS_IS_EINVAL(s)         ((s) == XM_EINVAL)
/** illegal seek */
#define XM_STATUS_IS_ESPIPE(s)         ((s) == XM_ESPIPE)

/** operation would block */
#if !defined(EWOULDBLOCK) || !defined(EAGAIN)
#define XM_STATUS_IS_EAGAIN(s)         ((s) == XM_EAGAIN)
#elif (EWOULDBLOCK == EAGAIN)
#define XM_STATUS_IS_EAGAIN(s)         ((s) == XM_EAGAIN)
#else
#define XM_STATUS_IS_EAGAIN(s)         ((s) == XM_EAGAIN \
                                      || (s) == EWOULDBLOCK)
#endif

/** interrupted system call */
#define XM_STATUS_IS_EINTR(s)          ((s) == XM_EINTR)
/** socket operation on a non-socket */
#define XM_STATUS_IS_ENOTSOCK(s)       ((s) == XM_ENOTSOCK)
/** Connection Refused */
#define XM_STATUS_IS_ECONNREFUSED(s)   ((s) == XM_ECONNREFUSED)
/** operation now in progress */
#define XM_STATUS_IS_EINPROGRESS(s)    ((s) == XM_EINPROGRESS)

/**
 * Software caused connection abort
 * @remark
 * EPROTO on certain older kernels really means ECONNABORTED, so we need to
 * ignore it for them.  See discussion in new-httpd archives nh.9701 & nh.9603
 *
 * There is potentially a bug in Solaris 2.x x<6, and other boxes that
 * implement tcp sockets in userland (i.e. on top of STREAMS).  On these
 * systems, EPROTO can actually result in a fatal loop.  See PR#981 for
 * example.  It's hard to handle both uses of EPROTO.
 */
#ifdef EPROTO
#define XM_STATUS_IS_ECONNABORTED(s)    ((s) == XM_ECONNABORTED \
                                       || (s) == EPROTO)
#else
#define XM_STATUS_IS_ECONNABORTED(s)    ((s) == XM_ECONNABORTED)
#endif

/** Connection Reset by peer */
#define XM_STATUS_IS_ECONNRESET(s)      ((s) == XM_ECONNRESET)
/** Operation timed out
 *  @deprecated */
#define XM_STATUS_IS_ETIMEDOUT(s)      ((s) == XM_ETIMEDOUT)
/** no route to host */
#define XM_STATUS_IS_EHOSTUNREACH(s)    ((s) == XM_EHOSTUNREACH)
/** network is unreachable */
#define XM_STATUS_IS_ENETUNREACH(s)     ((s) == XM_ENETUNREACH)
/** inappropiate file type or format */
#define XM_STATUS_IS_EFTYPE(s)          ((s) == XM_EFTYPE)
/** broken pipe */
#define XM_STATUS_IS_EPIPE(s)           ((s) == XM_EPIPE)
/** cross device link */
#define XM_STATUS_IS_EXDEV(s)           ((s) == XM_EXDEV)
/** Directory Not Empty */
#define XM_STATUS_IS_ENOTEMPTY(s)       ((s) == XM_ENOTEMPTY || \
                                          (s) == XM_EEXIST)
/** Address Family not supported */
#define XM_STATUS_IS_EAFNOSUPPORT(s)    ((s) == XM_EAFNOSUPPORT)
/** @} */


#endif /* XM_ERRNO_H */
