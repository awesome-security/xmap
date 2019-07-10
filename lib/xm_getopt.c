/*
 *
 *      Filename: xm_getopt.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 16:34:32
 * Last Modified: 2017-01-11 16:42:14
 */

#include "xm_getopt.h"
#include "xm_errno.h"
#include "xm_string.h"

#define EMSG    ""

int xm_getopt_init(xm_getopt_t **os, xm_pool_t *cont,
                                      int argc, const char *const *argv)
{
    void *argv_buff;

    *os = xm_palloc(cont, sizeof(xm_getopt_t));
    (*os)->cont = cont;
    (*os)->reset = 0;
    (*os)->errfn = (xm_getopt_err_fn_t*)(fprintf);
    (*os)->errarg = (void*)(stderr);

    (*os)->place = EMSG;
    (*os)->argc = argc;

    /* The argv parameter must be compatible with main()'s argv, since
       that's the primary purpose of this function.  But people might
       want to use this function with arrays other than the main argv,
       and we shouldn't touch the caller's data.  So we copy. */
    argv_buff = xm_palloc(cont, (argc + 1) * sizeof(const char *));
    memcpy(argv_buff, argv, argc * sizeof(const char *));
    (*os)->argv = argv_buff;
    (*os)->argv[argc] = NULL;

    (*os)->interleave = 0;
    (*os)->ind = 1;
    (*os)->skip_start = 1;
    (*os)->skip_end = 1;

    return 0;
}

int xm_getopt(xm_getopt_t *os, const char *opts, 
                                     char *optch, const char **optarg)
{
    const char *oli;  /* option letter list index */

    if (os->reset || !*os->place) {   /* update scanning pointer */
        os->reset = 0;
        if (os->ind >= os->argc || *(os->place = os->argv[os->ind]) != '-') {
            os->place = EMSG;
            *optch = os->opt;
            return (XM_EOF);
        }
        if (os->place[1] && *++os->place == '-') {        /* found "--" */
            ++os->ind;
            os->place = EMSG;
            *optch = os->opt;
            return (XM_EOF);
        }
    }                                /* option letter okay? */
    if ((os->opt = (int) *os->place++) == (int) ':' ||
        !(oli = strchr(opts, os->opt))) {
        /*
         * if the user didn't specify '-' as an option,
         * assume it means -1.
         */
        if (os->opt == (int) '-') {
            *optch = os->opt;
            return (XM_EOF);
        }
        if (!*os->place)
            ++os->ind;
        if (os->errfn && *opts != ':') {
            (os->errfn)(os->errarg, "%s: illegal option -- %c\n",
                        xm_filepath_name_get(*os->argv), os->opt);
        }
        *optch = os->opt;
        return (XM_BADCH);
    }
    if (*++oli != ':') {        /* don't need argument */
        *optarg = NULL;
        if (!*os->place)
            ++os->ind;
    }
    else {                        /* need an argument */
        if (*os->place)                /* no white space */
            *optarg = os->place;
        else if (os->argc <= ++os->ind) {        /* no arg */
            os->place = EMSG;
            if (*opts == ':') {
                *optch = os->opt;
                return (XM_BADARG);
            }
            if (os->errfn) {
                (os->errfn)(os->errarg, 
                            "%s: option requires an argument -- %c\n",
                            xm_filepath_name_get(*os->argv), os->opt);
            }
            *optch = os->opt;
            return (XM_BADCH);
        }
        else                        /* white space */
            *optarg = os->argv[os->ind];
        os->place = EMSG;
        ++os->ind;
    }
    *optch = os->opt;
    return 0;
}

/* Reverse the sequence argv[start..start+len-1]. */
static void reverse(const char **argv, int start, int len)
{
    const char *temp;

    for (; len >= 2; start++, len -= 2) {
        temp = argv[start];
        argv[start] = argv[start + len - 1];
        argv[start + len - 1] = temp;
    }
}

/*
 * Permute os->argv with the goal that non-option arguments will all
 * appear at the end.  os->skip_start is where we started skipping
 * non-option arguments, os->skip_end is where we stopped, and os->ind
 * is where we are now.
 */
