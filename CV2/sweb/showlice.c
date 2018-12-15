/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SHOWLICENSE.   Display the Caucus license information. */

/*: XX 11/30/91 17:12 Change release date string to 30-NOV-91. */
/*: JV 12/20/91 17:38 Change release date string to 20-DEC-91. */
/*: CR  1/01/92 15:21 Change release date string to  1-JAN-92. */
/*: JV  1/20/92 17:45 Change release date string to 20-JAN-92. CauLink 1.1 */
/*: JX  5/27/92 16:41 Chixify. */
/*: JX  8/01/92 11:11 Change release date string to  1-AUG-92. */
/*: CI 10/05/92 15:54 Chix Integration. */
/*: CR 10/09/92 18:20 Change release date string to 31-OCT-92. */
/*: CR 11/24/92 11:44 Change release date string to 26-NOV-92. */
/*: CR 12/16/92 16:14 Change release date string to 25-DEC-92. */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */
/*: CR  2/01/93 15:33 Change release date string to  1-JAN-93. */
/*: CP  3/07/93 13:48 Change release date string to  1-MAR-93. */
/*: JV  6/01/93 14:16 Change release date string to  1-JUN-93. */
/*: CK 10/13/93 17:43 Add confmax, itemmax, respmax display. */
/*: CR 10/17/93 17:21 Change release date string to  1-NOV-93. */
/*: CR 12/22/93 15:14 Change release date string to  1-JAN-94. */
/*: CR  5/05/94 13:48 Change release date string to  1-MAY-94. */
/*: CR  7/12/94 14:42 Change release date string to 15-JUL-94. */
/*: CR 10/30/95 10:59 Add totalusers. */

#include <stdio.h>
#include "caucus.h"

extern union  null_t    null;

FUNCTION  showlicense (usedic)
   int    usedic;
{
   short  i, any;
   int4   maxusers, totalusers, systime();
   char   temp[100];
   Chix   str, mtu, format;
   Chix   day, time;
   char  *strplain();

   ENTRY ("showlicense", "");

   mtu    = chxalloc (L(80), THIN, "showlice mtu");
   str    = chxalloc (L(20), THIN, "showlice str");
   format = chxalloc (L(40), THIN, "showlice format");
   day    = chxalloc (L(30), THIN, "showlice day");
   time   = chxalloc (L(30), THIN, "showlice time");

   sysversion (XWTX, usedic);
   if (usedic)       usedic = mdstr (mtu, "lic_Trelease", null.md);
   if (NOT usedic)   chxcpy         (mtu, CQ("Release date : "));
   unit_write (XWTX, mtu);
   unit_write (XWTX, CQ("1-MAR-96\n"));

   /*** Tell user which system this is. */
   if (NOT usedic  ||  NOT mdstr (format, "lic_Fsystem", null.md))
          chxcpy (format, CQ("This system  : %s\n"));
   systemid (str);
   chxclear   (mtu);
   chxformat  (mtu, format, L(0), L(0), str, nullchix);
   unit_write (XWTX, mtu);

   /*** Tell user current time... */
   if (NOT usedic  ||  NOT mdstr (mtu, "lic_Tnow", null.md))
      chxcpy (mtu, CQ("It is now    : "));
   unit_write (XWTX, mtu);
   sysdaytime(day, time, 0, systime());
   chxclear  (mtu);
   chxformat (mtu, CQ("%s %s"), L(0), L(0), day, time);
   unit_write (XWTX, mtu);


   chxfree (mtu);
   chxfree (str);
   chxfree (format);
   chxfree (day);
   chxfree (time);

   RETURN (1);
}
