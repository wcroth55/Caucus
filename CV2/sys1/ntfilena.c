
/*** NT_FILENAME.  Convert Unix-style file name to an NT-style file name.
/
/    nt_filename (str);
/
/    Parameters:
/       char  *str;     (file name to be converted)
/
/    Purpose:
/       Many portions of Caucus produce Unix-style file names, with
/       forward slashes and mixed case letters.
/
/       Rather than rewrite every such portion, nt_filename() converts
/       a unix-style filename into NT style.
/
/    How it works:
/       All forward slashes are converted to backslashes.
/       All lower-case letters are converted to uppercase.
/
/    Returns:
/
/    Known bugs:      none
/
/    Home:  ynt/ntfilena.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/28/97 15:16 New function. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

FUNCTION  void nt_filename (char *str)
{
#if WNT40
   char  *s;

   for (s=str;   *s;   ++s) {
      if      (*s == '/')                 *s = '\\';
      else if (*s >= 'a'  &&  *s <= 'z')  *s = (*s - 'a') + 'A';
   }

#endif
   return;
}
