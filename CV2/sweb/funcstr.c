
/*** FUNC_STR.   Handle $str_xxx()  functions.
/
/    func_str (result, what, arg, conf);
/   
/    Parameters:
/       Chix       result;  (append resulting value here)
/       char      *what;    (function name)
/       Chix       arg;     (function argument)
/       SWconf     conf;    (server configuration info)
/
/    Purpose:
/       Handle string manipulation functions.
/
/       $str_index (what text)
/        Returns index into string TEXT of beginning of first occurrence
/        of (one-word!) string WHAT, or -1 if not found.
/
/       $str_revdex (what text)
/        Returns index into string TEXT of beginning of *last* occurrence
/        (one-word!) string WHAT, or -1 if not found.
/
/       $str_sub (pos len text)
/        Returns substring of TEXT, starting at position POS, for LEN
/        characters total.
/
/    How it works:
/
/    Returns: void
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/funcstr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/16/97 15:48 New function. */
/*: CR  7/06/04 Add str_replace() */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  func_str (Chix result, char *what, Chix arg, 
                    struct sweb_conf_t *conf)
{
   static Chix word1 = nullchix;
   static Chix text, word2;
   int4        pos, where, len, ptr;
   char        temp[100];
   ENTRY ("func_str", "");
   
   /*** Initialization. */
   if (word1 == nullchix) {
      word1 = chxalloc ( L(30), THIN, "func_str word1");
      word2 = chxalloc ( L(30), THIN, "func_str word2");
      text  = chxalloc (L(100), THIN, "func_str text");
   }
   
   /*** $str_index (what text) */
   if (streq (what, "str_index")) {
      chxtoken (word1, nullchix, 1, arg);
      ptr = chxtoken (nullchix, text, 2, arg);

      if (ptr > 0)  where = chxindex (text, L(0), word1);
      else          where = -1;

      sprintf (temp, "%d", where);
      chxcat  (result, CQ(temp));
   }
   
   /*** $str_revdex (what text) */
   else if (streq (what, "str_revdex")) {
      chxtoken (word1, nullchix, 1, arg);
      ptr = chxtoken (nullchix, text, 2, arg);

      if (ptr > 0)  where = chxrevdex (text, ALLCHARS, word1);
      else          where = -1;

      sprintf (temp, "%d", where);
      chxcat  (result, CQ(temp));
   }

   /*** $str_sub (pos len text) */
   else if (streq (what, "str_sub")) {
      ptr = L(0);
      pos = chxint4 (arg, &ptr);
      len = chxint4 (arg, &ptr);
      chxtoken  (nullchix, text, 3, arg);
      chxcatsub (result, text, pos, len);
   }

   else if (streq (what, "strlen")) {
      sprintf (temp, "%d", chxlen (arg));
      chxcat  (result, CQ(temp));
   }

   else if (streq (what, "str_replace")) {
      chxtoken (text,  nullchix, 1, arg);
      quoter   (word1, "unquote", text, conf);
      chxtoken (text,  nullchix, 2, arg);
      quoter   (word2, "unquote", text, conf);
      chxtoken (nullchix, text,  3, arg);
      chxreplace (text, 0, word1, word2);
      chxcat     (result, text);
   }

   RETURN (1);
}
