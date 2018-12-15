
/*** UNIT_READ.  Read a line from file N into STR.
/
/    ok = unit_read (n, str, option);
/
/    Parameters:
/       int   ok;      (success?)
/       int   n;       (unit number of open file)
/       Chix  str;     (read line of text into here)
/       int4  option;  (input option mask)
/
/    Purpose:
/       Read a line of text from the file open on unit N into STR.
/       Trailing newline, if any, is removed.
/
/    How it works:
/       Unit_read() calls unit_control() to find out what kind of
/       character set is used to read the input.
/
/       OPTION controls several input options, according to its value:
/          UREAD_MAX + N    read maximum of N characters
/          UREAD_PFK        turn on PF key mapping where appropriate
/
/       If UREAD_MAX is not specified, unit_read() chooses its own maximum
/       (see BUFMAX below).
/       
/    Returns: 1 on success
/             0 on end-of-file, interrupt, hangup, or error
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: lots
/
/    Known bugs:
/       We may wish to have unit_read() return some indication
/       of an error other than the return value 0.
/
/       UREAD_MAX option is ignored for japanese character input.
/       Someday we may wish to apply a maximum *width* limit, 
/       measured in number of single-width characters.
/
/    Home:  unit/unitread.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/27/92 14:26 Begin chixification. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"
#include "diction.h"

FUNCTION  unit_read  (n, str, option)
   int  n;
   Chix str;
   int4 option;
{
#define   BUFMAX   512
   char   buf[BUFMAX], outseq[BUFMAX];
   int    maxlen, lang, uselen, code, init, warn;

   ENTRY ("unitread", "");

   chxclear (str);
   init = 0;

   /*** Determine the max length we really want to use... */
   maxlen = (option & UREAD_MAX ? option & 0xFFFF : BUFMAX);
   uselen = (maxlen < BUFMAX ? maxlen : BUFMAX);

   /*** And the default language representation for this unit. */
   lang   = unit_control (n, UNIT_GET);

   /*** And read the line of bytes from 3270 terminal... */

   /*** ... or japanese terminal... */
   if (n==XKEY  &&  (lang==LANG_EUC || lang==LANG_SJIS)) {
      code = jixread (str, lang);
      RETURN (code > -2);
   }

   else do {

      /*** ... or vanilla ascii terminal... */
      if (n==XKEY) code = (sysrdline (buf, uselen) <= -2 ? 0 : 1);

      /*** ... or disk file. */
      else         code = sysrdfile (n, buf, uselen);

      if (code == 0)  RETURN(0);

      /*** Convert the bytes, as selected by the default unit i/o character
      /    representation, to chix. */
      switch (lang) {
         case (LANG_NORM):
         case (LANG_ISO):  warn = 1;  chxcat (str, CQ(buf));             break;
         case (LANG_EUC):  warn = jixofeuc   (str, buf, init, outseq);   break;
         case (LANG_SJIS): warn = jixofsjis  (str, buf, init, outseq);   break;
         default:          warn = -1;                                    break;
      }
      init = 1;
      if (warn < 0)  code = -1;
   } while (code == 2);

   RETURN (code >= 0);
}
