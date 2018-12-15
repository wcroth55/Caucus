/*** A_DEL_SUBJ.   Delete a subject from a conference.
/
/    ok = a_del_subj (subj, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Subj      subj;     (object containing subject to be deleted)
/       int4      mask;     (ignored)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Delete an existing subject category from a conference.
/
/    How it works:
/       Caller fills in CNUM, OWNER (empty for osubject, userid
/       for psubject), and PARTNAME (full name of subject).
/
/    Returns: A_OK       success
/             A_NOCONF   no such conference
/             A_NOWRITE  user not allowed to write in conf
/             A_NONE     no such subject.
/             A_NOTUNIQ  subject name not unique
/             A_NOTOWNER trying to access other user's psubjects.
/             A_DBERR    unknown error
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/adelsubj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/12/93 08:22 New function. */
/*: CR  5/14/04 Use co_priv_str() to check access levels. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "unitcode.h"
#include "xaction.h"
#include "derr.h"
#include "done.h"

FUNCTION  a_del_subj (Subj subj, int4 mask, int wait)
{
   int    error1, error2, is_psub, subject, success;
   int    access, additem, chgresp, chgsubj;
   int    usd, usf, utd, utf;
   Chix   cname, userid;

   if (disk_failure (0, &error1, NULL))                   return (A_DBERR);

   if (subj == (Subj) NULL  ||  subj->tag != T_SUBJ)      return (A_BADARG);
   if (subj->cnum <= 0)                                   return (A_NOCONF);

   is_psub = (NOT EMPTYCHX(subj->owner));
   cname   = chxalloc (L(30), THIN, "adelsubj cname");
   userid  = chxalloc (L(20), THIN, "aaddsubj userid");

   /*** This user can only work with their own psubjects. */
   sysuserid (userid);
   if (is_psub  &&  NOT chxeq (userid, subj->owner))      DONE (A_NOTOWNER);

   /*** Make sure this user can write in this conference. */
   if (NOT a_cache (V_GET, subj->cnum, &access, &additem, &chgresp, &chgsubj,
                    cname))                      DONE (A_NOCONF);
/* if (NOT is_psub  &&  access < 3)              DONE (A_NOWRITE); */
   if (NOT is_psub  &&  
           access < co_priv_str("instructor"))   DONE (A_NOWRITE);

   /*** Find the number of the subject that uniquely matches NAME. */
   if (is_psub) {
      usd = XUND;   usf = XUNN;
      utd = XUSD;   utf = XUSF;
   }
   else {
      usd = XCND;   usf = XCNN;
      utd = XCSD;   utf = XCSF;
   }
   subject = submatch (usd,usf, utd,utf, subj->owner, subj->cnum, 
                       cname, subj->namepart);
   if (subject  < 0) DONE (A_NOTUNIQ);
   if (subject == 0) DONE (A_NONE);

   if (NOT chg_dsubject (XT_DELETE, subj->cnum, cname, subj->owner,
                         subj->namepart, nullchix, &error1)) {
      if (error1==DNOSUBJ)   DONE (A_NONE);
      if (error1==D2SUBJ)    DONE (A_NOTUNIQ);
      DONE (A_DBERR);
   }

   subj->ready = 1;
   DONE (A_OK);

done:
   chxfree (cname);
   chxfree (userid);
   return  (success);
}
