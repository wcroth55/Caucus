
/*** FUNC_PEO.   Return the resulting value of a peo_xxx() function.
/
/    func_peo (result, what, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Append value of peo_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/       $peo_members(num)     list of userids of members of conf CNUM
/       $peo_name(num, names...)  list of people
/       $peo_access (cnum priv)   list of userids with access to CNUM of PRIV
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funcpeo.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/30/95 13:12 New function. */
/*: CR  5/19/03 Return empty string on cnum <= 0. */
/*: CR  8/01/04 Add $peo_access(). */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern AsciiBuf  sqlbuf;

FUNCTION  func_peo (Chix result, char *what, Chix arg)
{
   static People  plist = (People) NULL;
   Chix           word;
   Person         person;
   Namelist       nl;
   int4           which, pos, cnum, priv;
   char           query[1000], w1[200], w2[200], temp[300];
   ENTRY ("func_peo", "");

   /*** Initialization. */
   if (plist == (People) NULL)   plist = a_mak_people ("func_peo plist");

   /*** $PEO_MEMBERS(cnum) */
   if      (streq (what, "peo_members")) {
      if (NOT chxnum (arg, &which))           { BAD("NO-CNUM");    RETURN(1); }
      if (which <= 0)                                              RETURN(1);

      sprintf (query, 
         "SELECT DISTINCT g.userid, u.lname, u.fname FROM groups g, user_info u "
         " WHERE g.userid = u.userid "
         "   AND g.owner  = 'CONF'  AND  g.name='%d'  AND g.access >= %d "
         "   AND g.active > 0       AND  u.active >= 10 "
         " ORDER BY u.lname, u.fname", which, privValue("minimum"));

      append_query_results (query, result, sqlbuf);
   }

   /*** $PEO_NAMES(cnum namepart1 [namepart2])    ( 2 nameparts max!) */
   else if (streq (what, "peo_names")) {
      word = chxalloc ( L(100), THIN, "func_peo word");
      pos  = L(0);
      cnum = chxint4 (arg, &pos);
      chxtoken (word, nullchix, 2, arg);    ascofchx (w1, word, 0, 200);
      chxtoken (word, nullchix, 3, arg);    ascofchx (w2, word, 0, 200);

      if (NOT NULLSTR(w1)) {
         strcpy (query, "SELECT DISTINCT u.userid FROM user_info u ");
         if (cnum > 0)  strcat (query, ", groups g ");
         strcat (query, "WHERE ");
         if (cnum > 0) {
            sprintf (temp, 
               "     g.userid = u.userid  AND g.owner='CONF' AND g.name='%d' "
               " AND g.access > 0  AND g.active > 0 AND ", cnum);
            strcat  (query, temp);
         }
         strcat (query, "u.active >= 10 ");
            
         sprintf (temp, "AND (u.lname LIKE '%s%%' OR u.fname LIKE '%s%%') ", 
                        w1, w1);
         strcat  (query, temp);
                        
         if (NOT NULLSTR(w2)) {
            sprintf (temp, "AND (u.lname LIKE '%s%%' OR u.fname LIKE '%s%%') ", 
                           w2, w2);
            strcat  (query, temp);
         }
         append_query_results (query, result, sqlbuf);
      }
      chxfree (word);
   }

   RETURN(1);
}
