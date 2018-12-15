/*** FROZEN.  Is this ITEM frozen?
/
/    cold = frozen (cnum, item, tell_user);
/
/    Parameters:
/       int    cold;        (return value)
/       int    cnum;        (conference number)
/       int    item;        (item number)
/       int    tell_user;   (tell the user about it?)
/
/    Purpose:
/       Determine if this item is frozen.  Frozen items may not have
/       any responses added.  Items may be frozen by the author or
/       organizer, or may automatically be frozen by Caucus if the
/       item reaches the max # of responses or partfiles.
/
/    How it works:
/       The frozen flag is stored in the item header, so we get at
/       it with load_header().  If TELL_USER is true, FROZEN tells
/       the user that this item is frozen.
/
/    Returns: 1 if frozen, 0 otherwise.
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  resp/frozen.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CW  5/18/89 15:43 Add XW unit codes. */
/*: CX 10/17/91 12:56 Chixify. */
/*: JV  1/16/92 18:38 Add arg CNUM. */
/*: CR  1/20/92 15:41 Add cnum arg. */
/*: DE  4/20/92 14:01 Add cnum arg to load_header call */
/*: CI 10/07/92 22:54 Chix Integration. */
/*: JV  4/02/93 11:29 Add attach arg to load_header(). */
/*: CR 10/12/05 Remove "tell_user" function, no terminal to write to! (C5) */

#include <stdio.h>
#include "caucus.h"

extern struct this_template thisitem;
extern union  null_t        null;

FUNCTION  frozen (cnum, item, tell_user)
   int    cnum, item, tell_user;
{
   Chix   str, title;

   ENTRY ("frozen", "");

   if (cnum != thisitem.cnum || item != thisitem.number) {
      str   = chxalloc (L(80), THIN, "frozen str");
      title = chxalloc (L(80), THIN, "frozen title");
      load_header (cnum, item, str, title, (Attachment) NULL);
      chxfree (str);
      chxfree (title);
   }

   RETURN (thisitem.frozen);
}
