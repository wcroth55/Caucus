/*** SHOW_RESPONSES.  Display responses RESP0 through RESP1.
/
/    show_responses (out, cnum, item, resp0, resp1, backwards, brief, 
/                    attach, shown, onlyattach);
/
/    Parameters:
/       int    out;        (display on this unit)
/       int    cnum;       (conference number)
/       int    item;       (item number)
/       int    resp0;      (low end of display range)
/       int    resp1;      (hi  end of display range)
/       int    backwards;  (0 => display low->hi;    1 => display hi->lo)
/       int    brief;      (0 => int4 form;          1 => "brief" form)
/       int    attach;     (0=>no attach, 1=>show attach, 2=>askme)
/       int   *shown;      (returned: number of complete responses displayed)
/       int    onlyattach; (1=>only show responses with attachments)
/
/    Purpose:
/       Display a set of responses to a particular item.
/
/    How it works:
/       Displays responses RESP0 through RESP1 (or RESP1 through RESP0
/       if BACKWARDS is true) to ITEM on unit OUT.
/
/       If a particular response has an attachment, decide what to do
/       with it based on the ATTACH argument.
/
/       If ONLYATTACH is true, just show responses that have attachments.
/       (Note: this is sort of awkward.  The selection of such responses
/        ought to come from higher up, during the command line parsing
/        and Rlist selection.  However, that would be extremely
/        inefficient...)
/
/    Returns: -1 on error
/             else number of highest numbered response shown.
/
/    Error Conditions:
/ 
/    Side effects:
/       Updates global thisitem.rnum.
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  resp/showresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: AM  2/07/89 10:32 Jcaucus changes*/
/*: CR  4/27/89 14:06 Make insensitive to null author. */
/*: CW  5/18/89 15:43 Add XW unit codes. */
/*: CR  9/05/89 15:00 Add DIRECTION control argument. */
/*: CR 10/11/89 15:24 Update SEEN for deleted responses. */
/*: CR 10/18/89 17:40 Write "Deleted responses" message to unit OUT! */
/*: CR  8/07/90 10:03 Use show_format() to display each response. */
/*: CR  8/29/90 14:31 Remove unused stuff. */
/*: CX 10/20/91 16:35 Chixify. */
/*: CR  9/08/92 18:23 Add SHOWN argument.  */
/*: CI 10/08/92 16:33 Chix Integration. */
/*: JV  6/08/93 16:10 Change show_format(title & header); start at line 0. */
/*: CP  6/30/93 17:26 Add 'resp2' arg to show_format() call. */
/*: CP  7/01/93 16:01 Add 'brief' arg. */
/*: JJ  7/21/93 10:56 Update thisitem.rnum. */
/*: CP  8/24/93 15:30 Add 'attach' arg. */
/*: CP  8/27/93 16:39 Skip attachments if BRIEF. */
/*: CK  9/18/93 23:40 Skip "deleted responses" message if string empty. */
/*: CK  9/19/93  0:04 Add 'onlyattach' arg. */
/*: CP 11/11/93 13:37 Add 'resp1' arg to show_format() call. */

#include <stdio.h>
#include "caucus.h"
#include "api.h"
#include "redirect.h"

extern union  null_t        null;
extern struct this_template thisitem;

FUNCTION  show_responses (out, cnum, item, resp0, resp1, backwards, brief, 
                          attach, shown, onlyattach)
   int    out, cnum, item, resp0, resp1, backwards, brief, attach, onlyattach;
   int   *shown;
{
   static Chix s_respsep  = nullchix;
   Chix        format, temp;
   int         r0, r1, r, inc, success;
   short       deleted, seen;
   Resp        respobj;

   ENTRY ("showresp", "");

  *shown = 0;
   if (resp0 > resp1)  RETURN (-1);

   deleted = seen = 0;
   if (backwards)  { r0 = resp1;   r1 = resp0;   inc = -1; }
   else            { r0 = resp0;   r1 = resp1;   inc =  1; }
   r = r0 - inc;

   /*** Display each response. */
   format  = chxalloc (L(80), THIN, "showresp format");
   respobj = a_mak_resp ("showresp");
   respobj->cnum = cnum;
   respobj->inum = item;
   mdstr (format, (brief ? "res_Fshowbrief" : "res_Fshowresp"), null.md);
   do {
      r += inc;

      thisitem.rnum = r;

      /*** If ONLYATTACH, display just those responses that have 
      /    attachments. */
      if (onlyattach) {
         respobj->rnum = r;
         if (a_get_resp (respobj, P_TITLE, A_WAIT) != A_OK)  continue;
         if (respobj->attach == (Attachment) NULL  ||
             respobj->attach->size <= 0)                     continue;
      }

      /*** Display this response according to the format string.  If
      /    it fails, the response must have been deleted. */
      if (NOT show_format (out, format, item, r, r, 0, 0, nullchix, nullchix)) {
         ++deleted;
         if (r > seen)  seen = r;
         continue;
      }

      if (sysbrktest())  DONE (seen);

      ++(*shown);
      if (r > seen)                seen = r;
      if (sysbrktest())            DONE (seen);
      if (brief)                   continue;

      /*** If there is an attachment, does/can the user want to see it? */
      if (ok_show_attachment (cnum, item, r, (Redirect *) NULL, 0, 0, attach))
         show_attachment (cnum, item, r, (Redirect *) NULL, 0);
      if (sysbrktest())            DONE (seen);

      if (r != r1  &&  NOT brief)  mdwrite (out, "res_Tsep", &s_respsep);
   } while (r != r1);

   if (deleted) {
      mdstr (format, "res_Fdeld", null.md);
      if (NOT EMPTYCHX(format)) {
         temp = chxalloc (L(80), THIN, "showresp temp");
         chxformat  (temp, format, (int4) deleted, L(0), nullchix, nullchix);
         unit_write (out,  temp);
         chxfree    (temp);
      }
   }
   DONE (seen);

done:
   chxfree    (format);
   a_fre_resp (respobj);
   RETURN     (success);
}
