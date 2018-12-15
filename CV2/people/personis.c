
/*** PERSON_IS.  Display information about PERSON on unit OUT.
/
/    PERSON is the userid of the person in question.  Displays
/    all information about PERSON if FULL is true, otherwise
/    just a one-line display.
/
/    If PARTIC is non-null, PERSON_IS loads the user's participation
/    file for this conference into PARTIC.   If the person is not a
/    member of the conference, PARTIC->ITEMS is set to -1.
/
/    NUMBER is a positive integer if a number should be displayed
/    in parentheses before the person is listed.  This is for listing
/    multiple, one-line, people displays.
/
/    PERSON_IS returns 1 if successful, and 0 otherwise. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR  2/14/89 12:19 Return immediately if loadreg fails. */
/*: AM  2/07/89 10:31 Jcaucus changes*/
/*: JV  9/06/89 10:33 Added NUMBER parameter. */
/*: CR  1/26/90 21:51 Set partic->items to -1 if not a member! */
/*: CR  8/12/90 15:24 Do not explicitly display userid; it's part of name. */
/*: CR  9/26/90 10:30 Make Lint happy if JAPAN is OFF. */
/*: CR  1/31/91 13:08 Use person_format() to display person info. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  7/16/91 17:08 Add HOST argument to person_format() calls. */
/*: CR  7/17/91 12:33 Make person_is handle remote people intelligently. */
/*: CR  7/30/91 18:14 Add 3rd arg to loadreg(). */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: DE  4/27/92 17:24 Chixified*/
/*: JX  5/15/92 18:04 Remove JAPAN. */
/*: JX  6/18/92 12:59 Allocate WHO. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CR 10/12/01 16:40 Ignore @'s!  Replace with "^" for now. */
/*: CR  8/20/04 Add XUPA argument to load_user() call. */

#include "caucus.h"
#include <stdio.h>

extern struct flag_template flag;
extern union  null_t        null;
extern Chix                 confname, ss_lparen, ss_rparen;
extern short                confnum;

FUNCTION  person_is (out, person, full, partic, number)
   int    out, full, number;
   Chix   person;
   struct partic_t *partic;
{
   Userreg         who, make_userreg();
   static Chix     s_laston, s_pfull, s_pshort;
   Chix   member, phone, name, numstr, id, host, localhost;
   int    local, success;
   Chix   result, format,  newstr, tempchx;
   Chix   person_format();

   ENTRY ("person_is", "");

   member    = chxalloc (L(50),  THIN, "person_is member");
   numstr    = chxalloc (L(40),  THIN, "person_is numstr");
   id        = chxalloc (L(80),  THIN, "person_is id");
   host      = chxalloc (L(100), THIN, "person_is host");
   localhost = chxalloc (L(100), THIN, "person_is localhost");
   format    = chxalloc (L(100), THIN, "person_is format");
   newstr    = chxalloc (L(100), THIN, "person_is newstr");
   tempchx   = chxalloc (L(100), THIN, "person_is tempchx");
/* result    is   allocated by called function */
   success   = 0;

   who = make_userreg();

   /*** Find out if this person is LOCAL to this host. */
   getmyhost (localhost);
   chxbreak  (person, id, host, CQ("^"));
   local = (EMPTYCHX(host)  ||  chxeq (host, localhost));

   /*** Load the user registration data about PERSON. */
   chxclear (who->name);
   chxclear (who->phone);
   chxclear (who->laston);
   chxcpy (who->id, id);
   if (local  &&  NOT loadreg (person, who, NULLSITE))  FAIL;
   chxclear (member);

   name  = who->name;
   phone = who->phone;

   /*** If FULL, display name, phone, userid, intro, last time on. */
   if (full) {
      mdstr (format, "peo_Ffull", &s_pfull);
      result = person_format (format, name, who->id, host, phone, who->briefs);
      unit_write (out, result);
      chxfree (result);

      /*** Display date the user was last on any conference. */
      mdstr   (format, "peo_Flaston", &s_laston);
      chxformat (newstr, format, L(0), L(0), who->laston, null.chx);
      unit_write (out, newstr);
 
      /*** Fetch the date the user was last IN this conference. */
      if (local  &&  unit_lock (XUPA, READ, L(confnum), L(0), L(0), L(0),
          person)) {
         if (partic != null.par) {
            partic->items = -1;
            if (load_user (partic, XUPA))  chxcpy (member, partic->lastin);
         }
         else if (unit_view (XUPA)) {
            unit_read (XUPA, member, L(0));
            unit_close(XUPA);
         }
         unit_unlk (XUPA);
      }

      /*** Display when user was last IN this conference. */
      if (NOT EMPTYCHX(member)) {
         mdstr   (format, "peo_Flastin", null.md);
         chxclear  (newstr);
         chxformat (newstr, format, L(0), L(0), confname, member);
         unit_write (out, newstr);
      }
   }


   /*** Short form displays number (if requested), 
   /    "*" if a member, name, userid. */
   else {
      if (number)
          chxformat (numstr, CQ("%1s%2d%1s"), L(number), L(0), ss_lparen, 
                   ss_rparen);
      else         chxclear(numstr);

      if (local  &&  unit_lock (XUPA, READ, L(confnum), L(0), L(0), L(0),
          person)) {
         if (unit_check (XUPA))  {
            chxcpy    (tempchx,CQ("*"));
            chxcatsub (tempchx, member, L(1), ALLCHARS);
            chxcpy    (member, tempchx);
	  }
         unit_unlk  (XUPA);
      }

      chxclear  (newstr);
      chxformat (newstr, CQ("%4s %1s"), L(0), L(0), numstr, member);
      unit_write (out, newstr);

      mdstr (format, "peo_Fshort", &s_pshort);
      result = person_format (format, name, who->id, host, phone, who->briefs);
      unit_write (out, result);
      chxfree (result);
   }

   SUCCEED;

 done:

   chxfree (member);
   chxfree (numstr);
   chxfree (id);
   chxfree (host);
   chxfree (localhost);
   chxfree (format);
   chxfree (newstr);
   chxfree (tempchx);
   free_userreg (who);

   RETURN ( success );
}
