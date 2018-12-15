
/*** VARTAB_FIND.   Find a slot for a variable. 
/
/    slot = vartab_find (vt, name);
/   
/    Parameters:
/       int     slot;   (returned slot number in VARS)
/       Vartab  vt;     (variable table)
/       Chix    name;   (variable name)
/
/    Purpose:
/       Find a slot for variable NAME in the variable table VT.
/
/    How it works:
/       If variable NAME is in VT, return that slot number.
/       Otherwise, find the first empty slot number, and return that.
/
/    Returns: slot number on success
/             -1   if VT table is full (will automatically try and expand)
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  api/vartfind.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/09/96 15:52 New function (based on old sweb/varfind) */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "null.h"

FUNCTION  vartab_find (Vartab vt, Chix name)
{
   int    num, new;

   /*** Scan all of the allocated variables first... */
   for (new=(-1), num=0;   num<vt->used;   ++num) {

      /*** Found a matching name? */
      if (chxeq (vt->name[num], name))        return (num);

      /*** Found an empty (allocated) slot? */
      if (chxvalue (vt->name[num], L(0)) == 0)  new = num;
   }

   return (vartab_new (vt, name, new));
}


FUNCTION  vartab_new (Vartab vt, Chix name, int new)
{

   if (new < 0  ||  new >= vt->used)  new = vt->used++;

   /*** If variable table is nearly full, attempt to expand it. */
   if (new > vt->max - 5) {
      if (NOT vartab_exp (vt))  return (-1);
   }

   if (vt->name [new] == nullchix) {
       vt->name [new]  = chxalloc (L(30), THIN, "vart_new name");
       vt->value[new]  = chxalloc (L(40), THIN, "vart_new value");
   }

   chxcpy   (vt->name [new], name);
   chxclear (vt->value[new]);
   return (new);
}


FUNCTION  vartab_exp (Vartab vt)
{
   Chix  *exname, *exvalue;
   short *excode;
   int    num;

   /*** Allocate space for elements of doubled vartab. */
   excode  = (short *) sysmem (sizeof(short) * vt->max * 2, "excode");
   exname  = (Chix *)  sysmem (sizeof(Chix)  * vt->max * 2, "exname");
   exvalue = (Chix *)  sysmem (sizeof(Chix)  * vt->max * 2, "exvalue");
   if (exvalue == NULL)   return (0);

   /*** Copy old data into new areas.  (Improve by using memcpy?) */
   for (num=0;   num<vt->used;   ++num) {
      excode [num] = vt->code [num];
      exname [num] = vt->name [num];
      exvalue[num] = vt->value[num];
   }

   /*** Fill into "null" data for expanded part of new area. */
   vt->max = vt->max * 2;
   for (     ;   num<vt->max;   ++num) {
      exname[num] = exvalue[num] = nullchix;
      excode[num] = -1;
   }

   /*** Free old data areas. */
   sysfree ( (char *) vt->name);
   sysfree ( (char *) vt->value);
   sysfree ( (char *) vt->code);

   /*** 'Swap' new data areas into struct for vartab. */
   vt->name  = exname;
   vt->value = exvalue;
   vt->code  = excode;

   return (1);
}
