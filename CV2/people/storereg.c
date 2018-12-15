/*** STOREREG.   Store this user's registration file. 
/
/    PRESENT means the user is currently using Caucus, and will
/    be marked that way in the LASTON field.  Otherwise, the user
/    is leaving Caucus. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR  6/28/89 12:33 Use peo_Tnow to get "  (now)" string. */
/*: JV  8/06/90 19:55 Add Site-Dependent Reg Info. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: CR  1/07/92 15:00 Write new form of SET options. */
/*: CR  1/09/92 13:20 Add sys opt stuff. */
/*: CR  1/30/92 10:32 Break up int4 sprintf(). */
/*: DE  4/29/92 12:09 Chixified */
/*: JX  5/19/92 19:43 Fix Chix. */
/*: JV 12/04/92 13:48 Add sitevar arg. */
/*: CP  8/17/93 14:16 Add 'showattach', 'terminal' members. */
/*: JV  9/07/93 11:09 Add 'transfer'. */
/*: CR 10/24/95 14:00 If possible, use reg->id for userid. */
/*: CR 10/12/05 Remove mdwrite() call. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  storereg  (reg, present, sitevar)
   Userreg reg;
   int     present;
   Chix    sitevar[];
{
   static Chix  s_now;
   struct namlist_t *n;
   Chix   userid, setopts, day, time, daytime;
   int4   systime();
   short  snum;
   int    success;

   ENTRY ("storereg", "");

   userid   = chxalloc (L(40),  THIN, "storereg userid");
   setopts  = chxalloc (L(160), THIN, "storereg setopts");
   day      = chxalloc (L(30),  THIN, "storeuse day");
   time     = chxalloc (L(30),  THIN, "storeuse time");
   daytime  = chxalloc (L(60),  THIN, "storeuse daytime");
   success  = 0;

   chxcpy (userid, reg->id);
   if (EMPTYCHX (userid))  sysuserid (userid);
   if (NOT unit_lock (XURG, WRITE, L(0), L(0), L(0), L(0), userid))    FAIL;
   if (NOT unit_make (XURG))            { unit_unlk  (XURG);   FAIL; }

   unit_write (XURG, reg->name);      unit_write (XURG, CQ("\n"));
   unit_write (XURG, reg->lastname);  unit_write (XURG, CQ("\n"));
   chxformat (setopts, CQ("%d %d "), L(reg->debug), L(reg->lines), null.chx,
             null.chx);
   chxformat (setopts, CQ("%d %d %s "), L(reg->width), L(1-reg->truncate),
              reg->editor, null.chx);
   chxformat (setopts, CQ("%d %d "), L(reg->tablength), L(reg->newpage), 
              null.chx, null.chx);
   chxformat (setopts, CQ("%d %d "), L(reg->printcode), L(reg->verbose), 
              null.chx, null.chx);
   chxformat (setopts, CQ("%d "), L(reg->dict), L(0), null.chx, null.chx);
   unit_write (XURG, setopts);
   unit_write (XURG, (EMPTYCHX(reg->eot) ? chxquick("2return",1) 
                 : (chxeq (reg->eot, chxquick("$endfile$",2)) ? 
                    chxquick("1eof",3) : reg->eot)) );
   chxclear  (setopts);
   chxformat (setopts, CQ(" %d %d "), L(reg->expwarn), L(reg->texten), 
              null.chx, null.chx);
   chxformat (setopts, CQ("%d %d %s "), L(reg->receiptsize), L(reg->mytext),
             (EMPTYCHX(reg->startmenu) ? chxquick("off",1) : reg->startmenu),
             null.chx);
   chxformat (setopts, CQ("%d %d %s"), L(reg->import_mail), L(reg->export_mail),
                reg->terminal, nullchix);
   chxformat (setopts, CQ(" %d %s"), L(reg->showattach), L(0),
                       reg->transfer, nullchix);
   unit_write (XURG, setopts);        unit_write (XURG, CQ("\n"));
   chxclear  (setopts);
   chxformat (setopts, CQ("%d"), L(reg->sysi1), L(0), null.chx, null.chx);
   unit_write (XURG, setopts);        unit_write (XURG, CQ("\n"));
   unit_write (XURG, reg->phone);     unit_write (XURG, CQ("\n"));
   sysdaytime (day, time, 0, systime());
   chxformat  (daytime, CQ("%s %s"), L(0), L(0), day, time);
   unit_write (XURG, daytime);
   if (present)  unit_write (XURG, CQ("  (now)"));
   unit_write (XURG, CQ("\n"));

   /* Write out Site-Dependent Reg Info */
   unit_write (XURG, CQ("=\n"));                     /* Preface to SDRI. */
   for (snum = 0; snum < 10; snum++) {
      unit_write (XURG, CQ(" "));
      unit_write (XURG, sitevar[snum]);
      unit_write (XURG, CQ("\n"));
   }

   for (n=reg->briefs->next;   n!=null.nl;   n = n->next) {
      unit_write (XURG, CQ(" "));
      unit_write (XURG, n->str);
      unit_write (XURG, CQ("\n"));
   }
   unit_write (XURG, CQ("+\n"));

   for (n=reg->print->next;   n!=null.nl;   n = n->next) {
      unit_write (XURG, n->str);
      unit_write (XURG, CQ("\n"));
   }

   unit_close (XURG);
   unit_unlk  (XURG);

   SUCCEED;

 done:

   chxfree ( userid );
   chxfree ( setopts );
   chxfree ( day );
   chxfree ( time );
   chxfree ( daytime );

   RETURN ( success );
}
