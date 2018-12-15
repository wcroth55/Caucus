
/*** NLUNIQUE.   Ask user to reduce a list of userids to one.
/
/    NLUNIQUE is called when we have a namelist containing more
/    than one userid, and we need to ask the user to whittle the
/    list down to one person.
/
/    NAME is the original "name" typed by the user that produced the
/    list of multiple userids.  PEOPLE is the namelist of userids.
/    STR is the name of a master dictionary prompt string.
/
/    NLUNIQUE displays the list of names corresponding to the userids
/    in PEOPLE, with a number preceeding each name.  It then prompts
/    the user with STR to choose one of the people, by number.
/    
/    Allocates and returns a one-element namelist if successful, which
/    must be nlfree'd by the caller.  Otherwise, returns null.nl. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  9/06/89 16:30 Created function. */
/*: CR 10/09/89 11:32 Use XW unit codes. */
/*: CR 10/10/89 16:39 Add header comments. */
/*: CR 10/11/89 10:19 Simplify use of ppl_Tlist. */
/*: CR 10/30/89 12:02 Remove ok, and bad decl. of nladd(). */
/*: JV  3/17/90 19:51 Removed stdio.h */
/*: CR  3/21/90 18:48 Replace null.str with "" in mdprompter(). */
/*: CR  8/09/90 20:21 Remove decl' of nlfree(). */
/*: JV  7/22/90 16:46 Removed decl of nlfree(). */
/*: DE  4/14/92 16:56 Chixified */
/*: CR 10/11/92 13:26 Check return value from chxnum. */

#include "caucus.h"

extern union null_t null;

FUNCTION  struct namlist_t *nlunique (name, people, str)
   Chix   name;
   char  *str;
   struct namlist_t *people;
{
   struct namlist_t *p, *nlnode(), *nlexact, *this, *success;
   short  i;
   int4   person;
   Chix   id, answer;
   Chix   newstr, format;

   ENTRY ("nlunique", "");

   id       = chxalloc (L(50),  THIN, "nlunique id");
   answer   = chxalloc (L(80),  THIN, "nlunique answer");
   newstr   = chxalloc (L(100), THIN, "nlunique newstr");
   format   = chxalloc (L(100), THIN, "nlunique format"); 

   mdstr   (format, "ppl_Tlist", null.md);
   chxformat (newstr, format, L(0), L(0), name, null.chx);
   unit_write (XWTX, newstr);

   sysbrkclear();
   for (p=people->next, i=0;   p!=null.nl;   p = p->next) {
      if (sysbrktest())  break;
      chxtoken  (id,  null.chx,  2,  p->str);
      person_is (XWTX, id, 0, null.par, ++i);
   }
   unit_write (XWTX, CQ("\n"));

   while (1) {
      if (mdprompter (str, null.chx, 1, "", 5, answer, CQ("0")) < -1)
         DONE(null.nl); /* Bad error. Drop Lead Weight on operator's toes. */
      /* Get number of exact person. */
      if (chxnum (answer, &person)  &&  person>0) {;
         nlexact = nlnode (1);        /* Create namelist for exact match. */
         for (this=people->next, i=1; /* Look thru matches for exact one. */
              this != null.nl && i != person; this=this->next, i++);
         if (this == null.nl) {       /* User entered a number greater  */
            mdwrite (XWTX, "fnd_Enumber", null.md);
            continue; }               /* than the last user on the list!*/
	 nladd (nlexact, this->str, 0);    /* This is the exact match! */
         DONE(nlexact);
      }
      else break;      /* User selected 0, "none of the above", */
   }                   /* or entered text. Same thing. */
   DONE(null.nl);

 done:

   chxfree ( id );
   chxfree ( answer );
   chxfree ( newstr );
   chxfree ( format );

   RETURN ( success );
}
