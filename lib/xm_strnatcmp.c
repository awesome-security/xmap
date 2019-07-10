/*
 *
 *      Filename: xm_strnatcmp.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 17:02:27
 * Last Modified: 2017-01-09 17:15:06
 */

#include "xm_string.h"

/* based on "strnatcmp.c,v 1.6 2000/04/20 07:30:11 mbp Exp $" */

static int
compare_right(char const *a, char const *b)
{
     int bias = 0;
     
     /* The longest run of digits wins.  That aside, the greatest
	value wins, but we can't know that it will until we've scanned
	both numbers to know that they have the same magnitude, so we
	remember it in BIAS. */
     for (;; a++, b++) {
	  if (!xm_isdigit(*a)  &&  !xm_isdigit(*b))
	       break;
	  else if (!xm_isdigit(*a))
	       return -1;
	  else if (!xm_isdigit(*b))
	       return +1;
	  else if (*a < *b) {
	       if (!bias)
		    bias = -1;
	  } else if (*a > *b) {
	       if (!bias)
		    bias = +1;
	  } else if (!*a  &&  !*b)
	       break;
     }

     return bias;
}


static int
compare_left(char const *a, char const *b)
{
     /* Compare two left-aligned numbers: the first to have a
        different value wins. */
     for (;; a++, b++) {
	  if (!xm_isdigit(*a)  &&  !xm_isdigit(*b))
	       break;
	  else if (!xm_isdigit(*a))
	       return -1;
	  else if (!xm_isdigit(*b))
	       return +1;
	  else if (*a < *b)
	       return -1;
	  else if (*a > *b)
	       return +1;
     }
	  
     return 0;
}


static int strnatcmp0(char const *a, char const *b, int fold_case)
{
     int ai, bi;
     char ca, cb;
     int fractional, result;
     ai = bi = 0;
     while (1) {
	  ca = a[ai]; cb = b[bi];

	  /* skip over leading spaces or zeros */
	  while (xm_isspace(ca))
	       ca = a[++ai];

	  while (xm_isspace(cb))
	       cb = b[++bi];

	  /* process run of digits */
	  if (xm_isdigit(ca)  &&  xm_isdigit(cb)) {
	       fractional = (ca == '0' || cb == '0');

	       if (fractional) {
		    if ((result = compare_left(a+ai, b+bi)) != 0)
			 return result;
	       } else {
		    if ((result = compare_right(a+ai, b+bi)) != 0)
			 return result;
	       }
	  }

	  if (!ca && !cb) {
	       /* The strings compare the same.  Perhaps the caller
                  will want to call strcmp to break the tie. */
	       return 0;
	  }

	  if (fold_case) {
	       ca = xm_toupper(ca);
	       cb = xm_toupper(cb);
	  }
	  
	  if (ca < cb)
	       return -1;
	  else if (ca > cb)
	       return +1;

	  ++ai; ++bi;
     }
}



int xm_strnatcmp(char const *a, char const *b)
{
     return strnatcmp0(a, b, 0);
}


/* Compare, recognizing numeric string and ignoring case. */
int xm_strnatcasecmp(char const *a, char const *b)
{
     return strnatcmp0(a, b, 1);
}

