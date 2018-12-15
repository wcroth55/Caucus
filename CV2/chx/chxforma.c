
/*** CHXFORMAT.   Sprintf-like formatting of chix strings.
/
/    chxformat (target, format, l1, l2, c1, c2);
/
/    Parameters:
/       Chix  target;     (append resulting string here)
/       Chix  format;     (format code string)
/       int4  l1;         (1st integer arg)
/       int4  l2;         (2nd integer arg)
/       Chix  c1;         (1st string arg)
/       Chix  c2;         (2nd string arg)
/
/    Purpose:
/       Provide a sprintf-like means of formatting chix strings.
/   
/    How it works:
/       Chxformat() scans the codes in FORMAT, and using the values
/       in L1, L2, C1, and C2, prepares a resulting chix string.  The
/       result is *appended* to TARGET.
/
/       Plain text in FORMAT is copied as is.  The special format codes
/       are:
/          %d         insert (next) integer arg in decimal.
/          %x         insert (next) integer arg in hexadecimal.
/          %o         insert (next) integer arg in octal.
/          %s         insert (next) string arg.
/          %%         insert a single "%".
/
/       Either the %d or %s codes may have numeric width codes, as in
/       "%5d", "%03o", or "%-6x".  The first %d/%x/%o encountered uses L1,
/       the second uses L2, and similarly with %s and C1/C2.
/
/    Returns: 1 normally, 0 on error.
/
/    Error Conditions:
/       TARGET or FORMAT not a chix.
/       If C1 or C2 is used, and is not a chix, they are treated as if
/       an empty chix was supplied.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  chx/chxforma.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/07/91 19:46 New function. */
/*: CX 10/17/91 13:42 Add %x, %o handling. */
/*: JX  6/15/92 17:44 Remove call to chxquick(slot 0). */
/*: CX  6/18/92 16:13 %s +/- justification backwards! */
/*: JX  6/30/92 11:27 Adjust string formatting with jixscrsize. */
/*: JV 10/16/92 09:23 Make tempchx static, don't free. */
/*: CR 11/20/92 19:49 Use %ld, %lx, %lo, as l1, l2 are longs! */
/*: CL 12/11/92 11:13 Long Chxcatsub args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CK  3/18/93 15:57 Allow "%ld", "%lx", "%lo". */
/*: OQ 02/09/98 replace chxcheck call by inline commands */
#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxformat (target, format, l1, l2, c1, c2)
   Chix   target, format;
   int4   l1, l2;
   Chix   c1, c2;
{
   static Chix tempchx=nullchix;
   int    ftype, i0, ldex, cdex, blanks, j,
          fieldwidth, maxchar, maxscr;
   int4   v, len, lval[2];
   int4   i;
   Chix   cval[2];
   char   ascii_format[40], ascii_result[80], zero[10];

   ENTRY ("chxformat", "");

   /*** Input validation. */
   /*if (NOT chxcheck (target, "chxformat target"))  RETURN (0);
     if (NOT chxcheck (format, "chxformat format"))  RETURN (0);*/
   if (target == nullchix )
      { chxerror (CXE_NULL, "chxformat target" , "");  RETURN(0); }
   if (format == nullchix )
      { chxerror (CXE_NULL, "chxformat format" , "");  RETURN(0); }
   if (FREED(target))    
      { chxerror (CXE_FREED, "chxformat target" , target->name); RETURN(0); }
   if (FREED(format))    
      { chxerror (CXE_FREED,  "chxformat format" , format->name);RETURN(0); }
   if ( NOT ISCHIX(target) )
      { chxerror (CXE_NOTCHX, "chxformat target" , "");          RETURN(0); } 
   if ( NOT ISCHIX(format) )
      { chxerror (CXE_NOTCHX, "chxformat format" , "");          RETURN(0); } 

   /*** Initialization... */
   ftype = format->type;
   ldex  = 0;
   lval[0] = l1;
   lval[1] = l2;
   cdex    = 0;
   cval[0] = cval[1] = chxquick ("", 9);

   /*if (c1 != nullchix  &&  chxcheck (c1, "chxformat c1"))  cval[0] = c1;*/
   if ( c1 != nullchix) {
      if ( (NOT FREED(c1)) && ISCHIX(c1) ) cval[0] = c1;
      else {
         chxerror (CXE_FREED, "chxformat c1" , c1->name);
         chxerror (CXE_NOTCHX, "chxformat c1" , "");
      }
   }

   /*if (c2 != nullchix  &&  chxcheck (c2, "chxformat c2"))  cval[1] = c2;*/
   if ( c2 != nullchix) {
     if ((NOT FREED(c2)) && (ISCHIX(c2)) ) cval[1] = c2;
     else {
        chxerror (CXE_FREED, "chxformat c2" , c2->name);
        chxerror (CXE_NOTCHX, "chxformat c2" , "");
     }
   }

   if (tempchx == nullchix) 
      tempchx = chxalloc (40L, THIN, "chxformat tempchx");

   /*** Scan each character in FORMAT.  If it's not a % format code,
   /    just add it to the end of TARGET.  If it *is* a format code,
   /    process it. */
   for (i=0;   v = chxvalue (format, i);   ++i) {
      if (v != '%') { chxcatval (target, ftype, v);   continue; }

      /*** Treat "%%" as a single "%" of text. */
      v = chxvalue (format, ++i);
      if (v == '%') { chxcatval (target, ftype, v);   continue; }

      /*** Pick up "%Nd" or "%Ns", put N in LEN. */
      zero[0] = (v == '0' ? '0' : '\0');
      zero[1] = '\0';
      i0   = i;
      len  = chxint4  (format, &i);
      v    = chxvalue (format,  i);
      if (v=='l')  v = chxvalue (format, ++i);  /* Allow %ld, etc. */

      /*** Handle "%d", "%-5d", "%5d", "%05d", "%o"..., "%x"... */
      if (v == 'd'  ||  v == 'x'  ||  v == 'o') {
         if (i0 == i)  sprintf (ascii_format, "%%%c", (int) v);
         else          sprintf (ascii_format, "%%%s%ldl%c", zero, (long) len, 
                                                            (int) v);
         sprintf (ascii_result, ascii_format, lval[ldex]);
         ldex = 1 - ldex;
         chxofascii (tempchx, ascii_result);
         chxcat  (target, tempchx);
      }

      /*** Handle "%s", "%-37s", "%37s". */
      else if (v == 's') {
         if (i0 == i)   chxcat (target, cval[cdex]);
         else {
            fieldwidth = (len > 0 ? len : -len);
            jixscrsize (cval[cdex], fieldwidth, &maxchar, &maxscr);
            blanks = fieldwidth - maxscr;

            if (len > 0)  for (j=0;   j<blanks;   ++j)
               chxcatval (target, ftype, (int4) ' ');
            chxcatsub (target, cval[cdex], 0L, L(maxchar));
            if (len < 0)  for (j=0;   j<blanks;   ++j)
               chxcatval (target, ftype, (int4) ' ');
         }
         cdex = 1 - cdex;
      }

      /*** Any other "%x" codes are treated as plain text. */
      else {
         chxcatval (target, ftype, (int4) '%');
         chxcatval (target, ftype, v);
      }
   }

   RETURN  (1);
}
