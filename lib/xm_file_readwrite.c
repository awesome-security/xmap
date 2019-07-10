/*
 *
 *      Filename: xm_file_readwrite.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 09:56:42
 * Last Modified: 2017-01-11 12:14:08
 */
#include "xm_file.h"
#include "xm_errno.h"
#include "xm_string.h"

static int file_read_buffered(xm_file_t *thefile, void *buf,
                                       size_t *nbytes)
{
    ssize_t rv;
    char *pos = (char *)buf;
    uint64_t blocksize;
    uint64_t size = *nbytes;

    if (thefile->direction == 1) {
        rv = xm_file_flush_locked(thefile);
        if (rv) {
            return rv;
        }
        thefile->bufpos = 0;
        thefile->direction = 0;
        thefile->dataRead = 0;
    }

    rv = 0;
    if (thefile->ungetchar != -1) {
        *pos = (char)thefile->ungetchar;
        ++pos;
        --size;
        thefile->ungetchar = -1;
    }
    while (rv == 0 && size > 0) {
        if (thefile->bufpos >= thefile->dataRead) {
            int bytesread = read(thefile->filedes, thefile->buffer, 
                                 thefile->bufsize);
            if (bytesread == 0) {
                thefile->eof_hit = 1;
                rv = XM_EOF;
                break;
            }
            else if (bytesread == -1) {
                rv = errno;
                break;
            }
            thefile->dataRead = bytesread;
            thefile->filePtr += thefile->dataRead;
            thefile->bufpos = 0;
        }

        blocksize = size > thefile->dataRead - thefile->bufpos ? thefile->dataRead - thefile->bufpos : size;
        memcpy(pos, thefile->buffer + thefile->bufpos, blocksize);
        thefile->bufpos += blocksize;
        pos += blocksize;
        size -= blocksize;
    }

    *nbytes = pos - (char *)buf;
    if (*nbytes) {
        rv = 0;
    }
    return rv;
}

int xm_file_read(xm_file_t *thefile, void *buf, size_t *nbytes)
{
    ssize_t rv;
    size_t bytes_read;

    if (*nbytes <= 0) {
        *nbytes = 0;
        return 0;
    }

    if (thefile->buffered) {
        rv = file_read_buffered(thefile, buf, nbytes);
        return rv;
    }
    else {
        bytes_read = 0;
        if (thefile->ungetchar != -1) {
            bytes_read = 1;
            *(char *)buf = (char)thefile->ungetchar;
            buf = (char *)buf + 1;
            (*nbytes)--;
            thefile->ungetchar = -1;
            if (*nbytes == 0) {
                *nbytes = bytes_read;
                return 0;
            }
        }

        do {
            rv = read(thefile->filedes, buf, *nbytes);
        } while (rv == -1 && errno == EINTR);

        *nbytes = bytes_read;
        if (rv == 0) {
            thefile->eof_hit = 1;
            return XM_EOF;
        }
        if (rv > 0) {
            *nbytes += rv;
            return 0;
        }
        return errno;
    }
}

int xm_file_write(xm_file_t *thefile, const void *buf, size_t *nbytes)
{
    size_t rv;

    if (thefile->buffered) {
        char *pos = (char *)buf;
        size_t blocksize;
        size_t size = *nbytes;


        if ( thefile->direction == 0 ) {
            /* Position file pointer for writing at the offset we are 
             * logically reading from
             */
            int64_t offset = thefile->filePtr - thefile->dataRead + thefile->bufpos;
            if (offset != thefile->filePtr)
                lseek(thefile->filedes, offset, SEEK_SET);
            thefile->bufpos = thefile->dataRead = 0;
            thefile->direction = 1;
        }

        rv = 0;
        while (rv == 0 && size > 0) {
            if (thefile->bufpos == thefile->bufsize)   /* write buffer is full*/
                rv = xm_file_flush_locked(thefile);

            blocksize = size > thefile->bufsize - thefile->bufpos ? 
                        thefile->bufsize - thefile->bufpos : size;
            memcpy(thefile->buffer + thefile->bufpos, pos, blocksize);                      
            thefile->bufpos += blocksize;
            pos += blocksize;
            size -= blocksize;
        }

        return rv;
    }
    else {
        do {
            rv = write(thefile->filedes, buf, *nbytes);
        } while (rv == (size_t)-1 && errno == EINTR);

        if (rv == (size_t)-1) {
            (*nbytes) = 0;
            return errno;
        }
        *nbytes = rv;
        return 0;
    }
}

int xm_file_writev(xm_file_t *thefile, const struct iovec *vec,
                                          size_t nvec, size_t *nbytes)
{
    int rv;
    ssize_t bytes;

    if (thefile->buffered) {

        rv = xm_file_flush_locked(thefile);
        if (rv != 0) {
            return rv;
        }
        if (thefile->direction == 0) {
            /* Position file pointer for writing at the offset we are
             * logically reading from
             */
            int64_t offset = thefile->filePtr - thefile->dataRead +
                                 thefile->bufpos;
            if (offset != thefile->filePtr)
                lseek(thefile->filedes, offset, SEEK_SET);
            thefile->bufpos = thefile->dataRead = 0;
        }

    }

    if ((bytes = writev(thefile->filedes, vec, nvec)) < 0) {
        *nbytes = 0;
        rv = errno;
    }
    else {
        *nbytes = bytes;
        rv = 0;
    }
    return rv;

}

