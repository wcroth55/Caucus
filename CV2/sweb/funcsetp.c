
/*** FUNC_SETPER.   Handle set_per_xxx() functions.
/
/    func_setper (result, what, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Append value of set_per_xxx() functions WHAT 
/       with arguments ARG to RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/       Last name handling.
/
/    Home:  funcsetp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/28/98 22:34 New function. */
/*: CR  4/23/00 21:39 Allow user X to $set_per_xxx (X ...) */
/*: CR  3/21/03 If run from sweba, has full permissions. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  FAIL     { success = 0;  goto done; }
#define  SUCCEED  { success = 1;  goto done; }

extern FILE *sublog;
extern Person g_person;
extern int    g_wrap;
extern int    g_mytext;

FUNCTION  func_setper (Chix result, char *what, Chix arg)
{
   static Person  person = (Person) NULL;
   static Chix    ignore;
   static Chix    word;
   Pstat          pstat;
   char           temp[200];
   int            i, code, success;
   int4           cnum;
   ENTRY ("func_setp", "");

   /*** Initialization. */
   if (person == (Person) NULL)  {
      person = a_mak_person ("func_setper person");
      word   = chxalloc (L(40), THIN, "func_setper word");
      ignore = chxalloc (L(40), THIN, "func_setper ignore");
   }

   /*** All set_per_xxx() functions take a userid as an argument. */
   chxtoken (person->owner, nullchix, 1, arg);
   if (chxvalue (person->owner, L(0)) == 0)                  FAIL;
   clean_newlines (person->owner);

   /*** Must either *be* user == 1st argument, or manager with
   /    MGR_MKID priv bit, or run from sweba. */
   sysuserid (word);
   if (NOT  chxeq (word, person->owner)  &&
       NOT (mgr_in_group ("userCreate systemAllPrivileges"))    &&
       NOT is_sweba() )                                      FAIL;

   success = 0;

   /*** $set_per_join (userid cnum) */
   if (streq (what, "set_per_join")) {
      pstat = a_mak_pstat("funcsetp pstat");
      chxtoken (pstat->owner, nullchix, 1, arg);
      chxtoken (word,         nullchix, 2, arg);
      if (chxnum (word, &cnum)) {
         pstat->cnum = cnum;
         code = a_add_pstat (pstat, P_EVERY | P_OVERRIDE, A_WAIT);
         success = (code == A_OK);
      }
      a_fre_pstat (pstat);
   }

done:
   chxcatval (result, THIN, (success ? L('1') : L('0')) );
   RETURN(1);
}
