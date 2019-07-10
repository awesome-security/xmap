/*
 *
 *      Filename: xm_errno.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 10:35:16
 * Last Modified: 2017-01-11 10:46:51
 */

#include "xm_errno.h"
#include "xm_string.h"
/*
 * stuffbuffer - like xm_cpystrn() but returns the address of the
 * dest buffer instead of the address of the terminating '\0'
 */
static char *stuffbuffer(char *buf, size_t bufsize, const char *s)
{
    xm_cpystrn(buf,s,bufsize);
    return buf;
}

static char *xm_error_string(int statcode)
{
    switch (statcode) {
    case XM_ENOSTAT:
        return "Could not perform a stat on the file.";
    case XM_ENOPOOL:
        return "A new pool could not be created.";
    case XM_EBADDATE:
        return "An invalid date has been provided";
    case XM_EINVALSOCK:
        return "An invalid socket was returned";
    case XM_ENOPROC:
        return "No process was provided and one was required.";
    case XM_ENOTIME:
        return "No time was provided and one was required.";
    case XM_ENODIR:
        return "No directory was provided and one was required.";
    case XM_ENOLOCK:
        return "No lock was provided and one was required.";
    case XM_ENOPOLL:
        return "No poll structure was provided and one was required.";
    case XM_ENOSOCKET:
        return "No socket was provided and one was required.";
    case XM_ENOTHREAD:
        return "No thread was provided and one was required.";
    case XM_ENOTHDKEY:
        return "No thread key structure was provided and one was required.";
    case XM_ENOSHMAVAIL:
        return "No shared memory is currently available";
    case XM_EDSOOPEN:
        return "DSO load failed";
    case XM_EBADIP:
        return "The specified IP address is invalid.";
    case XM_EBADMASK:
        return "The specified network mask is invalid.";
    case XM_ESYMNOTFOUND:
        return "Could not find the requested symbol.";
    case XM_ENOTENOUGHENTROPY:
        return "Not enough entropy to continue.";
    case XM_INCHILD:
        return
	    "Your code just forked, and you are currently executing in the "
	    "child process";
    case XM_INPARENT:
        return
	    "Your code just forked, and you are currently executing in the "
	    "parent process";
    case XM_DETACH:
        return "The specified thread is detached";
    case XM_NOTDETACH:
        return "The specified thread is not detached";
    case XM_CHILD_DONE:
        return "The specified child process is done executing";
    case XM_CHILD_NOTDONE:
        return "The specified child process is not done executing";
    case XM_TIMEUP:
        return "The timeout specified has expired";
    case XM_INCOMPLETE:
        return "Partial results are valid but processing is incomplete";
    case XM_BADCH:
        return "Bad character specified on command line";
    case XM_BADARG:
        return "Missing parameter for the specified command line option";
    case XM_EOF:
        return "End of file found";
    case XM_NOTFOUND:
        return "Could not find specified socket in poll list.";
    case XM_ANONYMOUS:
        return "Shared memory is implemented anonymously";
    case XM_FILEBASED:
        return "Shared memory is implemented using files";
    case XM_KEYBASED:
        return "Shared memory is implemented using a key system";
    case XM_EINIT:
        return
	    "There is no error, this value signifies an initialized "
	    "error code";
    case XM_ENOTIMPL:
        return "This function has not been implemented on this platform";
    case XM_EMISMATCH:
        return "passwords do not match";
    case XM_EABSOLUTE:
        return "The given path is absolute";
    case XM_ERELATIVE:
        return "The given path is relative";
    case XM_EINCOMPLETE:
        return "The given path is incomplete";
    case XM_EABOVEROOT:
        return "The given path was above the root path";
    case XM_EBADPATH:
        return "The given path is misformatted or contained invalid characters";
    case XM_EPATHWILD:
        return "The given path contained wildcard characters";
    case XM_EBUSY:
        return "The given lock was busy.";
    case XM_EPROC_UNKNOWN:
        return "The process is not recognized.";
    case XM_EGENERAL:
        return "Internal error (specific information not available)";
    default:
        return "Error string not specified yet";
    }
}



/* On Unix, xm_os_strerror() handles error codes from the resolver 
 * (h_errno). 
 */
static char *xm_os_strerror(char* buf, size_t bufsize, int err) 
{
    const char *msg = "";
	err = err;

    return stuffbuffer(buf, bufsize, msg);
}


/* plain old strerror(); 
 * thread-safe on some platforms (e.g., Solaris, OS/390)
 */
static char *native_strerror(int statcode, char *buf,
                             size_t bufsize)
{
    const char *err = strerror(statcode);
    if (err) {
        return stuffbuffer(buf, bufsize, err);
    } else {
        return stuffbuffer(buf, bufsize, 
                           "GW does not understand this error code");
    }
}

char * xm_strerror(int statcode, char *buf,
                                 size_t bufsize)
{
    if (statcode < XM_OS_START_ERROR) {
        return native_strerror(statcode, buf, bufsize);
    }
    else if (statcode < XM_OS_START_USERERR) {
        return stuffbuffer(buf, bufsize, xm_error_string(statcode));
    }
    else if (statcode < XM_OS_START_EAIERR) {
        return stuffbuffer(buf, bufsize, "GW does not understand this error code");
    }
    else if (statcode < XM_OS_START_SYSERR) {
#if defined(HAVE_GAI_STRERROR)
        statcode -= XM_OS_START_EAIERR;
#if defined(NEGATIVE_EAI)
        statcode = -statcode;
#endif
        return stuffbuffer(buf, bufsize, gai_strerror(statcode));
#else
        return stuffbuffer(buf, bufsize, "GW does not understand this error code");
#endif
    }
    else {
        return xm_os_strerror(buf, bufsize, statcode - XM_OS_START_SYSERR);
    }
}


