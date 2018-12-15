
/*** READCML.   Read a logical CML line from a CML file.
/
/    success = readcml (fp, conf, line, newline, lnum);
/   
/    Parameters:
/       int       success;     (1 if ok, 0 if end-of-file)
/       FILE     *fp;          (pointer to open CML file)
/       SWconf    conf;        (server configuration info)
/       Chix      line;        (where to place result)
/       int       newline;     (retain newline at end of line?)
/       int      *lnum;        (original linenumber counter)
/
/    Purpose:
/       Read a CML line from an open CML file.  Perform low-level
/       processing.  (Ignore blank & comment lines, handle
/       line continuation.)
/
/    How it works:
/
/    Returns:  1 on success
/              0 on end-of-file
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  readcml.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/09/95 17:33 New function. */
/*: CR  3/25/95 12:58 Convert to chix. */
/*: CR  8/17/98 12:04 Complete conversion to chix. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

#define   MAXTEMP  300

FUNCTION  readcml (FILE *fp, SWconf conf, Chix line, int newline, int *lnum)
{
   static Chix work = nullchix;
   static Chix ctab, cspc;
   char   temp[MAXTEMP+2];
   int    success, is_cont, start, pos, last, backslash, first;
   int4   cval, len;
   ENTRY ("readcml", "");

   if (work == nullchix) {
       work  = chxalloc (L(300), chxtype(line), "readcml work");
       ctab  = chxalsub (CQ("\t"), L(0), L(4));
       cspc  = chxalsub (CQ( " "), L(0), L(4));
   }
   chxclear (line);

   success = 0;
   is_cont = 0;
   while (1) {
      chxclear (work);
      for (first=1;   (1);   first=0) {
         if (fgets (temp, MAXTEMP, fp) == NULL) {
            if (first)  RETURN (success);
            break;
         }
         to_jix (work, temp, 1-first, conf->diskform+1);
         if (temp [strlen(temp)-1] == '\n')  break;
      }
/*
      if (fgets (temp, MAXTEMP, fp) == NULL)  RETURN (success);
      chxclear (work);
      to_jix   (work, temp, 0, conf->diskform+1);
*/
      ++(*lnum);

      /*** Remove trailing newlines and returns. */
      chxchomp (work);

      /*** Replace tabs with single blanks. */
      chxreplace (work, L(0), ctab, cspc);
  
      /*** Skip blank or comment lines. */
      len = chxlen (work);
      if (len ==  0 )                       continue;
      if (NOT is_cont) {
         pos = non_blank (work);
         if (chxvalue (work, pos)  == '#')  continue;
      }

      /*** If we got this far, we got *some* kind of data OK! */
      success = 1;

      /*** Handle indication that this line is continued. */
      for (backslash=0, last=len-1;   last > 0;   --last) {
         cval = chxvalue (work, last);
         if (cval == '\\')  { backslash = 1;   break; }
         if (NOT chxblank (cval))              break;
      }

      if (backslash) {
         start = (is_cont ? non_blank (work) : 0);
         chxcatsub (line, work, start, (last-start));
         is_cont = 1;
         continue;
      }

      start = (is_cont ? non_blank(work) : 0);
      chxcatsub (line, work, start, ALLCHARS);
      if (newline) chxcatval (line, THIN, L('\n'));
      RETURN (1);
   }

   RETURN (1);   /* Should never get here! */
}

FUNCTION non_blank (Chix work)
{
   int4  pos, cval;

   for (pos=0;   (cval = chxvalue (work, pos));  ++pos)   
      if (NOT chxblank (cval))  return (pos);

   return (pos-1);
}
