
/*** FUNC_LICE.  Return the value of a lice_xxx() function.
/
/    func_lice (result, what, arg, conf);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/       SWconf              conf;    (swebd configuration info)
/
/    Purpose:
/       Append value of lice_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/       lice_max_users()
/       lice_act_users()
/       lice_expires()
/       lice_customer()
/       lice_confs()
/
/    Returns: integer value of above functions
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funclice.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/18/97 17:48 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  int func_lice (Chix result, char *what, Chix arg, SWconf conf)
{
   int    total;
   int4   expires;
   Chix   dir;
   char   temp[100];
   char  *strplain();
   ENTRY ("func_lice", "");

   if (streq (what, "lice_act_users")) {
      dir = chxalloc (L(100), THIN, "func_lice dir");
      chxofascii (dir, conf->caucus_path);
      total = sys_tusers (dir);
      chxfree (dir);
      sprintf (temp, "%d", total);
      chxcat  (result, CQ(temp));
   }

   RETURN (total);
}
