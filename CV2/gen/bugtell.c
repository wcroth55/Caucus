/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** BUGTELL.   Tell user about debugging information.
/
/    BUGTELL typically is used to report debugging information about a
/    file that is being locked/unlocked/opened/closed.  The arguments
/    are:
/      ACTION   action being taken (string)
/      N1       file unit number   (integer)
/      N2       status             (int4 integer)
/      NAME     name of file       (string) 
/
/    BUGTELL returns a pointer to a static char area that contains the
/    full bug message.  This is useful for sequences like:
/
/      buglist (bugtell (action, n1, n2, name));
/
/    which inform both the current user, and log the problem in the
/    bugslist file.
/  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CR  4/28/89 12:33 Bugtell now returns string, and ignores DEBUG. */
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: CR  2/09/91 21:42 Remove systype.h include. */
/*: JV 10/31/91 15:13 Declare sprintf */
/*: DE  3/31/92 12:15 Chixified */
/*: DE  4/07/92 13:41 Semi-Unchixify... revert to accept char */
/*: CX  5/21/92 16:54 Complete rewrite. */
/*: JX 10/01/92 12:21 Undeclare sprintf. */
/*: CL 12/11/92 15:52 Long sysmem arg. */

#include <stdio.h>
#include "caucus.h"

extern  int   debug;

FUNCTION  char *bugtell (action, n1, n2, name)
   char   action[], name[];
   int    n1;
   int4   n2;
{
#define  BUFSIZE 510
   static char buf[BUFSIZE+2];
   int    len;
   char  *str, *sysmem();

   /*** Allocate a place to put the entire debug string. */
   len = strlen (action) + strlen (name) + 40;
   str = sysmem (L(len), "bugtell");
   if (str == NULL)  return (NULL);

   /*** Create the debug string, remove any security information from it. */
   sprintf   (str, "%s %d %d %s\n", action, n1, n2, name);
   syssecure (str);

   /*** Write the entire debug string. */
   unit_write (XWDB, CQ(str));

   /*** Save BUF worth of the debug string, and return a ptr to it. */
   if (len > BUFSIZE)  str[BUFSIZE] = '\0';
   strcpy  (buf, str);
   sysfree (str);
   return  (buf);
}
