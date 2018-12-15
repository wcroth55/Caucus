
/*** PERSON_FORMAT.  Format a person's name & other info.
/
/    s = person_format (fstr, name, userid, host, phone, intro);
/
/    Parameters:
/       Chix   s;                Resulting string.
/       Chix   fstr;             Format code string.
/       Chix   name;             User's full name, including "(userid)".
/       Chix   userid;           User's userid.
/       Chix   host;             User's host.
/       Chix   phone;            User's telephone;
/       struct namlist_t *intro; Namelist of brief introduction.
/
/    Purpose:
/       Place the name, userid, host, phone, and brief introduction of a 
/       user into a string, according to the format code in FSTR.  FSTR 
/       may contain arbitrary text, plus the format codes &a, &u, &p, &x,
/       &h, and &z.  
/
/       Person_format allocates the returned string; the caller must
/       free it with chxfree().
/
/    How it works:
/
/    Returns: a pointer to a newly-allocated string containing the result.
/             Or NULL if an error occurred.
/
/    Error Conditions:
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  person_is(), ...
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  people/personfo.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/31/91 12:41 New function. */
/*: CR  4/25/91  1:00 Declare strscan(). */
/*: CR  6/20/91 14:00 Handle &h host information. */
/*: CR  7/16/91 17:02 Add HOST argument. */
/*: CR 11/20/91 14:22 Split @host out of userid properly. */
/*: DE  4/15/92 11:40 Chixified */
/*: CX  5/26/92 14:36 Rewrite to not use buildstr, fix misc errors. */
/*: JX  6/30/92 16:26 Change &z formatting to correctly count jix. */
/*: CR 10/21/92 15:14 Remove commented-out piece. */
/*: CR 10/22/92 16:12 Remove unused reslen. */
/*: CR 12/10/92 15:32 chxindex 2nd arg is int4. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR  2/05/93 15:15 chxfree temp1. */
/*: CR 10/12/01 16:40 Ignore @'s!  Replace with "^" for now. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  Chix person_format (fstr, name, userid, host, phone, intro)
   Chix   fstr, name, userid, host, phone;
   struct namlist_t *intro;
{
   static Chix atsign;
   struct namlist_t *b;
   Chix   authorname, how_s, temp, athost, at, id, res, temp1;
   int4   t, s, fstr_len, i, pos;
   int4   length;
   int    maxchar, maxscr;

   ENTRY ("person_format", "");

   at          = chxalloc ( L(20), THIN, "person_format at");
   id          = chxalloc ( L(80), THIN, "person_format id");
   res         = chxalloc (L(200), THIN, "person_format res");
   temp        = chxalloc (L(200), THIN, "person_format temp");
   temp1       = chxalloc ( L(80), THIN, "person_format temp1");
   how_s       = chxalloc ( L(80), THIN, "person_format how_s");
   athost      = chxalloc (L(100), THIN, "person_format athost");
   authorname  = chxalloc ( L(80), THIN, "person_format authorname");

   /*** Pluck out the author's name, and process it to get name,
   /    "(userid)", and host parts. */
   chxbreak  (userid, id, athost, CQ("^"));
   removeid  (authorname, name, id, athost);
   mdstr (at, "peo_Katsign", &atsign);
   if (NOT EMPTYCHX(host))    chxcpy  (athost, host);
   if (NOT EMPTYCHX(athost))  chxconc (athost, at, athost);

   /*** Loop through the format string, processing all "&x" directives,
   /    and displaying all other text as is.  */
   for (s=0, fstr_len=chxlen(fstr);   s < fstr_len;  ) {

      /*** If no more "&"s left, display to the end of the string. */
      if ( (t = chxindex (fstr, s, CQ("&"))) < 0) {
         chxcatsub  (res, fstr, s, ALLCHARS);
         break;
      }

      /*** Treat escaped "&"s as ordinary text to be displayed. */
      else if (chxvalue (fstr, L(t+1)) == '&') {
         chxcatsub (res, fstr, s, t-s+1);
         s = t + 2;
         continue;
      }

      /*** Remaining code in loop processes "&x" forms.   Start by 
      /    displaying all text before the "&". */
      chxcatsub (res, fstr, s, t-s);
      s = t + 1;

      /*** Parse out the string length, including "-" => left-justify, and
      /    make a format string to use to display the info. */
      length = chxint4 (fstr, &s);
      if (length != 0) {
         chxclear  (how_s);
         chxformat (how_s, CQ("%%%ds"), length, L(0), null.chx, null.chx);
      }
      else chxcpy  (how_s, CQ("%s"));


      /*** Decide what to display according to "x" code in "&x". */
      chxclear (temp);
      switch (chxvalue (fstr, L(s))) {

         case ('a'):                                            /* Author. */
            chxformat (temp, how_s, L(0), L(0), authorname, null.chx);
            break;
         case ('p'):                                            /* Phone.  */
            chxformat (temp, how_s, L(0), L(0), phone, null.chx);
            break;
         case ('u'):                                            /* userid. */
            chxformat (temp, how_s, L(0), L(0), id, null.chx);
            break;

         case ('h'):                                            /* Host.   */
            if (NOT EMPTYCHX(athost))
               chxformat (temp, how_s, L(0), L(0), athost, null.chx);
            break;

         case ('z'):                                            /* Tab pos. */
            /*** Find the last \n.  Do we need to add blanks to pad out to
            /    LENGTH chars, or do we need to take a substring to trim
            /    down to that length? */
            pos    = chxrevdex (res, ALLCHARS, CQ("\n")) + 1;
            chxcatsub (temp1, res, pos, ALLCHARS);
            /* Get length of this line so far. */
            jixscrsize (temp1, 1000, &maxchar, &maxscr);
            if (length >= maxscr) {                      /* Pad. */
               for (i=maxscr;   i<length;   ++i)
                  chxcatval (temp, THIN, L(' '));
            }
            else {                                     /* Substring. */
               chxcatsub (temp, res, L(0), pos+maxscr);
               chxclear  (res);
            }
            break;

         /*** User's brief introduction.  In this case only, the
         /    length field refers to the number of spaces to indent
         /    each line of text. */
         case ('x'):
            if  (length > 0)  
               chxformat (temp, how_s, L(0), L(0), CQ(" "), null.chx);
            for (b=intro->next;   b!=null.nl;   b = b->next) {
               if (length > 0)  chxcat (res, temp);
               chxcat (res, b->str);
               chxcat (res, CQ("\n"));
               if (sysbrktest())  break;
            }
            chxclear(temp);
            break;

         default: chxclear(temp);
            break;
      }

      chxcat (res, temp);
      ++s;
   }

   chxfree ( authorname );
   chxfree ( how_s );
   chxfree ( temp );
   chxfree ( athost );
   chxfree ( at );
   chxfree ( id );
   chxfree ( temp1 );

   RETURN (res);
}
