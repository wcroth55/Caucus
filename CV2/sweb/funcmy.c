
/*** FUNC_MY.   Handle my_xxx() and set_my_xxx() functions.
/
/    func_my (result, what, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Append value of my_xxx() and set_my_xxx() functions WHAT 
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
/    Home:  funcmy.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/05/95 18:36 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern FILE *sublog;
extern Person g_person;
extern int    g_wrap;
extern int    g_mytext;

FUNCTION  func_my (Chix result, char *what, Chix arg)
{
   static Person  my     = (Person) NULL;
   static int     exists = 0;
   static Chix    word;
   char           temp[200];
   int            i, code;
   int4           pos;
   ENTRY ("func_my", "");

   /*** Initialization. */
   if (my == (Person) NULL)  {
      my = a_mak_person ("func_my my");
      sysuserid (my->owner);
      word = chxalloc (L(40), THIN, "func_my word");
   }
   if (NOT exists) {
      if (a_get_person (my, P_EVERY, A_WAIT) == A_OK)  exists = 1;
   }


   /*** Does user exist? */
   if (streq (what, "my_exist")) {
      chxcatval (result, THIN, L(exists ? '1' : '0'));
      RETURN(1);
   }

   /*** Is user trying to be created? */
   if (streq (what, "set_my_name")) {
      if (chxvalue (arg, L(0)) != 0) {
         chxclear  (g_person->owner);        /* Clear Person cache. */
         chxcpy    (my->name, arg);
         clean_newlines (my->name);
         for (i=1;   chxtoken (word, nullchix, i, arg) != -1;   ++i) 
            chxcpy (my->lastname, word); 
         jixreduce (my->lastname);
         code = a_chg_person (my, P_NAME | P_LASTNM, A_WAIT);
         if (code == A_NOPERSON) {
            code = a_add_person (my, P_NAME | P_LASTNM, A_WAIT);
         }
      }
      chxcatval (result, THIN, (code == A_TOOMANY ? L('0') : L('1')) );
      RETURN(1);
   }


   /*** All other possibilities fail if user does not exist. */
   if (NOT exists) { BAD("NO PERSON");    RETURN(1); }

   /*** Functions... */
   if      (streq (what, "my_name"))   chxcat (result, my->name);

   else if (streq (what, "my_phone"))  chxcat (result, my->phone);

   else if (streq (what, "my_intro"))  chxcat (result, my->intro->data);

   else if (streq (what, "my_laston")) chxcat (result, my->laston);

   else if (streq (what, "set_my_phone")) {
      chxclear (g_person->owner);        /* Clear Person cache. */
      chxcpy   (my->phone, arg);
      clean_newlines (my->phone);
      a_chg_person (my, P_PHONE, A_WAIT);
   }

   else if (streq (what, "set_my_intro")) {
      chxclear (g_person->owner);        /* Clear Person cache. */
      chxclear (my->intro->data);
      col_wrap (my->intro->data, arg, L(0), g_wrap);
      a_chg_person (my, P_TEXT, A_WAIT);
   }

   RETURN(1);
}