static void permute(xm_getopt_t *os)
{
    int len1 = os->skip_end - os->skip_start;
    int len2 = os->ind - os->skip_end;

    if (os->interleave) {
        /*
         * Exchange the sequences argv[os->skip_start..os->skip_end-1] and
         * argv[os->skip_end..os->ind-1].  The easiest way to do that is
         * to reverse the entire range and then reverse the two
         * sub-ranges.
         */
        reverse(os->argv, os->skip_start, len1 + len2);
        reverse(os->argv, os->skip_start, len2);
        reverse(os->argv, os->skip_start + len2, len1);
    }

    /* Reset skip range to the new location of the non-option sequence. */
    os->skip_start += len2;
    os->skip_end += len2;
}

/* Helper function to print out an error involving a long option */
static int serr(xm_getopt_t *os, const char *err, const char *str,
                         int status)
{
    if (os->errfn)
        (os->errfn)(os->errarg, "%s: %s: %s\n", 
                    xm_filepath_name_get(*os->argv), err, str);
    return status;
}

/* Helper function to print out an error involving a short option */
static int cerr(xm_getopt_t *os, const char *err, int ch,
                         int status)
{
    if (os->errfn)
        (os->errfn)(os->errarg, "%s: %s: %c\n", 
                    xm_filepath_name_get(*os->argv), err, ch);
    return status;
}

int xm_getopt_long(xm_getopt_t *os,
                                          const xm_getopt_option_t *opts,
                                          int *optch, const char **optarg)
{
    const char *p;
    int i;

    /* Let the calling program reset option processing. */
    if (os->reset) {
        os->place = EMSG;
        os->ind = 1;
        os->reset = 0;
    }

    /*
     * We can be in one of two states: in the middle of processing a
     * run of short options, or about to process a new argument.
     * Since the second case can lead to the first one, handle that
     * one first.  */
    p = os->place;
    if (*p == '\0') {
        /* If we are interleaving, skip non-option arguments. */
        if (os->interleave) {
            while (os->ind < os->argc && *os->argv[os->ind] != '-')
                os->ind++;
            os->skip_end = os->ind;
        }
        if (os->ind >= os->argc || *os->argv[os->ind] != '-') {
            os->ind = os->skip_start;
            return XM_EOF;
        }

        p = os->argv[os->ind++] + 1;
        if (*p == '-' && p[1] != '\0') {        /* Long option */
            /* Search for the long option name in the caller's table. */
            size_t len = 0;

            p++;
            for (i = 0; ; i++) {
                if (opts[i].optch == 0)             /* No match */
                    return serr(os, "invalid option", p - 2, XM_BADCH);

                if (opts[i].name) {
                    len = strlen(opts[i].name);
                    if (strncmp(p, opts[i].name, len) == 0
                        && (p[len] == '\0' || p[len] == '='))
                        break;
                }
            }
            *optch = opts[i].optch;

            if (opts[i].has_arg) {
                if (p[len] == '=')             /* Argument inline */
                    *optarg = p + len + 1;
                else { 
                    if (os->ind >= os->argc)   /* Argument missing */
                        return serr(os, "missing argument", p - 2, XM_BADARG);
                    else                       /* Argument in next arg */
                        *optarg = os->argv[os->ind++];
                }
            } else {
                *optarg = NULL;
                if (p[len] == '=')
                    return serr(os, "erroneous argument", p - 2, XM_BADARG);
            }
            permute(os);
            return 0;
        } else {
            if (*p == '-') {                 /* Bare "--"; we're done */
                permute(os);
                os->ind = os->skip_start;
                return XM_EOF;
            }
            else 
                if (*p == '\0')                    /* Bare "-" is illegal */
                    return serr(os, "invalid option", p, XM_BADCH);
        }
    }

    /*
     * Now we're in a run of short options, and *p is the next one.
     * Look for it in the caller's table.
     */
    for (i = 0; ; i++) {
        if (opts[i].optch == 0)                     /* No match */
            return cerr(os, "invalid option character", *p, XM_BADCH);

        if (*p == opts[i].optch)
            break;
    }
    *optch = *p++;

    if (opts[i].has_arg) {
        if (*p != '\0')                         /* Argument inline */
            *optarg = p;
        else { 
            if (os->ind >= os->argc)           /* Argument missing */
                return cerr(os, "missing argument", *optch, XM_BADARG);
            else                               /* Argument in next arg */
                *optarg = os->argv[os->ind++];
        }
        os->place = EMSG;
    } else {
        *optarg = NULL;
        os->place = p;
    }

    permute(os);
    return 0;
}
