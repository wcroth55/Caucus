
/*** PLUCK_NAMRAN.  Pluck a named range of items from a command line.
/
/    status = pluck_namran (comline, type, words);
/
/    Parameters:
/       int    status;      (was a named range of items found?)
/       Chix   comline;     (command line to be examined)
/       Chix   type;        (put type of range: TITLE, AUTHOR, etc.)
/       Chix   words;       (put names found in range)
/
/    Purpose:
/       Pluck_namran() examines a command line COMLINE for a named range
/       of items, and if found removes it from COMLINE, puts the type
/       of range in TYPE, and the names in WORDS.
/
/    How it works:
/       A named range is one of the keywords TITLE, AUTHOR, SUBJECT,
/       or PSUBJECT, followed by a list of words.  These words are used
/       to select an item or range of items accordingly.  (I.e. the words
/       are either words in the title of an item, or the name of the author
/       of an item, or words in the name of a conference-wide subject
/       category, or words in the name of a personal subject category.)
/   
/       If the list of words is in quotes, just the words in quotes are
/       used for the name of the TITLE, AUTHOR, etc.  If no quotes are
/       present, PLUCK_NAMRAN uses all of the rest of the words on the
/       command line.
/   
/       If a named range is found, PLUCK_NAMRAN returns the keyword in
/       TYPE, and the list of words as one string in WORDS.
/
/    Returns: 0 if no named range was found
/             1 if a quoted string (but no name range keyword) was found
/             2 if a properly formed named range was found.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  gen/plucknam.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: JX  5/21/92 11:49 Chixify. */
/*: CX  5/27/92 11:08 Guru-ized. */
/*: CR 12/10/92 15:07 chxalter arg int4. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CR  1/25/93 12:57 Check chxtoken() return >= 0, not just > 0. */

#include <stdio.h>
#include "caucus.h"

extern Chix ss_qt1, ss_qt2;

FUNCTION pluck_namran (comline, type, words)
   Chix  comline, type, words;
{
   static Chix indextab;
   Chix   quote, words2;
   int4   p1, p2, pos;
   int    i, choice;
   int    success;

   ENTRY ("pluck_namran", "");

   quote   = chxalloc ( L(6), THIN, "pluck_namran quote");
   words2  = chxalloc (L(80), THIN, "pluck_namran words2");
   success = 0;

   /*** See if COMLINE contains either single or double quotes. */
   chxcpy  (quote, ss_qt1);
   mdtable ("gen_Aindex", &indextab);

   if ( (p1 = chxinside (comline, ss_qt1, words,  ss_qt1)) < 0)  p1 = 999;
   if ( (p2 = chxinside (comline, ss_qt2, words2, ss_qt2)) < 0)  p2 = 999;

   /*** If there's a quoted string of either sort (" or '), set P1 to
   /    point to it, and put the quoted part in WORDS. */
   if (p1 != p2) {
      if (p2 < p1)
         { chxcpy (words, words2);   chxcpy (quote, ss_qt2);   p1 = p2; }

      /*** Then find the word immediately before P1.  That's the TYPE. */
      for (i=1;   (pos = chxtoken (type, nullchix, i, comline)) >= 0;   ++i)
         if (pos >= p1)  break;
      pos = chxtoken (type, nullchix, i-1, comline);

      /*** Finally, remove the quotes and WORDS from COMLINE. */
      chxalter (comline, p1, quote, CQ(""));
      chxalter (comline, p1, words, CQ(""));
      chxalter (comline, p1, quote, CQ(""));

      /*** Find the index of the TYPE; if valid, remove it from COMLINE. */
      choice = tablematch (indextab, type);
      if (choice < 0  ||  chxlen (type) < 2)  DONE(1);
      chxalter (comline, pos, type, CQ(""));
   }

   /*** Here if no quoted strings, e.g. "LIST ITEM AUTHOR JOHN SMITH". */
   else {

      /*** Find a valid TYPE keyword... */
      for (i=1;   (pos = chxtoken (type, nullchix, i, comline)) >= 0;   ++i) {
         if ( (choice = tablematch (indextab, type)) >= 0  &&
                                      chxlen (type)  >= 2)     break;
      }
      if (pos < 0)  FAIL;

      /*** And everything after that goes in WORDS. */
      chxtoken (nullchix, words, i+1, comline);

      /*** Remove all of that stuff from COMLINE. */
      chxclear  (words2);
      chxcatsub (words2, comline, L(0), pos);
      chxcpy    (comline, words2);
   }

   DONE(2);

 done:

   chxfree ( quote );
   chxfree ( words2 );

   RETURN ( success );
}