int xm_file_putc(char ch, xm_file_t *thefile)
{
    size_t nbytes = 1;

    return xm_file_write(thefile, &ch, &nbytes);
}

int xm_file_ungetc(char ch, xm_file_t *thefile)
{
    thefile->ungetchar = (unsigned char)ch;
    return 0; 
}

int xm_file_getc(char *ch, xm_file_t *thefile)
{
    size_t nbytes = 1;

    return xm_file_read(thefile, ch, &nbytes);
}

int xm_file_puts(const char *str, xm_file_t *thefile)
{
    return xm_file_write_full(thefile, str, strlen(str), NULL);
}

int xm_file_flush_locked(xm_file_t *thefile)
{
    int rv = 0;

    if (thefile->direction == 1 && thefile->bufpos) {
        size_t written = 0;
		ssize_t ret;

        do {
            ret = write(thefile->filedes, thefile->buffer + written,
                        thefile->bufpos - written);
            if (ret > 0)
                written += ret;
        } while (written < thefile->bufpos &&
                 (ret > 0 || (ret == -1 && errno == EINTR)));
        if (ret == -1) {
            rv = errno;
        } else {
            thefile->filePtr += written;
            thefile->bufpos = 0;
        }
    }

    return rv;
}

int xm_file_flush(xm_file_t *thefile)
{
    int rv = 0;

    if (thefile->buffered) {
        rv = xm_file_flush_locked(thefile);
    }
    /* There isn't anything to do if we aren't buffering the output
     * so just return success.
     */
    return rv;
}

int xm_file_sync(xm_file_t *thefile)
{
    int rv = 0;


    if (thefile->buffered) {
        rv = xm_file_flush_locked(thefile);

        if (rv != 0) {
            
            return rv;
        }
    }

    if (fsync(thefile->filedes)) {
        rv = xm_get_os_error();
    }


    return rv;
}

int xm_file_datasync(xm_file_t *thefile)
{
    int rv = 0;


    if (thefile->buffered) {
        rv = xm_file_flush_locked(thefile);

        if (rv != 0) {
            return rv;
        }
    }

    if (fsync(thefile->filedes)) {
        rv = xm_get_os_error();
    }


    return rv;
}

int xm_file_gets(char *str, int len, xm_file_t *thefile)
{
    int rv = 0; /* get rid of gcc warning */
    size_t nbytes;
    const char *str_start = str;
    char *final = str + len - 1;

    if (len <= 1) {  
        /* sort of like fgets(), which returns NULL and stores no bytes 
         */
        return 0;
    }

    /* If we have an underlying buffer, we can be *much* more efficient
     * and skip over the xm_file_read calls.
     */
    if (thefile->buffered) {
   

        if (thefile->direction == 1) {
            rv = xm_file_flush_locked(thefile);
            if (rv) {
         
                return rv;
            }

            thefile->direction = 0;
            thefile->bufpos = 0;
            thefile->dataRead = 0;
        }

        while (str < final) { /* leave room for trailing '\0' */
            /* Force ungetc leftover to call xm_file_read. */
            if (thefile->bufpos < thefile->dataRead &&
                thefile->ungetchar == -1) {
                *str = thefile->buffer[thefile->bufpos++];
            }
            else {
                nbytes = 1;
                rv = file_read_buffered(thefile, str, &nbytes);
                if (rv != 0) {
                    break;
                }
            }
            if (*str == '\n') {
                ++str;
                break;
            }
            ++str;
        }
    }
    else {
        while (str < final) { /* leave room for trailing '\0' */
            nbytes = 1;
            rv = xm_file_read(thefile, str, &nbytes);
            if (rv != 0) {
                break;
            }
            if (*str == '\n') {
                ++str;
                break;
            }
            ++str;
        }
    }

    /* We must store a terminating '\0' if we've stored any chars. We can
     * get away with storing it if we hit an error first. 
     */
    *str = '\0';
    if (str > str_start) {
        /* we stored chars; don't report EOF or any other errors;
         * the app will find out about that on the next call
         */
        return 0;
    }
    return rv;
}

struct xm_file_printf_data {
    xm_vformatter_buff_t vbuff;
    xm_file_t *fptr;
    char *buf;
};

static int file_printf_flush(xm_vformatter_buff_t *buff)
{
    struct xm_file_printf_data *data = (struct xm_file_printf_data *)buff;

    if (xm_file_write_full(data->fptr, data->buf, 
                            data->vbuff.curpos - data->buf, NULL)) {
        return -1;
    }

    data->vbuff.curpos = data->buf;
    return 0;
}

int xm_file_printf(xm_file_t *fptr, 
                                        const char *format, ...)
{
    struct xm_file_printf_data data;
    va_list ap;
    int count;

    /* don't really need a HUGE_STRING_LEN anymore */
    data.buf = malloc(HUGE_STRING_LEN);
    if (data.buf == NULL) {
        return -1;
    }
    data.vbuff.curpos = data.buf;
    data.vbuff.endpos = data.buf + HUGE_STRING_LEN;
    data.fptr = fptr;
    va_start(ap, format);
    count = xm_vformatter(file_printf_flush,
                           (xm_vformatter_buff_t *)&data, format, ap);
    /* xm_vformatter does not call flush for the last bits */
    if (count >= 0) file_printf_flush((xm_vformatter_buff_t *)&data);

    va_end(ap);

    free(data.buf);

    return count;
}

