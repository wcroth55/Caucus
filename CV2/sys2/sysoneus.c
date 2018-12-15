
/*** SYSONEUSER.  Allow only one user per userid on Caucus at a time.
/
/    SYSONEUSER is called with the argument "on" when a user first gets
/    on a conference.  It attempts to lock the XUOU file.  If successful,
/    it returns 1.
/
/    If XUOU is not locked, SYSONEUSER returns immediately.
/    If XUOU is already locked, the action taken by SYSONEUSER depends
/    on the setting of the ONEUSER option.
/    If ONEUSER is ON, Caucus exits immediately.
/    If ONEUSER is OFF, the user is informed that someone else
/    is using that id.  SYSONEUSER prompts to see if the
/    user *really* wants to join anyway.  If the user responds YES,
/    SYSONEUSER returns 1, otherwise it returns 0.
/
/    SYSONEUSER is called with the argument "off" when a user is exiting
/    CAUCUS; this unlocks XUOU. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern Userreg    thisuser;
extern struct flag_template flag;
extern union  null_t     null;
extern Chix              confid;
extern Chix              yesnotable;

FUNCTION  sysoneuser (on_or_off)
   char   on_or_off[];
{
   int  success;
   Chix no, mtu, format;
   Chix  reply;

   ENTRY ("sysoneuser", "");

   no    = chxalloc (L(20), THIN, "sysoneu no");  
   mtu   = chxalloc (L(200), THIN, "sysoneu mtu");  
   format= chxalloc (L(80), THIN, "sysoneu format");  

/*-----------------------------Most UNIXes-------------------------------*/
#if UNIX | NUT40 | WNT40
   reply = chxalloc (L(20), THIN, "sysoneu reply");
   success=1;

   if      (streq (on_or_off, "off"))      unit_unlk (XUOU);

   else if (unit_lock (XUOU, TLOCK, L(0), L(0), L(0), L(0), thisuser->id)) ;

   else {
      if (flag.oneuser) {
         /* Tell person why we're quitting.  */
         mdwrite (XWER, "sys_Etwoyou", null.md);
         sysexit (1);
      }
      else {
         chxtoken (no, null.chx, 1, yesnotable);
         mdstr    (format, "sys_Ttwoyou", null.md);
         chxformat  (mtu, CQ("\n%s %s"), L(0), L(0), thisuser->name, format);
         success = prompter (mtu, yesnotable, 2, "sys_Htwoyou", 10, reply, no)>0;
      }
   }
   chxfree (reply);
#endif

   chxfree (no);
   chxfree (mtu);
   chxfree (format);

   RETURN (success);
}
