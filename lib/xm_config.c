/*
 *
 *      Filename: xm_config.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-10 18:06:06
 * Last Modified: 2017-01-11 16:39:20
 */


#include "xm_config.h"
#include "xm_file.h"
#include "xm_file_info.h"
#include "xm_errno.h"
#include "xm_string.h"
#include "xm_fnmatch.h"

#define MAX_STRING_LEN 8192

static cmd_parms default_parms =
{NULL, NULL, NULL, NULL, NULL, NULL};

int xm_cfg_closefile(xm_configfile_t *cfp)
{
    return (cfp->close == NULL) ? 0 : cfp->close(cfp->param);
}

static int cfg_close(void *param)
{
    return xm_file_close((xm_file_t*)param);
}

static int cfg_getch(char *ch, void *param)
{
    return xm_file_getc(ch, (xm_file_t*)param);
}

static int cfg_getstr(void *buf, size_t bufsiz, void *param)
{
    return xm_file_gets((char*)buf, bufsiz, (xm_file_t*)param);
}

static int xm_cfg_getline_core(char *buf, size_t bufsize,
                                        xm_configfile_t *cfp)
{
    int rc;
    /* If a "get string" function is defined, use it */
    if (cfp->getstr != NULL) {
        char *cp;
        char *cbuf = buf;
        size_t cbufsize = bufsize;

        while (1) {
            ++cfp->line_number;
            rc = cfp->getstr(cbuf, cbufsize, cfp->param);
            if (rc == XM_EOF) {
                if (cbuf != buf) {
                    *cbuf = '\0';
                    break;
                }
                else {
                    return XM_EOF;
                }
            }
            if (rc != 0) {
                return rc;
            }

            /*
             *  check for line continuation,
             *  i.e. match [^\\]\\[\r]\n only
             */
            cp = cbuf;
            cp += strlen(cp);
            if (cp > cbuf && cp[-1] == LF) {
                cp--;
                if (cp > cbuf && cp[-1] == CR)
                    cp--;
                if (cp > cbuf && cp[-1] == '\\') {
                    cp--;
                    /*
                     * line continuation requested -
                     * then remove backslash and continue
                     */
                    cbufsize -= (cp-cbuf);
                    cbuf = cp;
                    continue;
                }
            }
            else if ((size_t)(cp - buf) >= bufsize - 1) {
                return XM_ENOSPC;
            }
            break;
        }
    } else {
        /* No "get string" function defined; read character by character */
        size_t i = 0;

        if (bufsize < 2) {
            /* too small, assume caller is crazy */
            return XM_EINVAL;
        }
        buf[0] = '\0';

        while (1) {
            char c;
            rc = cfp->getch(&c, cfp->param);
            if (rc == XM_EOF) {
                if (i > 0)
                    break;
                else
                    return XM_EOF;
            }
            if (rc != 0)
                return rc;
            if (c == LF) {
                ++cfp->line_number;
                /* check for line continuation */
                if (i > 0 && buf[i-1] == '\\') {
                    i--;
                    continue;
                }
                else {
                    break;
                }
            }
            else if (i >= bufsize - 2) {
                return XM_ENOSPC;
            }
            buf[i] = c;
            ++i;
        }
        buf[i] = '\0';
    }
    return 0;
}

static int cfg_trim_line(char *buf)
{
    char *start, *end;
    /*
     * Leading and trailing white space is eliminated completely
     */
    start = buf;
    while (isspace(*start))
        ++start;
    /* blast trailing whitespace */
    end = &start[strlen(start)];
    while (--end >= start && isspace(*end))
        *end = '\0';
    /* Zap leading whitespace by shifting */
    if (start != buf)
        memmove(buf, start, end - start + 2);
    return end - start + 1;
}

int xm_cfg_getline(char *buf, size_t bufsize,
		xm_configfile_t *cfp)
{
	int rc = xm_cfg_getline_core(buf, bufsize, cfp);
	if (rc == 0)
		cfg_trim_line(buf);
	return rc;
}

