/*** ALLOWED_ON.  Is a user ALLOWED_ON this conference?
/
/    ok = allowed_on (userid, groups, cnum);
/
/    Parameters:
/       int      ok;        (Success?)
/       Chix     userid;    (userid)
/       Namelist groups;    (obsolete/ignored argument)
/       int      cnum;      (conference number)
/
/    Purpose:
/       Determine if a userid is allowed to join
/       a conference, and if so, at what security level?
/
/    How it works:
/
/    Returns: access level
/             0 if no userid or no access
/            -1 if conference CNUM does not exist
/            -2 on any other error
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  XFU, Caucus
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  main/allowedo.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:47 Source code master starting point */
/*: CR  4/13/89 11:47 Take wildcard character from dictionary. */
/*: CW  5/18/89 14:53 Add XW unit codes. */
/*: CR  9/15/89 16:46 Add ":readonly" class of users. */
/*: CR  7/13/90 14:10 Add ":organizer" class. */
/*: CR  7/17/90 11:46 Add group matching, via groupmatch() call. */
/*: CR  9/12/90 13:39 Fix interaction between :readonly & :organizer. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  4/25/91  1:09 Trim trailing blanks from entries in XCUS! */
/*: CR  6/20/91 16:19 Use equal_id() to parse host information. */
/*: CR  7/24/91 21:45 Remove unused variables. */
/*: CR 10/21/91 18:15 Add 3rd arg, change 1st arg & return codes. */
/*: CR 10/30/91 17:32 Use ulist's. */
/*: JX  3/04/92 11:20 Chixify. */
/*: DE  3/26/92 16:37 Patched chxindex w/ 0, */
/*: DE  5/26/92 15:19 Fix chixification */
/*: JX  8/10/92 10:54 Add cache. */
/*: JX 10/01/92 13:45 Protect upper-case for group file names. */
/*: CR 12/02/92 14:34 Use ()'s for confnum=-1. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR 10/08/97 17:24 Use simplified Ulist. */
/*: CR 10/16/01 13:50 Add groupmatch (permnames, searchall). */
/*: CR 04/12/04 Use co_priv() to check privilege-names. */
/*: CR 09/02/05 Complete rewrite for C5. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

#define FLUSH -1

FUNCTION  allowed_on (userid, groups, cnum)
   Chix   userid;
   struct namlist_t  *groups;
   int                cnum;
{
   static Chix conf = nullchix;
   static Chix confnum;
   char        temp[40];

   if (conf == nullchix) {
      conf    = chxalsub (CQ("CONF"), L(0), L(4));
      confnum = chxalloc (L(10), THIN, "allowedo confnum");
   }

   if (cnum <= 0)  return (0);
      
   sprintf (temp, "%d", cnum);
   chxcpy  (confnum, CQ(temp));
   return  (group_access (userid, conf, confnum));
}
