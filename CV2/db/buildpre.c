/*** BUILD_PREFACE.  Build a message "preface" line.
/
/    build_preface (preface, mnum, messtype, author, authorid, authorchn,
/                       date, time, cmi);
/
/    Parameters:
/       Chix   preface;     (place to put assembled preface)
/       int    mnum;        (message number)
/       int    messtype;    (message type)
/       Chix   author;      (author name)
/       Chix   authorid;    (author userid)
/       Chix   authorchn;   (author host name)
/       Chix   date;        (date of message)
/       Chix   time;        (time of message)
/       int    cmi;         (1=>cmi message, 0=>regular caucus mail)
/
/    Purpose:
/       Each "part" of a Caucus message begins with a preface line
/       that looks like:
/         35) MESSAGE from Charles Roth (roth)   4-JUL-91 20:13 ...
/
/       Build_preface() builds that preface line.
/
/    How it works:
/
/    Returns: 1.  Always.
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  add_dmess(), messparse()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/buildpre.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/18/91 21:40 New function. */
/*: CR  1/10/92 21:49 Hard-code "MESSAGE_from..." string. */
/*: CR  7/24/92 11:59 Remove 35 char limit to person's "signature". */
/*: DE  4/10/92 14:15 Converted sysfree -> chxfree */
/*: JX  5/26/92 13:50 Streamline chix. */
/*: CI 10/07/92 22:27 Chix Integration. */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CP  6/15/93 15:46 Remove 35 character limit for good! */

#include <stdio.h>
#include "caucus.h"

extern union null_t  null;

FUNCTION  build_preface (preface, mnum, messtype, author, authorid, authorchn,
                        date, time, cmi)
   Chix   preface, author, authorid, authorchn, date, time;
   int    mnum, messtype, cmi;
{
   Chix   type, format, signature;
   Chix   person_format();
   int    success;

   ENTRY ("build_preface", "");

   type     = chxalloc (L(150), THIN, "buildpre type");
   format   = chxalloc (L(300), THIN, "buildpre format");

   /*** Start assembling the parts of the PREFACE.  Get the "signature", the
   /    author's name. */
   if (cmi) {
      signature = chxalloc (L(32), THIN, "buildpre sign");
      chxcpy (signature, authorid);
   }
   else {
      mdstr (format, "mes_Fname", null.md);
      signature = person_format (format, author, authorid, authorchn, null.chx,
                                 null.nl);
   }

   /*** Get the type of message.  Assemble the full PREFACE from
   /    message number, type, signature, date, and time. */
   chxtoken (type, null.chx, messtype+1,
   CQ ("MESSAGE_from  FORWARDED_by  REPLY___from  MESSAGE_from  MESSAGE_from"));
   while     (chxalter (type, L(0), chxquick("_",0), chxquick(" ",1))) ;
   chxformat (preface, CQ("%3d) %-12s %s"), L(mnum), L(0), type, signature);
   chxformat (preface, CQ(" %s %s\n"), L(0), L(0), date, time );

   SUCCEED;

done:

   chxfree (type);
   chxfree (format);
   chxfree (signature);

   RETURN  (success);
}
