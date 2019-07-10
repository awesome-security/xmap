/*
 *
 *      Filename: xm_fileacc.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 13:10:06
 * Last Modified: 2017-01-11 13:18:40
 */

#include "xm_file_info.h"

mode_t xm_perms2mode(int32_t perms)
{
    mode_t mode = 0;

    if (perms & XM_USETID)
        mode |= S_ISUID;
    if (perms & XM_UREAD)
        mode |= S_IRUSR;
    if (perms & XM_UWRITE)
        mode |= S_IWUSR;
    if (perms & XM_UEXECUTE)
        mode |= S_IXUSR;

    if (perms & XM_GSETID)
        mode |= S_ISGID;
    if (perms & XM_GREAD)
        mode |= S_IRGRP;
    if (perms & XM_GWRITE)
        mode |= S_IWGRP;
    if (perms & XM_GEXECUTE)
        mode |= S_IXGRP;

#ifdef S_ISVTX
    if (perms & XM_WSTICKY)
        mode |= S_ISVTX;
#endif
    if (perms & XM_WREAD)
        mode |= S_IROTH;
    if (perms & XM_WWRITE)
        mode |= S_IWOTH;
    if (perms & XM_WEXECUTE)
        mode |= S_IXOTH;

    return mode;
}

int32_t xm_mode2perms(mode_t mode)
{
    int32_t perms = 0;

    if (mode & S_ISUID)
        perms |= XM_USETID;
    if (mode & S_IRUSR)
        perms |= XM_UREAD;
    if (mode & S_IWUSR)
        perms |= XM_UWRITE;
    if (mode & S_IXUSR)
        perms |= XM_UEXECUTE;

    if (mode & S_ISGID)
        perms |= XM_GSETID;
    if (mode & S_IRGRP)
        perms |= XM_GREAD;
    if (mode & S_IWGRP)
        perms |= XM_GWRITE;
    if (mode & S_IXGRP)
        perms |= XM_GEXECUTE;

#ifdef S_ISVTX
    if (mode & S_ISVTX)
        perms |= XM_WSTICKY;
#endif
    if (mode & S_IROTH)
        perms |= XM_WREAD;
    if (mode & S_IWOTH)
        perms |= XM_WWRITE;
    if (mode & S_IXOTH)
        perms |= XM_WEXECUTE;

    return perms;
}


