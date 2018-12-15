
/*** READPROMPT.   Display prompt, and read a line from the user.
/
/    code = readprompt (kind, prompt, defans, input, map_pfkeys, eot);
/
/    Parameters:
/       int   code;        (returned status code)
/       int4  kind;        (read input line from this unit number)
/       Chix  prompt;      (prompt to display to user)
/       Chix  defans;      (default answer, if user just presses return)
/       Chix  input;       (place line read in here)
/       int   map_pfkeys;  (map VM/CMS pf keys to ascii strings?)
/       Chix  eot;         (end of text marker)
/
/    Purpose:
/       Readprompt() provides a simple prompt-and-read-a-line
/       capability.  It displays PROMPT on the user's terminal, 
/       aint4 with the default answer DEFANS in ()'s.
/
/       Readprompt() then reads a single line of INPUT from KIND.  (If
/       the user just pressed RETURN, DEFANS is placed in INPUT.)
/   
/    How it works:
/       For VM/CMS systems, the last line of PROMPT is shown in the
/       one-line prompt window.  The pfkey map is shown in the appropriate
/       window.  If MAP_PFKEYS is true, any unassigned pf keys are 
/       mapped into ascii strings, e.g. pressing PF6 produces ":pf06".
/       Otherwise, a pf key press is ignored. 
/
/       KIND must be one of IC_DATA, IC_COMM, IC_MENU, or IC_SESS.
/
/       If EOT is "$endfile$", and readprompt() encounters an end-of-file
/       on input, it places "$endfile$" in INPUT.
/
/    Returns: 1 on success
/             0 on (permanent) end-of-file, or serious error
/
/    Error Conditions:
/       Hangup, or end-of-file on IC_SESS using XISF.
/  
/    Side effects:
/
/    Related functions: input_control()
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  inout/readprom.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  6/05/92 13:52 New function. */
/*: JX  6/06/92 15:15 Add 3rd arg to unit_read. */
/*: JX  7/06/92 14:45 Only free EMPTY after DEFANS no longer needed. */
/*: CR 10/19/92 11:23 Change 'unit' to 'kind', use input_control(). */
/*: TP 10/20/92 15:52 EOF => failure only on session (XISF) file. */
/*: CR 12/10/92 15:11 chxalter arg int4. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: JV  9/07/93 15:22 Copy all prompt text to one line. */
/*: CK  9/21/93 23:04 Add 'eot' arg. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"

extern struct screen_template used;

FUNCTION  readprompt (kind, prompt, defans, input, map_pfkeys, eot)
   int4   kind;
   Chix   prompt, defans, input, eot;
   int    map_pfkeys;
{
   static Chix m_pfkey = nullchix;
   Chix   buffer, empty;
   int    newline, code, success, in_unit;
   int4   last;

   ENTRY ("readprompt", "");

   empty   = chxalloc (  L(4), THIN, "readprompt empty");
   buffer  = chxalloc (L(300), THIN, "readprompt buffer");

   /*** Allow the *(%^$! caller to use nullchix. */
   if (prompt == nullchix)  prompt = empty;
   if (defans == nullchix)  defans = empty;


   /*** Loop until we get data, or an EOF condition on XKEY or XISF. */
   while (1) {

      /*** Skip prompt for redirected data/command input. */
      in_unit = input_control (IC_ISUNIT | kind, nullchix);
      if (in_unit != XKEY  &&  in_unit != XISF)  ;
   
      /*** Special case for empty PROMPT. */
      else if (EMPTYCHX (prompt)) {
         if (systype() == SYSTYPE_VMCMS) {
            /*** Hack to handle menus: ensures that all of menu text appears on
            /    screen, and empties out prompt window. */
            unit_write (XWTX, CQ("\n"));
            mdstr      (buffer, "Menu_pfkeys", &m_pfkey);
            unit_write (XWPM, buffer);
         }
      }
   
      /*** Display PROMPT, but sneak "(default-answer)" in before the
      /    last newline. */
      else {
         /*** Display PROMPT, less the final newline, if any. */
         chxcpy (buffer, prompt);
         last    = chxlen (buffer) - 1;
         newline = (chxvalue (prompt, L(last)) == L('\n'));
         if (newline)
            chxalter (buffer, last, chxquick ("\n", 0), chxquick ("", 1));
   
         if (NOT EMPTYCHX(defans))
            chxformat (buffer, CQ(" (%s) "), L(0), L(0), defans, nullchix);
   
         unit_write (XWPM, buffer);
   
         if (newline) unit_write (XWPM, CQ("\n"));
      }
   
   
      /*** Get the INPUT line. */
      used.lines = used.width = 0;
      sysbrkclear();
      code = input_control (IC_GETEOF | kind, input);
   
      /*** Exactly what readprompt() should do on return is somewhat
      /    complicated.  Here are the possible cases:
      /  (a) Hangup.                           INPUT irrelevant.  Fail!
      /  (b) Interrupt.                        Clear INPUT.       Success.
      /  (c) input_control() succeeds.                            Success.
      /  (d) input_control() fails on XISF.    Clear INPUT.       Fail!
      /  (e) input_control() fails on XKEY.                       Success.
      /  (f) input_control() fails, all else.                     Loop again. */
      if (syshngtest())                        FAIL;
      if (sysbrktest())  { chxclear (input);   SUCCEED; }
      if (code) {
         if (EMPTYCHX(input) && NOT EMPTYCHX(defans))
            chxcpy (input, defans);
         SUCCEED;
      }
      chxclear (input);
      if (in_unit == XKEY)  {
         if (streq (ascquick(eot), "$endfile$"))  chxcpy (input, eot);
         SUCCEED;
      }
      if (in_unit == XISF)  FAIL;
   }
   
done:
   chxfree (buffer);
   chxfree (empty);

   RETURN  (success);
}