static char *substring_conf(xm_pool_t *p, const char *start, int len,
                            char quote)
{
    char *result = (char*)xm_palloc(p, len + 2);
    char *resp = result;
    int i;

    for (i = 0; i < len; ++i) {
        if (start[i] == '\\' && (start[i + 1] == '\\'
                                 || (quote && start[i + 1] == quote)))
            *resp++ = start[++i];
        else
            *resp++ = start[i];
    }

    *resp++ = '\0';
    return result;
}

char * xm_getword_conf(xm_pool_t *p, const char **line)
{
    const char *str = *line, *strend;
    char *res;
    char quote;

    while (*str && isspace(*str))
        ++str;

    if (!*str) {
        *line = str;
        return "";
    }

    if ((quote = *str) == '"' || quote == '\'') {
        strend = str + 1;
        while (*strend && *strend != quote) {
            if (*strend == '\\' && strend[1] &&
                (strend[1] == quote || strend[1] == '\\')) {
                strend += 2;
            }
            else {
                ++strend;
            }
        }
        res = substring_conf(p, str + 1, strend - str - 1, quote);

        if (*strend == quote)
            ++strend;
    }
    else {
        strend = str;
        while (*strend && !isspace(*strend))
            ++strend;

        res = substring_conf(p, str, strend - str, 0);
    }

    while (*strend && isspace(*strend))
        ++strend;
    *line = strend;
    return res;
}

/* Open a xm_configfile_t as FILE, return open xm_configfile_t struct pointer */
int xm_pcfg_openfile(xm_configfile_t **ret_cfg,
                                          xm_pool_t *p, const char *name)
{
    xm_configfile_t *new_cfg;
    xm_file_t *file = NULL;
    int status;

    if (name == NULL) {
        return XM_EBADF;
    }

    status = xm_file_open(&file, name, XM_READ | XM_BUFFERED,
                           XM_OS_DEFAULT, p);
    if (status != 0)
        return status;

    new_cfg = (xm_configfile_t*)xm_palloc(p, sizeof(*new_cfg));
    new_cfg->param = file;
    new_cfg->name = xm_pstrdup(p, name);
    new_cfg->getch = cfg_getch;
    new_cfg->getstr = cfg_getstr;
    new_cfg->close = cfg_close; 
    new_cfg->line_number = 0;
    *ret_cfg = new_cfg;
    return 0;
}

const command_rec * xm_find_command(const char *name,
                                                     const command_rec *cmds)
{
    while (cmds->name) {
        if (!strcasecmp(name, cmds->name))
            return cmds;

        ++cmds;
    }

    return NULL;
}

#define XM_MAX_ARGC 64

