
/*** FUNC_CL.   Return the resulting value of a cl_xxx() function.
/
/    func_cl (result, what, arg, conf);
/   
/    Parameters:
/       Chix          result;  (append resulting value here)
/       char         *what;    (function name)
/       Chix          arg;     (function argument)
/       SWconf        conf;    (server configuration info)
/
/    Purpose:
/       Append value of cl_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funccl.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/28/95 12:00 New function. */
/*: CR 11/23/05 Rewrite for C5. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern FILE *sublog;
extern AsciiBuf  sqlbuf;


FUNCTION  func_cl (Chix result, char *what, Chix arg, SWconf conf)
{
   static Chix cname    = nullchix;
   static Chix word;
   static int  first_cl = 1;
   char   temp[200], leading[100], confname[200];
   char  *s;
   int4   cnum, token, mask, which, pos;
   int    empty_arg, access;

   AsciiBuf ascii_buf();
   static AsciiBuf bigquery;

   ENTRY ("func_cl", "");

   if (cname == nullchix) {
      cname = chxalloc (L(40), THIN, "func_cl cname");
      word  = chxalloc (L(40), THIN, "func_cl word");
      bigquery = ascii_buf (NULL, 1000);
   }

   /*** $CL_LIST().   Query the join of 'confs' and 'groups' to get
   /    the list of accessible conferences. */
   if (streq (what, "cl_list")) {

      /*** Count the number of words, and build a temp buffer for the query
      /    that is guaranteed to be big enough. */
      for (which=pos=0;   chxnextword (word, arg, &pos) >= 0;   ++which) ;
      ascii_buf (bigquery, 1000 + 25 * which + chxlen(arg));

      /*** Build the start of the query.  If there are no args, it will
      /    suffice by itself to get *all* confs. */
      sysuserid (word);
      strcpy    (bigquery->str, "SELECT cnum FROM confs WHERE cnum > 0 AND deleted=0 ");

      /*** For each word in 'arg', add a "LIKE 'confname%'" clause. */
      strcpy (leading, "AND ( ");
      for (pos=0;   chxnextword (word, arg, &pos) >= 0;   ) {
         strcpy (temp, leading);
         strcat (temp, "name LIKE '");
         strcat (temp, ascquick(word));
         strcat (temp, "%'");
         strcat (bigquery->str, temp);

         strcpy (leading, " OR ");
      }
      if (pos > 0)  strcat (bigquery->str, ") ");

      strcat (bigquery->str, "ORDER BY name");

      append_query_results (bigquery->str, result, sqlbuf);
   }


   /*** $CL_NUM(name).   Return number of conf 'name'. */
   else if (streq (what, "cl_num")) {
      sysuserid (word);

      /*** Build cleaned up version of conf name. */
      if (chxtoken  (cname, nullchix, 1, arg) < 0)  RETURN(0);
      ascofchx  (confname, cname, 0, 200);
      for (s=confname;   *s;   ++s)  if (*s=='\'' || *s=='%')  *s = '_';

      /*** Look for exact match first. */
      sprintf (bigquery->str, 
            "SELECT cnum FROM confs WHERE name='%s' AND deleted=0", confname);
      sql_quick (0, bigquery->str, sqlbuf, 1);
      strtoken (temp, 1, sqlbuf->str);

      /*** If no match, try for abbreviation. */
      if (NULLSTR(temp)) {
         sprintf (bigquery->str, 
            "SELECT cnum FROM confs WHERE name LIKE '%s%%' AND deleted=0 ", confname);
         sql_quick (0, bigquery->str, sqlbuf, 1);
         strtoken (temp, 1, sqlbuf->str);
         if (NULLSTR(temp))  strcpy (temp, "0");
      }

      chxcat    (result, CQ(temp));
   }

   /*** $CL_NAME(). */
   else if (streq (what, "cl_name")) {
      cl_name (sqlbuf->str, sqlbuf->max, atoi(ascquick(arg)));
      chxcat  (result, CQ(sqlbuf->str));
   }

   RETURN(1);
}

FUNCTION cl_name (char *result, int resmax, int cnum) {
   char  temp[200];

   sprintf (temp, "SELECT name FROM confs WHERE cnum='%d'", cnum);
   sql_quick_limited (0, temp, result, resmax, 1);

   return (1);
}

