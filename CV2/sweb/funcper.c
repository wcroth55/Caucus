
/*** FUNC_PER.   Return the resulting value of a per_xxx() function.
/
/    func_per (result, what, arg, protect);
/   
/    Parameters:
/       Chix   result;  (append resulting value here)
/       char  *what;    (function name)
/       Chix   arg;     (function argument)
/       int    protect; (protected mode on?)
/
/    Purpose:
/       Append value of per_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funcper.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/09/95 15:18 New function. */
/*: CR 01/10/06 Don't set 'exists'; hacked transition 'til full move to MySQL */

#include <stdio.h>
#include "systype.h"
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#if UNIX | NUT40
#include <pwd.h>
#include <sys/types.h>
#endif

extern FILE   *sublog;
extern Person g_person;
extern Pstat  g_pstat2;

FUNCTION  func_per (Chix result, char *what, Chix arg, int protect)
{
   static Chix    empty = (Chix) NULL;
   static Chix    word, userid, me;
   int4           val;
   int            code, exists;
   char           temp[100];
   struct passwd *passwd;
   ENTRY ("func_per", "");

   /*** Initialization. */
   if (empty == (Chix)   NULL) {
      empty  = chxalloc ( L(4), THIN, "func_per empty");
      word   = chxalloc (L(40), THIN, "func_per word");
      me     = chxalloc (L(40), THIN, "func_per me");
      userid = chxalloc (L(40), THIN, "func_per userid");
   }

   /*** All per_xxx() functions take a userid as an argument. */
   chxtoken (userid, nullchix, 1, arg);
   if (chxvalue (userid, L(0)) == 0)  RETURN(1);

   /*** $PER_REAL     Unix "gecos field" name of userid. */
   if (strindex (what, "per_real") == 0) {
#if UNIX | NUT40
      ascofchx (temp, userid, L(0), L(100));
      passwd = getpwnam (temp);
      if (passwd != NULL)  chxcat (result, CQ(passwd->pw_gecos));
#endif
      RETURN(1);
   }

   /*** The rest of the functions load (or use the cached version of)
   /    the personal info for this userid. */
   exists = 1;
   if (NOT chxeq (userid, g_person->owner)) {
      chxcpy (g_person->owner, userid);
/*
      exists = a_get_person (g_person, P_EVERY, A_WAIT) == A_OK;
      if (exists) removeid (g_person->name, g_person->name, 
                            g_person->owner, empty);
      else chxclear (g_person->owner);
*/
      a_get_person (g_person, P_EVERY, A_WAIT);
   }

   if (streq (what, "per_delete")) {
      code = 1;
      if (NOT protect) {
         if (exists) {
            code = -1;
            if (mgr_in_group ("userDelete systemAllPrivileges"))
               code = (a_del_person (g_person, 0, A_WAIT) == A_OK ? 0 : 1);
         }
      }
      sprintf (temp, "%d", code);
      chxcat  (result, CQ(temp));
      RETURN(1);
   }

   if (NOT exists)   RETURN(1);

   /*** Functions... */
   if      (streq (what, "per_name"))   chxcat (result, g_person->name);

   else if (streq (what, "per_phone"))  chxcat (result, g_person->phone);

   else if (streq (what, "per_intro"))  chxcat (result, g_person->intro->data);

   else if (streq (what, "per_laston")) chxcat (result, g_person->laston);

   /*** $PER_LASTIN (id cnum) */
   else if (streq (what, "per_lastin")) {
      chxtoken (word, nullchix, 2, arg);
      chxnum   (word, &val);
      if (NOT chxeq (userid, g_pstat2->owner)  ||  g_pstat2->cnum != val) {
         chxcpy (g_pstat2->owner, userid);
         g_pstat2->cnum  = val;
         g_pstat2->items = 0;
         if (a_get_pstat (g_pstat2, P_EVERY, A_WAIT) != A_OK)  
            g_pstat2->items = -1;
      }

      chxcat (result, g_pstat2->lastin);
   }
 
   RETURN(1);
}
