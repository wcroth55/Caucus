
/*** pullParens.   Remove next parenthesized expression from a Chix string.
/
/    found = pullParens (source, expression);
/ 
/    Parameters:
/       int       found;   (1=success, 0=none found)
/       Chix      source;  (source string, look for expressions here)
/       Chix      expression;  (place removed expression here)
/
/    Purpose:
/       Extract parenthesized expression(s) from a string.
/
/    How it works:
/       Finds first parenthesized expression in 'source', removes it,
/       and places it in 'expression'.   Expression must have an opening
/       and a closing parenthesis.  Parenthesis *inside* an expression
/       are treated as ordinary characters.
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sweb/pullparens.c
/ */

/*** Copyright (C) 2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/25/06 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  int pullParens (Chix source, Chix expr)
{
   static Chix  empty = nullchix;
   static Chix  lparen, rparen;
   int          p0, p1, p, parens, c;

   ENTRY ("pullParens", "");

   if (empty == nullchix)  {
      empty = chxalloc (40L, THIN, "func_sql empty");
      lparen = chxalsub (CQ("("),    L(0), L(4));
      rparen = chxalsub (CQ(")"),    L(0), L(4));
   }

   chxclear (expr);
   if ( (p0 = chxindex (source, 0,  lparen)) < 0)  RETURN (0);
   if ( (p1 = chxindex (source, p0, rparen)) < 0)  RETURN (0);

   for (parens=0, p=p0+1;   (c = chxvalue(source, p)) != 0;   ++p) {
      if (c == ')'  &&  parens==0) {
         chxcatsub (expr, source, p0, p-p0+1);
         chxalter  (source, p0, expr, empty);
         RETURN    (1);
      }

      if (c == '(' )  ++parens;
      if (c == ')' )  --parens;
   }

   RETURN (0);
}
