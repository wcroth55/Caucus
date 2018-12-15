
/*** SHOW_HEADER.   Display item header and title.
/
/    ok = show_header (out, item, str, title, brief);
/
/    Parameters:
/       int   ok;          (success?)
/       int   out;         (Unit number for output)
/       int   item;        (item number)
/       Chix  str;         (header string -- IGNORED!)
/       Chix  title;       (item title, may be 'nullchix')
/       int   brief;       (display in brief form?)
/ 
/    Purpose:
/       Display the header and title of an item.
/
/    How it works:
/       Display the header and title of item number ITEM on unit OUT.
/       (If TITLE is supplied, use it; otherwise get it from the
/       actual item data on disk.)  If BRIEF is true, display a "brief"
/       form, otherwise display the "verbose" form.
/
/    Returns: 0   if the response does not exist
/             1   on successful completion
/
/    Side effects:      none
/
/    Related functions: uses show_format()
/
/    Called by:
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  resp/showhead.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: AM  2/07/89 10:32 Jcaucus changes*/
/*: CR  4/27/89 14:06 Make insensitive to null author. */
/*: CR  7/26/89 12:33 Expand TIME[] to handle int4 1st name of author. */
/*: CR  8/09/90 19:04 Use show_format(). */
/*: CX 10/20/91 16:25 Chixify. */
/*: JV  6/08/93 16:21 Don't need title, start lines at 0. */
/*: CP  6/30/93 17:26 Add 'resp2' arg to show_format() call. */
/*: CP  8/01/93 10:50 Add a_mak...() name arg. */
/*: CP 11/11/93 13:37 Add 'resp1' arg to show_format() call. */
/*: CP 11/17/93 17:54 Add hdr comments, ignore STR arg. */

#include <stdio.h>
#include "caucus.h"
#include "api.h"

extern struct master_template master;
extern struct this_template   thisitem;
extern union  null_t          null;

FUNCTION  show_header (out, item, str, title, brief)
   int    out, item, brief;
   Chix   str, title;
{
   static Chix s_head1 = nullchix;
   static Chix s_head2 = nullchix;
   Chix   temp;
   int    success;
   Resp   resp;

   ENTRY ("show_head", "");

   /*** Set the frozen indicator for this item. */
   resp = a_mak_resp("showhead resp");
   resp->cnum = master.cnum;
   resp->inum = item;
   resp->rnum = 0;
   if (a_get_resp (resp, P_FROZEN, A_WAIT) != A_OK) FAIL;
   thisitem.frozen = resp->frozen;

   temp = chxalloc (L(80), THIN, "showhead temp");

   if (brief) mdstr (temp, "res_Fshowhead2", &s_head2);
   else       mdstr (temp, "res_Fshowhead1", &s_head1);
   success = show_format (out, temp, item, 0, 0, 0, 0, title, str);
   chxfree (temp);

   if (sysbrktest())  { sysbrkclear();   FAIL;}

done:
   a_fre_resp (resp);
   RETURN (success);
}
