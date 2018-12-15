/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** CWIN.   Caucus Windows.  Definition of system-dependent window stuff. */

/*: CR 05/19/89 14:05 Definition of Caucus windows. */

#define  PF_NULL    0
#define  PF_HOME    1
#define  PF_PGUP    2
#define  PF_PGDN    3
#define  PF_END     4
#define  PF_CANCEL  5
#define  PF_HELP    6

#if PX
#include <curses.h>
#endif

struct cwin_t {
#if PX
   WINDOW  *win;      /* Pointer to curses window. */
   char   **text;     /* Array of ptrs to lines of text. */
#endif
#if CVM
   void    *win;      /* Actual panel window. */
   int      f0;       /* Number of first field stored used by window. */
   int      f1;       /* Number of last  field stored used by window. */
#endif
   int      lines;    /* Number of lines stored in TEXT. */
   int      top;      /* Number of line displayable at top of window. */
   int      height;   /* Height of this window in lines. */
   int      p;        /* Assembly area buffer pointer. */
   char     buf[100]; /* Assembly area for next line of text. */
   char     open;     /* Is this window open? */
   char     gotobot;  /* Heading for bottom of window? */
   char     jump;     /* Jump window ahead a half page if warranted. */
};
