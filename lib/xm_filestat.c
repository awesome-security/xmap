/*
 *
 *      Filename: xm_filestat.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 14:20:18
 * Last Modified: 2017-01-11 15:10:33
 */

#include "xm_file_info.h"
#include "xm_file.h"
#include "xm_errno.h"
#include "xm_time.h"

static xm_filetype_e filetype_from_mode(mode_t mode)
{
    xm_filetype_e type;

    switch (mode & S_IFMT) {
    case S_IFREG:
        type = XM_REG;  break;
    case S_IFDIR:
        type = XM_DIR;  break;
    case S_IFLNK:
        type = XM_LNK;  break;
    case S_IFCHR:
        type = XM_CHR;  break;
    case S_IFBLK:
        type = XM_BLK;  break;
#if defined(S_IFFIFO)
    case S_IFFIFO:
        type = XM_PIPE; break;
#endif
#if !defined(BEOS) && defined(S_IFSOCK)
    case S_IFSOCK:
        type = XM_SOCK; break;
#endif

    default:
	/* Work around missing S_IFxxx values above
         * for Linux et al.
         */
#if !defined(S_IFFIFO) && defined(S_ISFIFO)
    	if (S_ISFIFO(mode)) {
            type = XM_PIPE;
	} else
#endif
#if !defined(BEOS) && !defined(S_IFSOCK) && defined(S_ISSOCK)
    	if (S_ISSOCK(mode)) {
            type = XM_SOCK;
	} else
#endif
        type = XM_UNKFILE;
    }
    return type;
}

static void fill_out_finfo(xm_finfo_t *finfo, struct_stat *info,
                           int32_t wanted)
{
	wanted = wanted;

    finfo->valid = XM_FINFO_MIN | XM_FINFO_IDENT | XM_FINFO_NLINK
                 | XM_FINFO_OWNER | XM_FINFO_PROT;
    finfo->protection = xm_mode2perms(info->st_mode);
    finfo->filetype = filetype_from_mode(info->st_mode);
    finfo->user = info->st_uid;
    finfo->group = info->st_gid;
    finfo->size = info->st_size;
    finfo->device = info->st_dev;
    finfo->nlink = info->st_nlink;

    /* Check for overflow if storing a 64-bit st_ino in a 32-bit
     * ino_t for LFS builds: */
    if (sizeof(ino_t) >= sizeof(info->st_ino)
        || (ino_t)info->st_ino == info->st_ino) {
        finfo->inode = info->st_ino;
    } else {
        finfo->valid &= ~XM_FINFO_INODE;
    }

    xm_time_ansi_put(&finfo->atime, info->st_atime);
    finfo->atime += info->st_atim.tv_nsec / 1000L;

    xm_time_ansi_put(&finfo->mtime, info->st_mtime);
    finfo->mtime += info->st_mtim.tv_nsec / 1000L;

    xm_time_ansi_put(&finfo->ctime, info->st_ctime);
    finfo->ctime += info->st_ctim.tv_nsec / 1000L;

#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
#ifdef DEV_BSIZE
    finfo->csize = (xm_off_t)info->st_blocks * (xm_off_t)DEV_BSIZE;
#else
    finfo->csize = (xm_off_t)info->st_blocks * (xm_off_t)512;
#endif
    finfo->valid |= XM_FINFO_CSIZE;
#endif
}

int xm_file_info_get_locked(xm_finfo_t *finfo, int32_t wanted,
                                      xm_file_t *thefile)
{
    struct_stat info;

    if (thefile->buffered) {
        int rv = xm_file_flush_locked(thefile);
        if (rv != 0)
            return rv;
    }

    if (fstat(thefile->filedes, &info) == 0) {
        finfo->pool = thefile->pool;
        finfo->fname = thefile->fname;
        fill_out_finfo(finfo, &info, wanted);
        return (wanted & ~finfo->valid) ? XM_INCOMPLETE : 0;
    }
    else {
        return errno;
    }
}

int xm_file_info_get(xm_finfo_t *finfo, 
                                            int32_t wanted,
                                            xm_file_t *thefile)
{
    struct_stat info;

    if (thefile->buffered) {
        int rv = xm_file_flush(thefile);
        if (rv != 0)
            return rv;
    }

    if (fstat(thefile->filedes, &info) == 0) {
        finfo->pool = thefile->pool;
        finfo->fname = thefile->fname;
        fill_out_finfo(finfo, &info, wanted);
        return (wanted & ~finfo->valid) ? XM_INCOMPLETE : 0;
    }
    else {
        return errno;
    }
}

