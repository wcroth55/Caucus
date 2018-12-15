/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** LOG_USER.  Record in a log when a user enters or leaves Caucus. 
/ 
/    'n' is the unit number of the logfile,
/    'in_out' determines whether person is entering or
/       leaving Caucus or the conference.
/       if 1, mark the person as entering,
/       if 2, mark the person as exiting.
*/

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CR  2/11/89 15:49 Generalize log_user() for use by -l or -u options. */
/*: JV 12/12/89 10:10 Added description to header comments. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: DE  4/13/92 14:08 Chixified */
/*: JX  6/06/92 19:53 Add FORMAT arg to sysdaytime(). */

#include "caucus.h"
#include <stdio.h>

extern struct flag_template flag;
extern union  null_t        null;
extern short                confnum;

FUNCTION  log_user (n, in_out)
   int    n, in_out;
{
   Chix   name, date, time, daytime, newstr, format;
   int4   systime();

   ENTRY ("loguser", "");

   name      = chxalloc (L(40),  THIN, "loguser name");
   date      = chxalloc (L(40),  THIN, "loguser date");
   time      = chxalloc (L(40),  THIN, "loguser time");
   daytime   = chxalloc (L(80),  THIN, "loguser daytime");
   newstr    = chxalloc (L(160), THIN, "loguser newstr");
   format    = chxalloc (L(160), THIN, "loguser format");

   sysuserid (name);
   sysdaytime   (date, time, 0, systime());

   mdstr   (format, (in_out ? "gen_Tloguin" : "gen_Tloguout"), null.md);
   chxformat (daytime, CQ("%s %s"), L(0), L(0), date, time);
   chxformat (newstr, format, L(0), L(0), daytime, name);

   unit_lock   (n, WRITE, L(confnum), L(confnum), L(0), L(0), null.chx);
/*   unit_open   (n); */
   unit_append (n);
   unit_write  (n, newstr);
   unit_close  (n);
   unit_unlk   (n);

   sysconsole  (newstr);

   chxfree ( name );
   chxfree ( date );
   chxfree ( time );
   chxfree ( daytime );
   chxfree ( newstr );
   chxfree ( format );

   RETURN (1);
}
