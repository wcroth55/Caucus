
/*** PROMPTER.   Prompt the user and get a reply.
/
/    PROMPTER prints the supplied PROMPT string, and reads a line of text
/    into REPLY, to a maximum of MAXLEN characters (including the null).
/
/    PROMPTER then tries to find REPLY in the TABLE supplied
/    by the caller.  A null reply (just hitting <RETURN>) means use
/    the DEFAULTANSWER supplied by the caller.
/
/    The MODE argument controls what kind of REPLY will be accepted:
/       2 or 5    reply must be from the TABLE
/       1 or 4    from the table, or a number
/       0 or 3    anything is acceptable.
/
/    If MODE is 0, 1, or 2, PROMPTER operates normally.  If MODE is
/    3, 4, or 5, PROMPTER does NOT initially prompt for input; it
/    assumes that the input is already in the string REPLY.  If this
/    input does not fit the MODE conditions described above, *then*
/    PROMPTER will prompt for more input.
/
/       If the user desires help, PROMPTER will print message HELPMESSAGE,
/    and re-issue the prompt.
/
/       PROMPTER RETURNs one of the following values:
/    -4  if the user hung up (modem communications only)
/    -3  if the user hit the break/cancel key  (clears break flag)
/    -2  if the user merely hit <RETURN> with an empty DEFAULTANSWER.
/    -1  if the user's choice was not in the table
/     N  if the user's choice was entry N in the table
/
/     Note: the DEFAULTANSWER may be a nullchix; this has the same
/     effect as an empty string.
/  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: AM  2/07/89 10:27 Jcaucus changes*/
/*: CR  6/20/89 12:01 Allow prompter to parse "HELP blotto". */
/*: CW  6/20/89 12:01 Prompter to parse "HELP blotto"; defanswer "a/b". */
/*: CW  6/21/89 11:11 Use XW unit codes. */
/*: CR 10/09/89 14:18 Remove word2. */
/*: CR 10/18/89 17:21 HELP gets help only if no 2nd word following HELP. */
/*: CR  3/21/90 17:10 Empty table means map pf keys to ascii in sysprompt().*/
/*: CR  3/22/90 14:08 NULL table becomes "". */
/*: JX  5/16/92 15:28 Chixify. */
/*: CX  6/05/92 15:17 Replace sysprompt() with readprompt(). */
/*: CR 10/11/92 21:47 Add to comments. */
/*: TP 10/19/92 13:28 readprompt() reads from IC_DATA. */
/*: CR 12/02/92 14:36 Use ()'s in found=-1. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CK  9/21/93 23:05 Add 'eot' arg to readprompt() call. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"

extern struct screen_template used;
extern Userreg                thisuser;
extern union  null_t          null;

FUNCTION  prompter (prompt, table, mode, helpmessage, maxlen, 
                    reply, defaultanswer)
   Chix  prompt, table, reply, defaultanswer;
   char *helpmessage;
   int   mode, maxlen;
{
   static Chix s_help;
   int4  junk;
   int   found=(-1), number, wants_help, repeat, success;
   Chix  word, chunk, defaultword, word2, rest, input;
   short supplied;

   ENTRY ("prompter", "");

   rest = chxalloc (L(80), THIN, "prompter rest");
   word = chxalloc (L(30), THIN, "prompter word");
   chunk = chxalloc (L(80), THIN, "prompter chunk");
   input = chxalloc (L(80), THIN, "prompter input");
   word2 = chxalloc (L(30), THIN, "prompter word2");
   defaultword = chxalloc (L(60), THIN, "prompter defaultword");

   /*** If the defaultanswer has a "answer1/answer2" format, pluck off
   /    the very first part to be used as the default answer. */
   if (defaultanswer != nullchix)
      chxbreak (defaultanswer, defaultword, chunk, CQ("/"));

   /*** Is the input already supplied? */
   supplied = mode > 2;
   if (supplied)  mode = mode - 3;

   /*** Loop until the user has answered the PROMPT. */
   do {

      if (supplied) { chxcpy (input, reply);   supplied = 0; }
      else {
         /*** Flush any outstanding input. */
         sysflushin();

         /*** Display the prompt and get the INPUT. */
         sysbrkclear();
         chxclear (reply);
         if (NOT readprompt (IC_DATA, prompt, defaultword, input, 
                              NOT EMPTYCHX(table), thisuser->eot)) {
            success = -4;
            goto done;
         }

         if (sysbrktest()) {
            unit_write (XWTX, CQ("\n"));
            sysbrkclear();
            success = -3; goto done;
         }
      }
   
      /*** Pick off the first word of the user's reply, up to MAXLEN chars. */
      if (EMPTYCHX (input))  chxcpy (input, defaultword);
      chxclear (chunk);
      chxcatsub (chunk, input, L(0), L(20));
      chxtoken (word,  nullchix, 1,  chunk);
      chxtoken (word2, nullchix, 2,  chunk);
      jixreduce (word);
      chxclear (reply);
      chxcatsub (reply, word, L(0), L(maxlen));

      /*** Decide what to do with the REPLY.  REPEAT if HELP or not a
      /    valid choice. */
      if (EMPTYCHX(input))  { success = -2; goto done; }
      number = chxnum (word, &junk);
      found  = tablematch (table, word);

      mdtable ("gen_Aprmhelp", &s_help);
      wants_help = (tablematch (s_help, word) == 0)  &&  EMPTYCHX(word2);
      repeat     =  (mode==2 && found<0)
                 || (mode==1 && found<0 && NOT number)  ||  wants_help;

      if (repeat) {
         if      (wants_help)   help   (chunk, helpmessage);
         else if (found == -1) {
            chxupper (word);
            unit_write (XWER, word);
            mdwrite    (XWER, "gen_Fnotchoice", null.md);
         }
         else if (found == -2)  ambiguous (word);
      }
   } while (repeat);

   chxclear (reply);
   chxcatsub (reply, input, L(0), L(maxlen-1));
   chxcatsub (rest,  input, L(maxlen), L(4000));
   if (chxlen (input) >= maxlen) {
      mdwrite    (XWER, "gen_Eignored", null.md);
      unit_write (XWER, rest);
      unit_write (XWER, CQ("\n"));
   }
   success=1;

done:
   chxfree (rest);
   chxfree (word);
   chxfree (chunk);
   chxfree (input);
   chxfree (word2);
   chxfree (defaultword);

   if (success > 0) RETURN (found<0 ? -1 : found)
   else             RETURN (success);
}
