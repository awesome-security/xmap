/*
 *
 *      Filename: xm_getopt.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 16:31:23
 * Last Modified: 2017-01-11 16:31:23
 */

#ifndef XM_GETOPT_H
#define XM_GETOPT_H

typedef void (xm_getopt_err_fn_t)(void *arg, const char *err, ...);

/** @see xm_getopt_t */
typedef struct xm_getopt_t xm_getopt_t;

#include "xm_mpool.h"

/**
 * Structure to store command line argument information.
 */ 
struct xm_getopt_t {
    /** context for processing */
    xm_pool_t *cont;
    /** function to print error message (NULL == no messages) */
    xm_getopt_err_fn_t *errfn;
    /** user defined first arg to pass to error message  */
    void *errarg;
    /** index into parent argv vector */
    int ind;
    /** character checked for validity */
    int opt;
    /** reset getopt */
    int reset;
    /** count of arguments */
    int argc;
    /** array of pointers to arguments */
    const char **argv;
    /** argument associated with option */
    char const* place;
    /** set to nonzero to support interleaving options with regular args */
    int interleave;
    /** start of non-option arguments skipped for interleaving */
    int skip_start;
    /** end of non-option arguments skipped for interleaving */
    int skip_end;
};

/** @see xm_getopt_option_t */
typedef struct xm_getopt_option_t xm_getopt_option_t;

/**
 * Structure used to describe options that getopt should search for.
 */
struct xm_getopt_option_t {
    /** long option name, or NULL if option has no long name */
    const char *name;
    /** option letter, or a value greater than 255 if option has no letter */
    int optch;
    /** nonzero if option takes an argument */
    int has_arg;
    /** a description of the option */
    const char *description;
};

/**
 * Initialize the arguments for parsing by xm_getopt().
 * @param os   The options structure created for xm_getopt()
 * @param cont The pool to operate on
 * @param argc The number of arguments to parse
 * @param argv The array of arguments to parse
 * @remark Arguments 3 and 4 are most commonly argc and argv from main(argc, argv)
 * The (*os)->errfn is initialized to fprintf(stderr... but may be overridden.
 */
int xm_getopt_init(xm_getopt_t **os, xm_pool_t *cont,
                                      int argc, const char * const *argv);

/**
 * Parse the options initialized by xm_getopt_init().
 * @param os     The xm_opt_t structure returned by xm_getopt_init()
 * @param opts   A string of characters that are acceptable options to the 
 *               program.  Characters followed by ":" are required to have an 
 *               option associated
 * @param option_ch  The next option character parsed
 * @param option_arg The argument following the option character:
 * @return There are four potential status values on exit. They are:
 * <PRE>
 *             XM_EOF      --  No more options to parse
 *             XM_BADCH    --  Found a bad option character
 *             XM_BADARG   --  No argument followed the option flag
 *             XM_SUCCESS  --  The next option was found.
 * </PRE>
 */
int xm_getopt(xm_getopt_t *os, const char *opts, 
                                     char *option_ch, const char **option_arg);

/**
 * Parse the options initialized by xm_getopt_init(), accepting long
 * options beginning with "--" in addition to single-character
 * options beginning with "-".
 * @param os     The xm_getopt_t structure created by xm_getopt_init()
 * @param opts   A pointer to a list of xm_getopt_option_t structures, which
 *               can be initialized with { "name", optch, has_args }.  has_args
 *               is nonzero if the option requires an argument.  A structure
 *               with an optch value of 0 terminates the list.
 * @param option_ch  Receives the value of "optch" from the xm_getopt_option_t
 *                   structure corresponding to the next option matched.
 * @param option_arg Receives the argument following the option, if any.
 * @return There are four potential status values on exit.   They are:
 * <PRE>
 *             XM_EOF      --  No more options to parse
 *             XM_BADCH    --  Found a bad option character
 *             XM_BADARG   --  No argument followed the option flag
 *             XM_SUCCESS  --  The next option was found.
 * </PRE>
 * When XM_SUCCESS is returned, os->ind gives the index of the first
 * non-option argument.  On error, a message will be printed to stdout unless
 * os->err is set to 0.  If os->interleave is set to nonzero, options can come
 * after arguments, and os->argv will be permuted to leave non-option arguments
 * at the end (the original argv is unaffected).
 */
int xm_getopt_long(xm_getopt_t *os,
					  const xm_getopt_option_t *opts,
					  int *option_ch,
                                          const char **option_arg);

#endif /* XM_GETOPT_H */