int xm_file_perms_set(const char *fname, 
                                             int32_t perms)
{
    mode_t mode = xm_perms2mode(perms);

    if (chmod(fname, mode) == -1)
        return errno;
    return 0;
}

int xm_file_attrs_set(const char *fname,
                                             xm_fileattrs_t attributes,
                                             xm_fileattrs_t attr_mask,
                                             xm_pool_t *pool)
{
    int status;
    xm_finfo_t finfo;

    /* Don't do anything if we can't handle the requested attributes */
    if (!(attr_mask & (XM_FILE_ATTR_READONLY
                       | XM_FILE_ATTR_EXECUTABLE)))
        return 0;

    status = xm_stat(&finfo, fname, XM_FINFO_PROT, pool);
    if (status)
        return status;

    /* ### TODO: should added bits be umask'd? */
    if (attr_mask & XM_FILE_ATTR_READONLY)
    {
        if (attributes & XM_FILE_ATTR_READONLY)
        {
            finfo.protection &= ~XM_UWRITE;
            finfo.protection &= ~XM_GWRITE;
            finfo.protection &= ~XM_WWRITE;
        }
        else
        {
            /* ### umask this! */
            finfo.protection |= XM_UWRITE;
            finfo.protection |= XM_GWRITE;
            finfo.protection |= XM_WWRITE;
        }
    }

    if (attr_mask & XM_FILE_ATTR_EXECUTABLE)
    {
        if (attributes & XM_FILE_ATTR_EXECUTABLE)
        {
            /* ### umask this! */
            finfo.protection |= XM_UEXECUTE;
            finfo.protection |= XM_GEXECUTE;
            finfo.protection |= XM_WEXECUTE;
        }
        else
        {
            finfo.protection &= ~XM_UEXECUTE;
            finfo.protection &= ~XM_GEXECUTE;
            finfo.protection &= ~XM_WEXECUTE;
        }
    }

    return xm_file_perms_set(fname, finfo.protection);
}


int xm_file_mtime_set(const char *fname,
                                              xm_time_t mtime,
                                              xm_pool_t *pool)
{
    int status;
    xm_finfo_t finfo;

    status = xm_stat(&finfo, fname, XM_FINFO_ATIME, pool);
    if (status) {
        return status;
    }

    {
      struct timeval tvp[2];
    
      tvp[0].tv_sec = xm_time_sec(finfo.atime);
      tvp[0].tv_usec = xm_time_usec(finfo.atime);
      tvp[1].tv_sec = xm_time_sec(mtime);
      tvp[1].tv_usec = xm_time_usec(mtime);
      
      if (utimes(fname, tvp) == -1) {
        return errno;
      }
    }

    return 0;
}


int xm_stat(xm_finfo_t *finfo, 
                                   const char *fname, 
                                   int32_t wanted, xm_pool_t *pool)
{
    struct_stat info;
    int srv;

    if (wanted & XM_FINFO_LINK)
        srv = lstat(fname, &info);
    else
        srv = stat(fname, &info);

    if (srv == 0) {
        finfo->pool = pool;
        finfo->fname = fname;
        fill_out_finfo(finfo, &info, wanted);
        if (wanted & XM_FINFO_LINK)
            wanted &= ~XM_FINFO_LINK;
        return (wanted & ~finfo->valid) ? XM_INCOMPLETE : 0;
    }
    else {
#if !defined(ENOENT) || !defined(ENOTDIR)
#error ENOENT || ENOTDIR not defined; please see the
#error comments at this line in the source for a workaround.
        /*
         * If ENOENT || ENOTDIR is not defined in one of the your OS's
         * include files, GW cannot report a good reason why the stat()
         * of the file failed; there are cases where it can fail even though
         * the file exists.  This opens holes in Apache, for example, because
         * it becomes possible for someone to get a directory listing of a 
         * directory even though there is an index (eg. index.html) file in 
         * it.  If you do not have a problem with this, delete the above 
         * #error lines and start the compile again.  If you need to do this,
         * please submit a bug report to http://www.apache.org/bug_report.html
         * letting us know that you needed to do this.  Please be sure to 
         * include the operating system you are using.
         */
        /* WARNING: All errors will be handled as not found
         */
#if !defined(ENOENT) 
        return XM_ENOENT;
#else
        /* WARNING: All errors but not found will be handled as not directory
         */
        if (errno != ENOENT)
            return XM_ENOENT;
        else
            return errno;
#endif
#else /* All was defined well, report the usual: */
        return errno;
#endif
    }
}


