
/*** APPLY_REDIRECT.   Apply redirection information.
/
/    ok = apply_redirect (red);
/
/    Parameters:
/       int       ok;        (success)
/       Redirect *red;       (redirection object)
/
/    Purpose:
/       Apply redirection information previously parsed from the command
/       line.  I.e., lock & open appropriate unit & file for input
/       or output.
/
/    How it works:
/       If RED specifies a file for output or append, lock and open
/       the file.
/
/       If RED specifies an input file, just lock it.
/
/       If apply_redirect() fails, it resets the red->unit number
/       to XWTX (output) or XKEY (input).
/
/    Returns: 1 on success
/             0 if cannot lock or open output file.
/
/    Error Conditions:
/ 
/    Side effects: may write to user's terminal on error.
/
/    Related functions:
/
/    Called by:  Caucus UI functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  gen/applyred.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  8/09/93 15:43 New function, based on old parse_output(). */
/*: CP  9/03/93  0:27 Change comments, reset unit number on failure. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"
#include "redirect.h"

extern union  null_t         null;
extern Chix                  ss_dotnl;

FUNCTION  apply_redirect (red)
   Redirect *red;
{
   Chix  newstr, format;
   int   pass;

   ENTRY ("apply_redirect", "");

   /*** Handle output redirection. */
   if (red->inout >= 1  &&  red->unit != XWTX) {
      if (unit_lock (red->unit, WRITE, L(0), L(0), L(0), L(0), CQ(red->fname))) {
         if (red->inout==1 ? unit_make (red->unit) : unit_append (red->unit)) {
            red->locked = 1;
            RETURN (1);
         }
         unit_unlk (red->unit);
      }
      red->unit = XWTX;
      newstr = chxalloc (L(80), THIN, "apply_red newstr");
      format = chxalloc (L(80), THIN, "apply_red format");
      mdstr      (format, "gen_Fnooutput", null.md);
      chxformat  (newstr, format, L(0), L(0), CQ(red->fname), null.chx);
      unit_write (XWER, newstr);
      chxfree (newstr);
      chxfree (format);
      RETURN  (0);
   }

   /*** Handle input redirection. */
   if (red->inout == 0  &&  red->unit != XKEY) {
      if (pass = unit_lock (red->unit, READ, L(0), L(0), L(0), L(0), CQ(red->fname))) {
         if (pass = unit_view (red->unit))  unit_close (red->unit);
         else                               unit_unlk  (red->unit);
      }
      if (pass)   red->locked = 1;
      else {
         red->unit = XKEY;
         mdwrite    (XWER, "err_Tnofile", null.md);
         unit_write (XWER, CQ(red->fname));
         unit_write (XWER, ss_dotnl);
         RETURN (0);
      }
   }

   RETURN (1);
}
