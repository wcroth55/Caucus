
/*** FUNC_UNHTML.    Filter out HTML tags.
/
/    func_unhtml (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Implements CML function $unhtml().  Filters out HTML tags, 
/       replacing certain tags with newlines, the rest with blanks.
/
/       Particularly useful in combination with $find_filter()
/       to make search "hit" highlighting useful in HTML or CML text.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/funcunhtml.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 07/05/06 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define TEXT  0    /* plain text */
#define TAG_A 1    /* Found <, assembling tag name */
#define TAG_B 2    /* In rest of tag */
#define Q2    3    /* Inside double-quoted string in tag */
#define Q1    4    /* Inside single-quoted string in tag */

FUNCTION  func_unhtml (Chix result, Chix arg)
{
   static Chix blank = nullchix;
   static Chix comma, newline;
   Chix        tagname, replace, nltags;
   int         c, type, state, pos;
   ENTRY ("func_unhtml", "");

   if (blank == nullchix) {
      blank   = chxalsub (CQ(" "),   L(0), L(10));
      comma   = chxalsub (CQ(","),   L(0), L(10));
      newline = chxalsub (CQ("\n"),  L(0), L(10));
   }

   type    = chxtype  (arg);
   nltags  = chxalloc (L(80),   type, "func_unhtml nltags");
   tagname = chxalloc (L(20),   type, "func_unhtml tagname");
   replace = chxalloc (L(10),   type, "func_unhtml replace");

   pos = chxtoken (nltags, nullchix, 2, arg);
   if (pos < 0)  pos = chxlen (arg);
   chxtoken   (nltags, nullchix, 1, arg);
   chxreplace (nltags, 0, comma, blank);
   jixreduce  (nltags);

   state = TEXT;
   for (;   (c = chxvalue (arg, pos));   ++pos) {
      if (state == TEXT) {
         if (c == '<') { state = TAG_A;    chxclear (tagname); }
         else            chxcatval (result, type, c);
      }

      else if (state == TAG_A) {
         if      (isalpha(c))  chxcatval (tagname, type, c);
         else if (c == '\'')   state = Q1;
         else if (c == '"')    state = Q2;
         else {
            state = (c == '>' ? TEXT : TAG_B);
            chxcpy (replace, blank);
            jixreduce (tagname);
            if (tablefind (nltags, tagname) >= 0) chxcpy (replace, newline);
            chxcat (result, replace);
         }
      }

      else if (state == Q1) {
         if      (c == '\\')  ++pos;
         else if (c == '\'')  state = TAG_B;
      }

      else if (state == Q2) {
         if      (c == '\\')  ++pos;
         else if (c == '"')   state = TAG_B;
      }

      else if (state == TAG_B) {
         if      (c == '\'')  state = Q1;
         else if (c == '"')   state = Q2;
         else if (c == '>')   state = TEXT;
      }
   }

   chxfree (nltags);
   chxfree (tagname);
   chxfree (replace);

   RETURN (1);
}