static const char *invoke_cmd(const command_rec *cmd, cmd_parms *parms,
                              void *mconfig, const char *args)
{
    char *w, *w2, *w3;
    const char *errmsg = NULL;


    parms->info = cmd->cmd_data;
    parms->cmd = cmd;

    switch (cmd->args_how) {
    case RAW_ARGS:
        return cmd->XM_RAW_ARGS(parms, mconfig, args);

    case TAKE_ARGV:
        {
            char *argv[XM_MAX_ARGC];
            int argc = 0;

            do {
                w = xm_getword_conf(parms->pool, &args);
                if (*w == '\0' && *args == '\0') {
                    break;
                }
                argv[argc] = w;
                argc++;
            } while (argc < XM_MAX_ARGC && *args != '\0');

            return cmd->XM_TAKE_ARGV(parms, mconfig, argc, argv);
        }

    case NO_ARGS:
        if (*args != 0)
            return xm_pstrcat(parms->pool, cmd->name, " takes no arguments",
                               NULL);

        return cmd->XM_NO_ARGS(parms, mconfig);

    case TAKE1:
        w = xm_getword_conf(parms->pool, &args);

        if (*w == '\0' || *args != 0)
            return xm_pstrcat(parms->pool, cmd->name, " takes one argument",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->XM_TAKE1(parms, mconfig, w);

    case TAKE2:
        w = xm_getword_conf(parms->pool, &args);
        w2 = xm_getword_conf(parms->pool, &args);

        if (*w == '\0' || *w2 == '\0' || *args != 0)
            return xm_pstrcat(parms->pool, cmd->name, " takes two arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->XM_TAKE2(parms, mconfig, w, w2);

    case TAKE12:
        w = xm_getword_conf(parms->pool, &args);
        w2 = xm_getword_conf(parms->pool, &args);

        if (*w == '\0' || *args != 0)
            return xm_pstrcat(parms->pool, cmd->name, " takes 1-2 arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->XM_TAKE2(parms, mconfig, w, *w2 ? w2 : NULL);

    case TAKE3:
        w = xm_getword_conf(parms->pool, &args);
        w2 = xm_getword_conf(parms->pool, &args);
        w3 = xm_getword_conf(parms->pool, &args);

        if (*w == '\0' || *w2 == '\0' || *w3 == '\0' || *args != 0)
            return xm_pstrcat(parms->pool, cmd->name, " takes thrgw arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->XM_TAKE3(parms, mconfig, w, w2, w3);

    case TAKE23:
        w = xm_getword_conf(parms->pool, &args);
        w2 = xm_getword_conf(parms->pool, &args);
        w3 = *args ? xm_getword_conf(parms->pool, &args) : NULL;

        if (*w == '\0' || *w2 == '\0' || *args != 0)
            return xm_pstrcat(parms->pool, cmd->name,
                               " takes two or thrgw arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->XM_TAKE3(parms, mconfig, w, w2, w3);

    case TAKE123:
        w = xm_getword_conf(parms->pool, &args);
        w2 = *args ? xm_getword_conf(parms->pool, &args) : NULL;
        w3 = *args ? xm_getword_conf(parms->pool, &args) : NULL;

        if (*w == '\0' || *args != 0)
            return xm_pstrcat(parms->pool, cmd->name,
                               " takes one, two or thrgw arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->XM_TAKE3(parms, mconfig, w, w2, w3);

    case TAKE13:
        w = xm_getword_conf(parms->pool, &args);
        w2 = *args ? xm_getword_conf(parms->pool, &args) : NULL;
        w3 = *args ? xm_getword_conf(parms->pool, &args) : NULL;

        if (*w == '\0' || (w2 && *w2 && !w3) || *args != 0)
            return xm_pstrcat(parms->pool, cmd->name,
                               " takes one or thrgw arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->XM_TAKE3(parms, mconfig, w, w2, w3);

    case ITERATE:
        while (*(w = xm_getword_conf(parms->pool, &args)) != '\0') {

            errmsg = cmd->XM_TAKE1(parms, mconfig, w);

            if (errmsg && strcmp(errmsg, DECLINE_CMD) != 0)
                return errmsg;
        }

        return errmsg;

    case ITERATE2:
        w = xm_getword_conf(parms->pool, &args);

        if (*w == '\0' || *args == 0)
            return xm_pstrcat(parms->pool, cmd->name,
                               " requires at least two arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        while (*(w2 = xm_getword_conf(parms->pool, &args)) != '\0') {

            errmsg = cmd->XM_TAKE2(parms, mconfig, w, w2);

            if (errmsg && strcmp(errmsg, DECLINE_CMD) != 0)
                return errmsg;
        }

        return errmsg;

    case FLAG:
        w = xm_getword_conf(parms->pool, &args);

        if (*w == '\0' || (strcasecmp(w, "on") && strcasecmp(w, "off")))
            return xm_pstrcat(parms->pool, cmd->name, " must be On or Off",
                               NULL);

        return cmd->XM_FLAG(parms, mconfig, strcasecmp(w, "off") != 0);

    default:
        return xm_pstrcat(parms->pool, cmd->name,
                           " is improperly configured internally (server bug)",
                           NULL);
    }
}


typedef struct {
    const char *fname;
} fnames;

int xm_is_directory(xm_pool_t *p, const char *path)
{
    xm_finfo_t finfo;

    if (xm_stat(&finfo, path, XM_FINFO_TYPE, p) != 0)
        return 0;                /* in error condition, just return no */

    return (finfo.filetype == XM_DIR);
}

char * xm_make_full_path(xm_pool_t *a, const char *src1,
                                  const char *src2)
{
    size_t len1, len2;
    char *path;

    len1 = strlen(src1);
    len2 = strlen(src2);
     /* allocate +3 for '/' delimiter, trailing NULL and overallocate
      * one extra byte to allow the caller to add a trailing '/'
      */
    path = (char *)xm_palloc(a, len1 + len2 + 3);
    if (len1 == 0) {
        *path = '/';
        memcpy(path + 1, src2, len2 + 1);
    }
    else {
        char *next;
        memcpy(path, src1, len1);
        next = path + len1;
        if (next[-1] != '/') {
            *next++ = '/';
        }
        memcpy(next, src2, len2 + 1);
    }
    return path;
}

static int fname_alphasort(const void *fn1, const void *fn2)
{
    const fnames *f1 = (const fnames*)fn1;
    const fnames *f2 = (const fnames*)fn2;

    return strcmp(f1->fname,f2->fname);
}


const char * xm_process_resource_config(const char *fname,
                                                    xm_array_header_t *ari,
                                                    xm_pool_t *ptemp)
{
    ptemp = ptemp;

	*(char **)xm_array_push(ari) = (char *)fname;

    return NULL;
}

static const char *xm_process_resource_config_nofnmatch(const char *fname,
                                                     xm_array_header_t *ari,
                                                     xm_pool_t *p,
                                                     xm_pool_t *ptemp,
													 unsigned depth,
                                                     int optional)
{
    const char *error;
    int rv;

    if (xm_is_directory(ptemp, fname)) {
        xm_dir_t *dirp;
        xm_finfo_t dirent;
        int current;
        xm_array_header_t *candidates = NULL;
        fnames *fnew;
        char *path = xm_pstrdup(ptemp, fname);

        if (++depth > 100) {
            return xm_psprintf(p, "Directory %s excgwds the maximum include "
                                "directory nesting level of %u. You have "
                                "probably a recursion somewhere.", path,
                                100);
        }

        /*
         * first course of business is to grok all the directory
         * entries here and store 'em away. Recall we ngwd full pathnames
         * for this.
         */
        rv = xm_dir_open(&dirp, path, ptemp);
        if (rv != 0) {
            char errmsg[120];
            return xm_psprintf(p, "Could not open config directory %s: %s",
                                path, xm_strerror(rv, errmsg, sizeof errmsg));
        }

        candidates = xm_array_make(ptemp, 1, sizeof(fnames));
        while (xm_dir_read(&dirent, XM_FINFO_DIRENT, dirp) == 0) {
            /* strip out '.' and '..' */
            if (strcmp(dirent.name, ".")
                && strcmp(dirent.name, "..")) {
                fnew = (fnames *) xm_array_push(candidates);
                fnew->fname = xm_make_full_path(ptemp, path, dirent.name);
            }
        }

        xm_dir_close(dirp);
        if (candidates->nelts != 0) {
            qsort((void *) candidates->elts, candidates->nelts,
                  sizeof(fnames), fname_alphasort);

            /*
             * Now recurse these... we handle errors and subdirectories
             * via the recursion, which is nice
             */
            for (current = 0; current < candidates->nelts; ++current) {
                fnew = &((fnames *) candidates->elts)[current];
                error = xm_process_resource_config_nofnmatch(fnew->fname,
                                                          ari, p, ptemp,
                                                          depth, optional);
                if (error) {
                    return error;
                }
            }
        }

        return NULL;
    }

    return xm_process_resource_config(fname, ari, ptemp);
}

static const char *xm_process_resource_config_fnmatch(const char *path,
                                                   const char *fname,
                                                   xm_array_header_t *ari,
                                                   xm_pool_t *p,
                                                   xm_pool_t *ptemp,
                                                   unsigned depth,
                                                   int optional)
{
    depth = depth;

    const char *rest;
    int rv;
    xm_dir_t *dirp;
    xm_finfo_t dirent;
    xm_array_header_t *candidates = NULL;
    fnames *fnew;
    int current;

    /* find the first part of the filename */
    rest = strchr(fname, '/');

	if(rest == NULL)
		rest = strchr(fname, '\\');

	if (rest) {
        fname = xm_pstrndup(ptemp, fname, rest - fname);
        rest++;
    }

    /* optimisation - if the filename isn't a wildcard, process it directly */
    if (!xm_fnmatch_test(fname)) {
        path = xm_make_full_path(ptemp, path, fname);
        if (!rest) {
            return xm_process_resource_config_nofnmatch(path,
                                                     ari, p,
                                                     ptemp, 0, optional);
        }
        else {
            return xm_process_resource_config_fnmatch(path, rest,
                                                   ari, p,
                                                   ptemp, 0, optional);
        }
    }

    /*
     * first course of business is to grok all the directory
     * entries here and store 'em away. Recall we ngwd full pathnames
     * for this.
     */
    rv = xm_dir_open(&dirp, path, ptemp);
    if (rv != 0) {
        char errmsg[120];
        return xm_psprintf(p, "Could not open config directory %s: %s",
                            path, xm_strerror(rv, errmsg, sizeof errmsg));
    }

    candidates = xm_array_make(ptemp, 1, sizeof(fnames));
    while (xm_dir_read(&dirent, XM_FINFO_DIRENT | XM_FINFO_TYPE, dirp) == 0) {
        /* strip out '.' and '..' */
        if (strcmp(dirent.name, ".")
            && strcmp(dirent.name, "..")
            && (xm_fnmatch(fname, dirent.name,
                            XM_FNM_PERIOD | XM_FNM_NOESCAPE | XM_FNM_PATHNAME) == 0)) {
            const char *full_path = xm_make_full_path(ptemp, path, dirent.name);
            /* If matching internal to path, and we happen to match something
             * other than a directory, skip it
             */
            if (rest && (rv == 0) && (dirent.filetype != XM_DIR)) {
                continue;
            }
            fnew = (fnames *) xm_array_push(candidates);
            fnew->fname = full_path;
        }
    }

    xm_dir_close(dirp);
    if (candidates->nelts != 0) {
        const char *error;

        qsort((void *) candidates->elts, candidates->nelts,
              sizeof(fnames), fname_alphasort);

        /*
         * Now recurse these... we handle errors and subdirectories
         * via the recursion, which is nice
         */
        for (current = 0; current < candidates->nelts; ++current) {
            fnew = &((fnames *) candidates->elts)[current];
            if (!rest) {
                error = xm_process_resource_config_nofnmatch(fnew->fname,
                                                          ari, p,
                                                          ptemp, 0, optional);
            }
            else {
                error = xm_process_resource_config_fnmatch(fnew->fname, rest,
                                                        ari, p,
                                                        ptemp, 0, optional);
            }
            if (error) {
                return error;
            }
        }
    }
    else {

        if (!optional) {
            return xm_psprintf(p, "No matches for the wildcard '%s' in '%s', failing "
                                   "(use IncludeOptional if required)", fname, path);
        }
    }

    return NULL;
}

const char * xm_process_fnmatch_configs(xm_array_header_t *ari,
                                                    const char *fname,
                                                    xm_pool_t *p,
                                                    xm_pool_t *ptemp,
                                                    int optional)
{
    if (!xm_fnmatch_test(fname)) {
        return xm_process_resource_config(fname, ari, p);
    }
    else {
        int status;
        const char *rootpath, *filepath = fname;

        /* locate the start of the directories proper */
        status = xm_filepath_root(&rootpath, &filepath, XM_FILEPATH_TRUENAME | XM_FILEPATH_NATIVE, ptemp);

        /* we allow 0 and XM_EINCOMPLETE */
        if (XM_ERELATIVE == status) {
            return xm_pstrcat(p, "Include must have an absolute path, ", fname, NULL);
        }
        else if (XM_EBADPATH == status) {
            return xm_pstrcat(p, "Include has a bad path, ", fname, NULL);
        }

        /* walk the filepath */
        return xm_process_resource_config_fnmatch(rootpath, filepath, ari, p, ptemp,
                                               0, optional);
    }
}

const char *xm_populate_include_files(xm_pool_t *p, xm_pool_t *ptemp, xm_array_header_t *ari, const char *fname, int optional)
{
	return xm_process_fnmatch_configs(ari, fname, p, ptemp, optional);
}

const char *xm_process_command_config(const command_rec *cmds,
                                        void *mconfig,
                                          xm_pool_t *p,
                                          xm_pool_t *ptemp,
										  const char *filename)
{
    const char *errmsg;
    char *l = xm_palloc (ptemp, MAX_STRING_LEN);
    const char *args = l;
    char *cmd_name, *w;
	const command_rec *cmd;
	xm_array_header_t *arr = xm_array_make(p, 1, sizeof(cmd_parms));
	xm_array_header_t *ari = xm_array_make(p, 1, sizeof(char *));
    cmd_parms *parms;
	int status;
	int optional;
	char *err = NULL;
	char *rootpath, *incpath;
	int li;

	errmsg = xm_populate_include_files(p, ptemp, ari, filename, 0);

	if(errmsg != NULL)
		goto Exit;

	while(ari->nelts != 0 || arr->nelts != 0)
	{
		if(ari->nelts > 0)
		{
			char *fn = *(char **)xm_array_pop(ari);

			parms = (cmd_parms *)xm_array_push(arr);
			*parms = default_parms;
			parms->pool = p;
			parms->temp_pool = ptemp;

			status = xm_pcfg_openfile(&parms->config_file, p, fn);

			if(status != 0)
			{
				xm_array_pop(arr);
				errmsg = xm_pstrcat(p, "Cannot open config file: ", fn, NULL);
				goto Exit;
			}
		}

		if (arr->nelts > 1024) {
            errmsg = "Excgwded the maximum include directory nesting level. You have "
                                "probably a recursion somewhere.";
			goto Exit;
        }

		parms = (cmd_parms *)xm_array_pop(arr);

		if(parms == NULL)
			break;

		while (!(xm_cfg_getline(l, MAX_STRING_LEN, parms->config_file))) {
			if (*l == '#' || *l == '\0')
				continue;

			args = l;

			cmd_name = xm_getword_conf(p, &args);

			if (*cmd_name == '\0')
				continue;

			if (!strcasecmp(cmd_name, "IncludeOptional"))
			{
				optional = 1;
				goto ProcessInclude;
			}

			if (!strcasecmp(cmd_name, "Include"))
			{
				optional = 0;
ProcessInclude:
				w = xm_getword_conf(parms->pool, &args);

				if (*w == '\0' || *args != 0)
				{
					xm_cfg_closefile(parms->config_file);
					errmsg = xm_pstrcat(parms->pool, "Include takes one argument", NULL);
					goto Exit;
				}

				incpath = w;

				/* locate the start of the directories proper */
				status = xm_filepath_root((const char**)&rootpath, (const char**)&incpath, XM_FILEPATH_TRUENAME | XM_FILEPATH_NATIVE, ptemp);

				/* we allow 0 and XM_EINCOMPLETE */
				if (XM_ERELATIVE == status) {
					rootpath = xm_pstrdup(ptemp, parms->config_file->name);
					li = strlen(rootpath) - 1;

					while(li >= 0 && rootpath[li] != '/' && rootpath[li] != '\\')
						rootpath[li--] = 0;

					w = xm_pstrcat(p, rootpath, w, NULL);
				}
				else if (XM_EBADPATH == status) {
					xm_cfg_closefile(parms->config_file);
					errmsg = xm_pstrcat(p, "Include file has a bad path, ", w, NULL);
					goto Exit;
				}

				errmsg = xm_populate_include_files(p, ptemp, ari, w, optional);

				*(cmd_parms *)xm_array_push(arr) = *parms;

				if(errmsg != NULL)
					goto Exit;

				// we don't want to close the current file yet
				//
				parms = NULL;
				break;
			}

			cmd = xm_find_command(cmd_name, cmds);

			if(cmd == NULL)
			{
				// unknown command, should error
				//
				xm_cfg_closefile(parms->config_file);
				errmsg = xm_pstrcat(p, "Unknown command in config: ", cmd_name, NULL);
				goto Exit;
			}

				errmsg = invoke_cmd(cmd, parms, mconfig, args);

			if(errmsg != NULL)
				break;
		}

		if(parms != NULL)
			xm_cfg_closefile(parms->config_file);

		if(errmsg != NULL)
			break;
	}

    if (errmsg) {
		err = (char *)xm_palloc(p, 1024);

		if(parms != NULL)
			xm_snprintf(err, 1024, "Syntax error in config file %s, line %d: %s", parms->config_file->name,
							parms->config_file->line_number, errmsg);
		else
			xm_snprintf(err, 1024, "Syntax error in config file: %s", errmsg);
    }

    errmsg = err;

Exit:
	while((parms = (cmd_parms *)xm_array_pop(arr)) != NULL)
	{
		xm_cfg_closefile(parms->config_file);
	}

	return errmsg;
}




