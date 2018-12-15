/*** PRINT_IT.  Actually print the file we've been accumulating stuff
/    to print in. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: JV  8/24/88 14:49 Checked for pcd=null, to increase robustness.*/
/*: CR 11/04/88 13:41 Change sysrmfile(file) -> sysrmfile(pcd->str). */
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: CR 10/09/89 14:18 Remove "char file[]". */
/*: JV  3/23/90 12:12 Add new argument to syscheck (this is not an argument...*/
/*: CR  3/23/90 16:42 Use sysprv(), replace sysrmfile() with sysunlink(). */
/*: DE  4/01/92 18:00 Chixified */
/*: DE  6/02/92 10:46 Fixed inconsistant args */
/*: JV  3/18/93 12:34 Add WAIT arg to syscmd. */

#include "caucus.h"
#include "sys.h"
#include <stdio.h>

extern struct flag_template  flag;
extern Userreg               thisuser;
extern struct namlist_t     *sysmgrprint;
extern union  null_t         null;
extern Chix                  yesnotable;

FUNCTION  print_it()
{
   struct namlist_t *pcd;
   Chix   what, yes, newstr;
   int    c, pcd_exists;
   int    success;

   ENTRY ("printit", "");

   what      = chxalloc (L(20), THIN, "printit what");
   yes       = chxalloc (L(20), THIN, "printit yes");
   newstr    = chxalloc (L(80), THIN, "printit newstr");
   success   = 0;

   /*** Get a namelist containing the printing instructions.  If the
   /    user is not allowed his/her specific instructions, use the
   /    system default. */
   pcd = (flag.print == 1  ||  thisuser->print->next == null.nl
               ? sysmgrprint : thisuser->print);

   /*** Make sure the printfile (containing the stuff to be printed)
   /    exists.  If so, go ahead and print it! */
   pcd = pcd->next;
   if (pcd == null.nl) { /* if pcode instr. not there, don't crash! */
      mdstr (newstr, "new_Pnomgr", null.md);
      unit_write (XWER, newstr);
      FAIL;
   }

   sysprv(0);
   pcd_exists = syscheck (ascquick(pcd->str));
   sysprv(1);

   if (pcd_exists) {
      chxtoken (yes, null.chx, 2, yesnotable);
      c = mdprompter ("new_Pokprint", yesnotable, 2, "new_Hokprint", 
                       20, what, yes);

      if (c > 0)  for (pcd = pcd->next;   pcd!=null.nl;   pcd = pcd->next)
         syscmd    (ascquick(pcd->str), SYS_WAIT);
      else {
         sysprv(0);
         sysunlink (ascquick(pcd->str));
         sysprv(1);
      }
   }

   SUCCEED;

 done:

   chxfree ( what );
   chxfree ( yes );
   chxfree ( newstr );

   RETURN ( success );
}
