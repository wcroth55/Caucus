
/*** REMOVEID.  Remove the "(userid)" part from a user's name. 
/
/    removeid (authorname, name, userid, host);
/
/    Parameters:
/       Chix  authorname;   Resulting author name, no "(userid)".
/       Chix  name;         Supplied full name, prob. including "(userid)".
/       Chix  userid;       Supplied userid of person.
/       Chix  host;         Supplied host name.
/
/    Purpose:
/       REMOVEID takes the "(userid)" or "(userid@host)" out of NAME,
/       removes excess blanks, and puts the result in AUTHORNAME.
/
/    How it works:
/       Builds "(userid)" out of USERID and the external strings ss_luid
/       and ss_ruid[].  Copies NAME to AUTHORNAME, removes "(userid)".
/
/    Returns: 1
/
/    Error Conditions:  none
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  show_format() and similar "tailorable display" functions.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  people/removeid.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/29/91 17:11 New function. */
/*: CR  6/19/91 16:33 Add host argument. */
/*: CR 10/10/91 22:51 Parse even if missing final ')'. */
/*: CR 11/20/91 14:33 Don't over-write HOST. */
/*: DE  4/27/92 17:52 Chixified */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CR 10/12/01 16:40 Ignore @'s!  Replace with "^" for now. */

#include "caucus.h"
#include <stdio.h>

extern Chix  ss_luid, ss_ruid;
extern union null_t   null;

FUNCTION  removeid (authorname, name, userid, host)
   Chix  authorname, name, userid, host;
{
   Chix   authorid, id, chn, tempchx;
   int4   n, at;

   ENTRY ("removeid", "");

   authorid   = chxalloc (L(80),  THIN, "removeid authorid");
   id         = chxalloc (L(80),  THIN, "removeid id");
   chn        = chxalloc (L(100), THIN, "removeid chn");

   /*** Attempt to find the "@host" in either NAME or USERID. 
   /    While we're at it, get the plain old userid into ID. */
   chxbreak (userid, id, chn, CQ("^"));
   if (EMPTYCHX(chn))  chxcpy (chn, host);
   if (EMPTYCHX(chn)  &&  (at = chxindex (name, L(0), CQ("^"))) >= 0) {
      chxclear  (chn);
      chxcatsub (chn, name, at+1, ALLCHARS);
      if ( (n = chxindex (chn, L(0), ss_ruid)) >= 0)  {
         tempchx = chxalsub (chn, L(0), n);
         chxcpy (chn, tempchx);
         chxfree ( tempchx );
       }
   }

   /*** Remove "(userid)" or "(userid@host)" from NAME. */
   chxcpy    (authorname, name);
   chxclear  (authorid);
   chxformat (authorid, CQ("%s%s"), L(0), L(0), ss_luid, id);
   chxformat (authorid, CQ("^%s%s"), L(0), L(0), chn, ss_ruid);
   chxalter  (authorname, L(0), authorid, CQ(""));
   chxclear  (authorid);
   chxformat (authorid, CQ("%s%s"), L(0), L(0), ss_luid, id);
   chxformat (authorid, CQ("^%s"), L(0), L(0), id, chn);
   chxalter  (authorname, L(0), authorid, CQ(""));
   chxclear  (authorid);
   chxformat (authorid, CQ("%s%s"), L(0), L(0), ss_luid, id);
   chxformat (authorid, CQ("%s"),   L(0), L(0), ss_ruid, null.chx);
   chxalter  (authorname, L(0), authorid, CQ(""));
   chxtrim   (authorname);

   chxfree ( authorid );
   chxfree ( id );
   chxfree ( chn );
 
   RETURN (1);
}
