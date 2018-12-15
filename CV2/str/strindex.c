/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** STRINDEX.  Return starting position in string S of string T. */

/*: AA  7/29/88 17:53 Source code master starting point */
/*: AM  2/07/89 10:33 Jcaucus changes*/
#include "caucus.h"

FUNCTION  strindex (s, t)
   char  s[], t[];
{
   register int  i, j, k;
#if JAPAN
   char  stype, ttype, ssavetype;

   ENTRY ("strindex.c", "");
   cf_single('\0', &stype);
   for (i=0;  s[i] != '\0';  i++) {
	  cf_single('\0',&ttype);
	  ssavetype = stype;
      for (j=i, k=0;  t[k] != '\0';  j++, k++) {
         cf_single(s[j], &stype);
         cf_single(t[k], &ttype);
         if (stype != ttype || s[j] != t[k])
            break;
      }
      if (t[k] == '\0')
         RETURN (i);
	  stype = ssavetype;
	  cf_single(s[i],&stype);
   }
   RETURN (-1);
#else

   ENTRY ("strindex.c", "");
   for (i=0;  s[i] != '\0';  i++) {
      for (j=i, k=0;  t[k] != '\0' && s[j]==t[k];  j++, k++) ;
      if (t[k] == '\0')  RETURN (i);
   }
   RETURN (-1);
#endif
}
