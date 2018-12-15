/*** VALUE_SETOPTS.  Return the value of a particular SET option.
/
/    value_setopts (value, n, reg, table);
/
/    Parameters:
/       char   *value;            (place to put the value)
/       int     n;                (option number: 0 through 17)
/       struct  userreg_t *reg;   (user registration info struct)
/       char   *table;            (table of keywords for option N)
/
/    Purpose:
/       Prepare the displayable form of the value of a particular
/       SET option for a given user, and place it in VALUE.
/
/    How it works:
/       Value_setopts() looks in REG for the field that corresponds
/       to SET option number N.  It turns the value of that field into
/       a character string, and puts it in VALUE.
/
/       If the field is normally a number, it puts the "%d" form of that
/       number in VALUE.
/
/       If the field is an index to a restricted keyword set, it takes
/       the selected keyword from TABLE and puts it in VALUE.
/
/       If the field is just a string, it puts that string in VALUE.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  set()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  chg/valueset.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/07/92 17:10 New function. */
/*: DE  3/17/92 16:57 Chixified */
/*: JX  5/19/92 21:14 Fix Chix. */
/*: CP  8/17/93 16:16 Add 'terminal', 'showattach' members. */
/*: CP  8/24/93 13:56 SHOWAT now has multiple values. */
/*: JV  9/07/93 11:15 Add 'transfer'. */

#include <stdio.h>
#include "caucus.h"

extern union  null_t         null;

FUNCTION  value_setopts (value, n, reg, table)
   Chix    value;
   int     n;
   Userreg reg;
   Chix    table;
{
   int    is_num, is_dex, number;

   ENTRY ("value_setopts", "");

   is_num = is_dex = 0;
   switch (n) {
      case ( 0): is_num = 1;   number = reg->debug;         break;
      case ( 1): is_num = 1;   number = reg->lines;         break;
      case ( 2): is_num = 1;   number = reg->width;         break;
      case ( 3): is_dex = 1;   number = reg->truncate;      break;
      case ( 4): chxcpy (value, reg->editor);               break;
      case ( 5): is_num = 1;   number = reg->tablength;     break;
      case ( 6): is_num = 1;   number = reg->newpage;       break;
      case ( 7): is_dex = 1;   number = reg->printcode;     break;
      case ( 8): is_dex = 1;   number = 1-reg->verbose;     break;
      case ( 9): is_num = 1;   number = reg->dict;          break;

      case (10):
         if      (EMPTYCHX(reg->eot))                 { is_dex= 1; number = 1;}
         else if (chxeq ( reg->eot, CQ("$endfile$"))) { is_dex= 1; number = 0;}
         else                                       chxcpy (value, reg->eot);
         break;
       
      case (11): is_dex = 1;   number = 1-reg->expwarn;     break;
      case (12): is_dex = 1;   number = reg->texten;        break;
      case (13): is_num = 1;   number = reg->receiptsize;   break;
      case (14): is_dex = 1;   number = reg->mytext;        break;

      case (15):
         if (EMPTYCHX(reg->startmenu)) { is_dex = 1;   number = 1; }
         else                           chxcpy (value, reg->startmenu);
         break;

      case (16): is_dex = 1;   number = reg->import_mail;   break;
      case (17): is_dex = 1;   number = reg->export_mail;   break;
      case (18): chxcpy (value, reg->terminal);             break;
      case (19): is_dex = 1;   number = reg->showattach;    break;
      case (20): chxcpy (value, reg->transfer);             break;
   }

   if (is_num)  chxformat  (value, CQ("%d"), L(number), L(0), nullchix, null.chx);
   if (is_dex)  chxtoken   (value, nullchix, number+1, table);

   RETURN (1);
}
