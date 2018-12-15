
/*** VAR_SHOW.   Display value of a variable or object.
/
/    var_show (result, name, vars, otab, cfile);
/ 
/    Parameters:
/       Chix          result;  (append resulting value here)
/       Chix          arg;     (list of vars/objects)
/       Vartab        vars;    (CML variable list)
/       Obtab         otab;    (object table)
/       CML_File      cfile;   (original CML file)
/
/    Purpose:
/       "Display" names and values of named vars/objects to RESULT.
/
/    How it works:
/       If NAME is a variable, displays name and value.
/       If NAME is an object.member reference, displays name and value.
/       If NAME is an object, displays all members of that object.
/
/    Returns: 1
/
/    Known bugs:
/
/    Home:  sweb/varshow.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/29/98 17:04 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  var_show (Chix result, Chix name, Vartab vars, Obtab otab,
                    CML_File cfile)
{
   static Chix format = nullchix;
   static Chix ovalue;
   int    slot, mnum;
   char   oname[100], mname[100], temp[200];
   Object obj;

   ENTRY ("var_show", "");

   if (format == nullchix) {
      format = chxalsub (CQ("%s = '%s'\n\n"), L(0), ALLCHARS);
      ovalue = chxalloc (L(80), THIN, "var_show ovalue");
   }

   /*** Object syntax... */
   if (chxvalue (name, L(0)) == 0177) {
      ascofchx (oname, name, L(0), L(100));

      /*** Object.member.  Just display that value. */
      if (strindex (oname, ".") >= 0) {
         chxclear  (ovalue);
         if (ob_value (ovalue, name, otab, cfile))
            chxformat (result, format, L(0), L(0), name, ovalue);
      }

      /*** Entire object.  Display each member. */
      else if ( (slot = obtab_is (otab, oname)) >= 0) {
         obj = otab->obj[slot];
         for (mnum=0;   mnum < obj->members;   ++mnum) {
            strtoken  (mname, mnum+1, obj->memnames);
            sprintf   (temp, "%s.%s", oname, mname);
            chxformat (result, format, L(0), L(0), 
                       CQ(temp), otab->obj[slot]->value[mnum]);
         }
      }
   }

   /*** Normal variable. */
   else if ( (slot = vartab_is (vars, name)) >= 0) {
      chxformat (result, format, L(0), L(0), name, vars->value[slot]);
   }


   RETURN (1);
}
