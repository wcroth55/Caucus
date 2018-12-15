/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** NEXTUSER.  Find the "next" userid.
/
/    int n = nextuser (user)
/
/    Parameters:
/       Chix *user;
/
/    Purpose:
/       This function gets the "next" Caucus userid.
/       It assumes that the caller will chxclear USER before
/       the first call, and that the caller will not disturb the
/       contents between calls.  NEXTUSER returns 1 on success,
/       zero if there aren't any more users.
/
/       NEXTUSER copies the Caucus userid into USER.
/
/    Returns:
/       1 on success
/       0 if there were no more users
/      -1 on any error.  In this case nextuser() also writes a message
/         to the "Distributed Bug" file.
/
/    Error Conditions:
/      Can't lock or load XSND, or bad value in XSND
/      Can't lock or load XSNN
/
/    Side Effects: changes the contents of USER
/
/    Called by: xfp (main routine)
/
/    Operating System Dependencies: none
/
/    Home: xfp/nextuser.c
/
/    Warning: There is a rare condition which will make NEXTUSER() fail
/    to find a set of users.  But since this function is only for the
/    XFP, that set of users will be found next time around.
/    The circumstances: we load nmdir, but don't reload it every time
/    we find the "next" user.  But if the nmdir changes because Caucus
/    is splitting one of the XSNN files, under certain boundary conditions
/    we will skip one of the new "child" partfiles.
*/

/*: JV  3/02/91 18:21 Created function. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/02/91 17:18 Remove globals.h. */
/*: JV  7/04/91 20:06 Remove lint. */
/*: JV  7/22/91 16:11 Add ENTRY. */
/*: CR  7/24/91 22:07 Make Lint happier. */
/*: UL 12/11/91 18:45 Make unit_lock args 3-6 int4.*/
/*: DE  6/09/92 12:55 Started chixifying... */
/*: JX  9/01/92 16:12 Don't nlfree namdir: it's static! */
/*: JV  8/28/92 13:51 Remove errno (VV won't compile with it). */
/*: JV  8/24/93 10:59 Don't match duplicate adjacent lines. */
/*: CP  8/01/93 14:32 Add 'name' arg to make_textbuf() call. */

#include <stdio.h>
#include "handicap.h"
#include "goodlint.h"
#include "unitcode.h"
#include "chixuse.h"
#include "done.h"
#include "namelist.h"
#include "xfp.h"
#define  THISLINE ubuf->line[lnum]
#define  PREVLINE ubuf->line[lnum-1]

extern union  null_t        null;

FUNCTION nextuser (user)
   Chix  user;
{
   static struct namlist_t *namdir, *thisxuser;
   static int    lnum, first=1;
   struct namlist_t  *nlnode();
   static Textbuf ubuf;
   Textbuf make_textbuf();
   int    ok, success;
   Chix   name, id;

   ENTRY ("nextuser", "");

   name    = chxalloc (L(80), THIN, "nextuser name");
   id      = chxalloc (L(80), THIN, "nextuser id");
   success = 0;

   if (first) {
      ubuf = make_textbuf (TB_RESPS, TB_LINES, "nextuser ubuf");
      first=0;
   }

   /*** On the 1st call, load nmdir.  Any changes made to XSND after this
   /    won't be made to this copy of the data. */
   if (chxlen(user) == 0) {
      if (NOT unit_lock (XSND, READ, L(0), L(0), L(0), L(0), nullchix)) {
         bug ("NEXTUSER: can't lock XSND.", 0);
         DONE (-1); }
      namdir = nlnode (4);
      ok = loadnmdir (XSND, namdir);
      unit_unlk (XSND);
      if (NOT ok) { bug ("NEXTUSER: can't loadnmdir XSND.", 0); DONE (-1); }
      thisxuser = namdir;
      lnum = -1;
      ubuf->l_used = 0;
   }

   /*** For 1st time thru, now we have a valid list of partfiles
   /    that make up the namelist XSND.  It's already out of date,
   /    but we don't care.  So we load the first partfile.
   /    For subsequent calls, we need to check if we've already
   /    used up the present partfile, and load the next if necessary. */

   while (TRUE) {
      /* First pre-increment from any previous call. */
      for (++lnum; lnum < ubuf->l_used; ++lnum) {
         /*** Each time get the "next" namepart in the file, return the userid
         /    **if the namepart matches the userid**.  This provides us with
         /    a unique match for each user.
         /    Skip the "!" and "~~~~~~" and duplicate adjacent lines. */
         chxsimplify (THISLINE);
         if (lnum > 0  &&  chxeq (THISLINE, PREVLINE))     continue;
         if (chxtoken (id, nullchix, 2, THISLINE) == -1)   continue;
         chxtoken (name, nullchix, 1, THISLINE);
         if (chxeq (name, id)) {
             chxcpy (user, id);
             SUCCEED;            /* Success!! */
         }
      }         /* At this point we need to load the next pfile, if any. */
      thisxuser = thisxuser->next;                  /* (1st node is empty).*/
      if (thisxuser == null.nl || thisxuser->num == 0)
         FAIL;      /* No more pfiles: we're done.*/
      if (thisxuser->num < 0) { bug ("NEXTUSER: bad value in XSND.", 0);
         DONE (-1); }
      if (NOT unit_lock (XSNN, READ, L(0), L(thisxuser->num), L(0), L(0), nullchix))
         {bug ("NEXTUSER: can't lock XSNN.", 0); ; DONE (-1); }
      ok = loadnpart (XSNN, thisxuser->num, ubuf);
      unit_unlk (XSNN);
      if (NOT ok) {
         bug ("NEXTUSER: can't loadnpart XSNN.", 0);
         DONE (-1); }
      lnum = 0;
   }

   /*NOTREACHED*/
   bug ("NEXTUSER: got to end of function!", 0);
   DONE (-1);

 done:
   chxfree (name);
   chxfree (id);

   RETURN (success);
}
