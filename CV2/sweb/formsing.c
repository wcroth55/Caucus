/*** FORM_SINGLE.    Process result of a single-part form.
/
/    form_single (work, form, conf);
/ 
/    Parameters:
/       Chix                work;   (working chix)
/       Vartab              form;   (variable list for forms data)
/       struct sweb_conf_t *conf;   (server configuration info)
/
/    Purpose:
/       Process the resultant data from a single-part form, stored
/       in WORK, and store the results in the forms variables in
/       FORM.
/
/    How it works:
/       Runs a state-table parser over WORK, plucking out the name=value
/       pairs.
/
/    Returns:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  formsing.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/06/96 20:26 New function, derived from old readform.c */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "diction.h"
#include "api.h"

#define  ST_NAME       1
#define  ST_VALUE      2

FUNCTION  form_single (work, form, conf)
   Chix                work;
   Vartab              form;
   struct sweb_conf_t *conf;
{
   static Chix name = nullchix;
   int         state, type, lang, slot, init;
   char        temp[4];
   int4        cval, pos;
   ENTRY ("form_single", "");


   if (name == nullchix)  name = chxalloc ( L(40), THIN, "formsing name");

   /*** Using a state table, parse the WORKing chix into "name=value"
   /    pairs, and store the data in the FORM cml variable list. */
   state = ST_NAME;
   chxclear (name);
   type = chxtype (work);
   lang = conf->browserform+1;
   slot = -1;
   temp[1] = '\0';
   for (pos=0L;   (cval = chxvalue (work, pos));   ++pos) {

      /*** While accumulating characters in the NAME... */
      if      (state == ST_NAME) {
         if      (cval == '&')  chxclear (name);
         else if (cval == '=') {
            slot  = vartab_find (form, name);
            if (slot < 0)  RETURN(1);
            if (chxvalue  (form->value[slot], L(0)) != 0)
                chxcatval (form->value[slot], THIN, L(' '));
            init  = 0;
            state = ST_VALUE;
         }
         else  chxcatval (name, type, cval);
      }

      /*** While accumulating characters in the VALUE... */
      else if (state == ST_VALUE) {
         if (cval == '&')  {
            state = ST_NAME;
/*          if (slot != -1)  
               report_value (form->value[name], form->value[slot]);  */
            chxclear (name);
            continue;
         }

         /*** Translate +'s and %xx sequences. */
         if (cval == '+')  cval  = ' ';
         if (cval == '%') {
            cval =             unhex (chxvalue (work, ++pos));
            cval = 16 * cval + unhex (chxvalue (work, ++pos));
         }

         if (cval != 0x0D) {
            temp[0] = cval;
            to_jix (form->value[slot], temp, init, lang);
            init = 1;
         }
      }
   }

   RETURN(1);
}

FUNCTION  unhex (x)
   int4   x;
{
   if (x >= '0'  &&  x <= '9')  return ( (int) (x - '0')      );
   if (x >= 'a'  &&  x <= 'f')  return ( (int) (x - 'a' + 10) );
   if (x >= 'A'  &&  x <= 'F')  return ( (int) (x - 'A' + 10) );

   return (0);
}

FUNCTION report_value (Chix n, Chix x)
{
   char temp[2048], part[128];
   int4  i, cval;

   sprintf (temp, "form(%s)[%d]=", ascquick(n), chxtype (x));
   strcat  (temp, ascquick(x));
   strcat  (temp, " = ");
   for (i=0;   cval = chxvalue (x, i);  ++i) {
      sprintf (part, " %04x", cval);
      strcat  (temp, part);
   }
   logger (1, LOG_FILE, temp);

   return;
}
