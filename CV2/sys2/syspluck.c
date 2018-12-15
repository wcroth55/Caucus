
/*** SYSPLUCK.   Pluck a file name out from a redirection string.
/
/    ok = syspluck (comline, pos, file);
/
/    Parameters:
/       int  ok;            (success?)
/       Chix comline;       (command line containing redirection string)
/       int  pos;           (starting position in COMLINE of redirect string)
/       Chix file;          (put file name here)
/
/    Purpose:
/       Pluck a "redirection string" out of a command line, and return
/       the file name.
/
/    How it works:
/       Syspluck is handed the command line, and the position (in the
/       command line) at which the redirection string begins.  It removes
/       the entire redirection string from the command line, and places
/       just the file name in FILE.
/
/       Redirection strings look like:
/         "<file", ">file", or ">>file"      (most systems)
/
/         "<fname ftype [fmod]"              (vm/cms)
/         ">fname ftype [fmod]"
/         ">>fname ftype [fmod]"
/
/    Returns: 1 on success
/             0 on error
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  sys/syspluck.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern Chix ss_inred, ss_outred, ss_appred;
extern union null_t null;

FUNCTION  syspluck (comline, pos, file)
   Chix   comline, file;
   int4   pos;
{
   Chix  rest;

/*-------"Normal" systems: filename is one word.-------------------- */
#if UNIX | NUT40 | WNT40
   ENTRY ("syspluck", "");

   rest = chxalloc (L(80), THIN, "syspluck rest");
   chxcatsub (rest, comline, pos, ALLCHARS);
   chxtoken  (file, null.chx, 1, rest);
   chxalter  (comline, pos, file, CQ(""));

   if      (chxindex (file, L(0), ss_appred) == 0)  
            chxalter (file, L(0), ss_appred, CQ("")) ;
   else if (chxindex (file, L(0), ss_outred) == 0)  
            chxalter (file, L(0), ss_outred, CQ("")) ;
   else if (chxindex (file, L(0), ss_inred)  == 0)  
            chxalter (file, L(0), ss_inred,  CQ(""))  ;

   chxfree (rest);
#endif

   RETURN (1);
}
