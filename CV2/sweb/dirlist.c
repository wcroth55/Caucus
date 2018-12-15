
/*** DIRLIST.   Directory listing CML function.
/
/    dirlist (result, arg);
/   
/    Parameters:
/       Chix       result;  (append resulting value here)
/       Chix       arg;     (function argument)
/
/    Purpose:
/       Evaluate $dirlist(format directory).
/
/    How it works:
/       $dirlist(0 directory) produces a string of all of the
/       files (and directories) in DIRECTORY.
/
/       $dirlist(1 directory) produces a string of pairs of words,
/       representing each file (and directory) in DIRECTORY.  The
/       first word is the name, the second is the size in bytes.
/
/       $dirlist(2 directory) produces a string of triplets of words,
/       representing each file (and directory) in DIRECTORY.  The
/       first word is the name, the second is the size in bytes, and
/       the third is 1 (if directory) or 0 (if file).
/
/    Returns: 
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  dirlist.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/25/97 16:45 New function. */
/*: CR  6/28/99 20:29 Add $dirlist(2...  arg */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "namelist.h"
#include "null.h"

extern union null_t null;

FUNCTION  int dirlist (Chix result, Chix arg)
{
   int4     pos, fsize;
   int      format, start, error, rc;
   Chix     word;
   char     dname[256], fname[256], path[256], temp[100];
   ENTRY ("dirlist", "");

   word = chxalloc (L(80), THIN, "dirlist word");
   pos  = 0;
   format = chxint4 (arg, &pos);
   chxtoken (word, nullchix, 2, arg);
   ascofchx (dname, word, L(0), L(256));

   for (start=1;   sysgfdir (dname, fname, start, &error) == 1;  start = 0) {
      chxcat    (result, CQ(fname));
      chxcatval (result, THIN, L(' '));
      if (format > 0) {
         sysbpath (path, dname, fname);
         rc = sysfsize (path, &fsize, &error);
         if (NOT rc)  fsize = -1;
         sprintf (temp, "%d ", fsize);
         chxcat  (result, CQ(temp));

         if (format > 1) {
            sprintf (temp, "%d ", rc-1);
            chxcat  (result, CQ(temp));
         }
      }
   }

   chxfree (word);
   RETURN  (1);
}
