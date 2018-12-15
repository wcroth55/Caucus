
/*** UNIT_TERM.  Convert STR to local terminal character set & display.
/
/    unit_term (str);
/
/    Parameters:
/       Chix  str;        (text to be written)
/
/    Purpose:
/       Converts characters in STR to local representation used by
/       user's terminal, and writes the results directly to the terminal.
/
/    How it works:
/       
/    Returns:
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  unit_write(), unit_newline() only!
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  unit/unitterm.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/14/92 15:29 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"
#include "diction.h"

FUNCTION  unit_term (str)
   Chix   str;
{
#define   BUFSIZE  L(512)
   char   buf[BUFSIZE];
   int    lang, inc;
   int4   pos;
   char  *bugtell();

   ENTRY ("unit_term", "");

   lang = unit_control (XITT, UNIT_GET);
   for (pos=0;   (1);   pos += inc) {
      switch (lang) {
         case (LANG_NORM):
         case (LANG_ISO):  inc = ascofchx  (buf, str, pos, BUFSIZE);  break;
         case (LANG_EUC):  inc = eucofjix  (buf, str, pos, BUFSIZE);  break;
         case (LANG_SJIS): inc = sjisofjix (buf, str, pos, BUFSIZE);  break;
         default:          inc = -1;                                  break;
      }
 
      sysputstr (buf);
      if (inc <= 0)  break;
   }

   RETURN (1);
}
