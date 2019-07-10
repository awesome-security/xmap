/*
 *
 *      Filename: xm_dir.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 12:39:57
 * Last Modified: 2017-01-11 13:08:59
 */

#include "xm_file_info.h"
#include "xm_errno.h"
#include "xm_constants.h"
#include "xm_string.h"
#include "xm_file.h"

#define PATH_SEPARATOR '/'

/* Remove trailing separators that don't affect the meaning of PATH. */
static const char *path_canonicalize (const char *path, xm_pool_t *pool)
{
    /* At some point this could eliminate redundant components.  For
     * now, it just makes sure there is no trailing slash. */
    size_t len = strlen (path);
    size_t orig_len = len;
    
    while ((len > 0) && (path[len - 1] == PATH_SEPARATOR))
        len--;
    
    if (len != orig_len)
        return xm_pstrndup (pool, path, len);
    else
        return path;
}

/* Remove one component off the end of PATH. */
static char *path_remove_last_component (const char *path, xm_pool_t *pool)
{
    const char *newpath = path_canonicalize (path, pool);
    int i;
    
    for (i = (strlen(newpath) - 1); i >= 0; i--) {
        if (path[i] == PATH_SEPARATOR)
            break;
    }

    return xm_pstrndup (pool, path, (i < 0) ? 0 : i);
}

int xm_dir_open(xm_dir_t **new, const char *dirname, 
                          xm_pool_t *pool)
{
    /* On some platforms (e.g., Linux+GNU libc), d_name[] in struct 
     * dirent is declared with enough storage for the name.  On other
     * platforms (e.g., Solaris 8 for Intel), d_name is declared as a
     * one-byte array.  Note: gcc evaluates this at compile time.
     */
    size_t dirent_size = 
        sizeof(*(*new)->entry) +
        (sizeof((*new)->entry->d_name) > 1 ? 0 : 255);
    DIR *dir = opendir(dirname);

    if (!dir) {
        return errno;
    }

    (*new) = (xm_dir_t *)xm_palloc(pool, sizeof(xm_dir_t));

    (*new)->pool = pool;
    (*new)->dirname = xm_pstrdup(pool, dirname);
    (*new)->dirstruct = dir;
    (*new)->entry = xm_pcalloc(pool, dirent_size);

    return 0;
}

int xm_dir_close(xm_dir_t *dir)
{
    if (closedir(dir->dirstruct) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}

#ifdef DIRENT_TYPE
static xm_filetype_e filetype_from_dirent_type(int type)
{
    switch (type) {
    case DT_REG:
        return XM_REG;
    case DT_DIR:
        return XM_DIR;
    case DT_LNK:
        return XM_LNK;
    case DT_CHR:
        return XM_CHR;
    case DT_BLK:
        return XM_BLK;
#if defined(DT_FIFO)
    case DT_FIFO:
        return XM_PIPE;
#endif
#if !defined(BEOS) && defined(DT_SOCK)
    case DT_SOCK:
        return XM_SOCK;
#endif
    default:
        return XM_UNKFILE;
    }
}
#endif

int xm_dir_read(xm_finfo_t *finfo, int32_t wanted,
                          xm_dir_t *thedir)
{
    int ret = 0;
#ifdef DIRENT_TYPE
    xm_filetype_e type;
#endif



    /* We're about to call a non-thread-safe readdir() that may
       possibly set `errno', and the logic below actually cares about
       errno after the call.  Therefore we need to clear errno first. */
    errno = 0;
    thedir->entry = readdir(thedir->dirstruct);
    if (thedir->entry == NULL) {
        /* If NULL was returned, this can NEVER be a success. Can it?! */
        if (errno == 0) {
            ret = XM_ENOENT;
        }
        else
            ret = errno;
    }

    /* No valid bit flag to test here - do we want one? */
    finfo->fname = NULL;

    if (ret) {
        finfo->valid = 0;
        return ret;
    }

#ifdef DIRENT_TYPE
    type = filetype_from_dirent_type(thedir->entry->DIRENT_TYPE);
    if (type != XM_UNKFILE) {
        wanted &= ~XM_FINFO_TYPE;
    }
#endif
#ifdef DIRENT_INODE
    if (thedir->entry->DIRENT_INODE && thedir->entry->DIRENT_INODE != -1) {

        wanted &= ~XM_FINFO_INODE;
    }
#endif /* DIRENT_INODE */

    wanted &= ~XM_FINFO_NAME;

    if (wanted)
    {
        char fspec[XM_PATH_MAX];
        char *end;

        end = xm_cpystrn(fspec, thedir->dirname, sizeof fspec);

        if (end > fspec && end[-1] != '/' && (end < fspec + XM_PATH_MAX))
            *end++ = '/';

        xm_cpystrn(end, thedir->entry->d_name, 
                    sizeof fspec - (end - fspec));

        ret = xm_stat(finfo, fspec, XM_FINFO_LINK | wanted, thedir->pool);
        /* We passed a stack name that will disappear */
        finfo->fname = NULL;
    }

    if (wanted && (ret == 0 || ret == XM_INCOMPLETE)) {
        wanted &= ~finfo->valid;
    }
    else {
        /* We don't bail because we fail to stat, when we are only -required-
         * to readdir... but the result will be XM_INCOMPLETE
         */
        finfo->pool = thedir->pool;
        finfo->valid = 0;
#ifdef DIRENT_TYPE
        if (type != XM_UNKFILE) {
            finfo->filetype = type;
            finfo->valid |= XM_FINFO_TYPE;
        }
#endif
#ifdef DIRENT_INODE
        if (thedir->entry->DIRENT_INODE && thedir->entry->DIRENT_INODE != -1) {
            finfo->inode = thedir->entry->DIRENT_INODE;
            finfo->valid |= XM_FINFO_INODE;
        }
#endif
    }

    finfo->name = xm_pstrdup(thedir->pool, thedir->entry->d_name);
    finfo->valid |= XM_FINFO_NAME;

    if (wanted)
        return XM_INCOMPLETE;

    return 0;
}

int xm_dir_rewind(xm_dir_t *thedir)
{
    rewinddir(thedir->dirstruct);
    return 0;
}

int xm_dir_make(const char *path, int32_t perm, 
                          xm_pool_t *pool)
{
	pool = pool;

    mode_t mode = xm_perms2mode(perm);

    if (mkdir(path, mode) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}

int xm_dir_make_recursive(const char *path, int32_t perm,
                                           xm_pool_t *pool) 
{
    int xm_err = 0;

    xm_err = xm_dir_make (path, perm, pool); /* Try to make PATH right out */
    
    if (xm_err == ENOENT) { /* Missing an intermediate dir */
        char *dir;
        
        dir = path_remove_last_component(path, pool);
        /* If there is no path left, give up. */
        if (dir[0] == '\0') {
            return xm_err;
        }

        xm_err = xm_dir_make_recursive(dir, perm, pool);
        
        if (!xm_err) 
            xm_err = xm_dir_make (path, perm, pool);
    }

    /*
     * It's OK if PATH exists. Timing issues can lead to the second
     * xm_dir_make being called on existing dir, therefore this check
     * has to come last.
     */
    if (XM_STATUS_IS_EEXIST(xm_err))
        return 0;

    return xm_err;
}

int xm_dir_remove(const char *path, xm_pool_t *pool)
{
	pool = pool;

    if (rmdir(path) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}



  
