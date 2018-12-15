
/*** NEW_PARSE.  Parse an item/response range into an Rlist.
/
/    ok = new_parse (range, result, modifier, attach, cnum, thisitem,
/                           clearmod, mod1st, errchk, errpos);
/
/    Parameters:
/       int    ok;          (Success?)
/       Chix   range;       (string containing item/response range)
/       Rlist *result;      (place to put resulting Rlist)
/       char  *modifier;    (array of modifier results)
/       Chix   attach;      (file attachment data)
/       int    cnum;        (conference number)
/       This   thisitem;    (info about this item and resp number)
/       int    clearmod;    (clear all MODIFIER entries?)
/       int    mod1st;      (first response follows RESPONSE modifier?)
/       int    errchk;      (check for errors?)
/       int   *errpos;      (returned position of first error)
/
/    Purpose:
/       New_parse() takes a range string RANGE, and parses any
/       item/response range information in it.  This information
/       is encoded into the Rlist RESULT.
/
/    How it works:
/       New_parse() goes through several passes over the RANGE information.
/       First it parses the text of RANGE into an array of tokens; then
/       compacts the information in the tokens; translates the tokens into
/       an array of Rlists; compacts the array into a single Rlist; and
/       finally filters the resulting Rlist according to the modifier
/       information taken from RANGE.
/
/       Since new_parse() allocates the Rlist RESULT, it is up to the
/       caller to free it.  New_parse() allocates RESULT even if the
/       function fails.
/
/       MODIFIER[i] is set to 0 (not found) or 1 (found) for each of
/       the possible command line modifiers.  If CLEARMOD is true,
/       the entire MODIFIER array is cleared before parsing the modifiers.
/       (A false CLEARMOD allows new_parse() to "add" to modifiers already
/       parsed by the caller or a previous call to new_parse().)
/       The indices to the MODIFIER array are defined as macros in the
/       include file "modifier.h":
/
/           M_ITEMS       M_RESPONSES   M_NEW           M_ATTACH
/           M_UNSEEN      M_NOFORGET    M_FORGOTTEN     M_NOATTACH
/           M_FRESH       M_FROZEN      M_BRIEF         M_WAIT
/           M_PASS        M_LIFO        M_SINCE         M_NOWAIT
/           M_NOASK       M_NOCOMPLAIN  M_MULTIPLE
/
/       If an ATTACHMENT modifier is found, and it is immediately
/       followed by a quoted string, that string is placed in the
/       ATTACH argument.
/
/       The MOD1ST argument controls whether the first response selected
/       on an item follows the RESPONSE modifier.  If MOD1ST is true and
/       RANGE contains the "RESPONSE" keyword, the default "first" response
/       is 1.  Otherwise the default "first" response is 0.  (A 0 value for
/       MOD1ST is useful for parsing expressions like "ADD RESP 5", because
/       item 5 may not *have* any responses yet!)
/
/       If argument ERRCHK is false, new_parse() ignores any syntax errors
/       and tries to make the best sense it can of RANGE.  ERRPOS is set
/       to -1.
/
/       If ERRCHK is true, new_parse() fails if a syntax error is detected.
/       It returns 0, an empty RESULT Rlist, and sets ERRPOS to the
/       position number of the character in RANGE that apparently caused
/       the error.
/
/    Returns: 1 if a range containing at least one instance was found
/                  (even if that instance evaluated to no items)
/             0 if no range was found (ERRPOS set to -1)
/             0 on error (ERRPOS set to position >= 0)
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus command-line interpetation functions
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  gen/newparse.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/12/93 21:13 New function. */
/*: CN  5/18/93 13:14 Parse repl_fields manually. */
/*: CT  5/27/93 21:00 Change to use Rlist's. */
/*: CP  6/13/93 22:36 Add third level parsing. */
/*: CP  7/27/93 17:05 Correct 2nd arg to get_frozen(). */
/*: CP  8/01/93 10:50 Add a_mak...() name arg. */
/*: CP  8/03/93 12:49 Add SINCE code. */
/*: CK  8/05/93 15:37 Skip ":resp" part that has no item part. */
/*: CK  8/07/93 18:11 Initialize RLTHIS. */
/*: CP  8/09/93  0:36 Add 'attach' arg. */
/*: CP  8/10/93 11:38 Filter out negative item numbers. */
/*: CP  9/08/93 12:48 Return 0 if no instances. */
/*: CP  9/08/93 15:54 Add code for filtering against FROZEN modifier. */
/*: CP  9/09/93 16:20 Add CLEARMOD argument. */
/*: CK  9/15/93 13:33 Add 'mod1st' arg. */
/*: CK 10/06/93 14:41 New args: thisitem, errchk, errpos. */
/*: CK 10/14/93 14:39 Do not automatically set M_ITEMS. */
/*: CR 10/19/93 20:04 Pass 9, must use 1 of and_isince(), and_rsince(). */
/*: CR 10/29/93 13:51 NEW ITEMS filter must use "NEW !RESP"; ditto FRESH. */
/*: CP 11/12/93 13:22 Remove non-existent items; UNSEEN does not include NEW! */
/*: CK 11/02/93 14:38 Return error on bad SINCE date. */
/*: CK 12/01/93 17:01 Remove extra a_fre_rlist (rlset[...]). */
/*: CR  3/08/94 20:33 LAST is last non-deleted item. */
/*: CP  4/30/94 14:03 Fix incomplete handling of FORGOTTEN. */

#include <stdio.h>
#include "caucus.h"
#include "modifier.h"
#include "token.h"
#include "api.h"

#define  RLNULL       ( (Rlist) NULL)
#define  MAXTOKEN     200
#define  DEBUG        0
#define  ERROR(x)     { *errpos = x;   FAIL; }
#define  RESP_NUM(x)  (x == -999 ? thisitem->rnum : x)

extern struct master_template master;
extern struct fresh_template  fresh;
extern struct partic_t        thisconf;
extern union null_t           null;

FUNCTION  new_parse (range, result, modifier, attach, cnum, thisitem,
                            clearmod, mod1st, errchk, errpos)
   Chix   range, attach;
   Rlist *result;
   char  *modifier;
   This   thisitem;
   int    cnum, clearmod, mod1st, errchk;
   int   *errpos;
{
   static   Chix   data_fields, mod_fields, repl_fields;
   Token    token[MAXTOKEN];
   int      this, rlcount, i, first, last, field, ipart, success, type;
   int      sincepos, lastitem;
   Rlist    rlthis, rlptr, rlnew, rl;
   Rlist    rlset[100];
   Chix     itemstr, word1, since;
   Rlist    nr_to_rlist(), add_to_rlist(), and_isince(), and_rsince(),
            rl_all();

   ENTRY ("new_parse", "");

   /*** Get the dictionary strings for named (data) ranges, modifiers,
   /    and replacement (THIS, ALL, etc) fields. */
   /*** The named ranges string may eventually have to be passed in to handle
   /    ATTACHMENTS, which may have different meaning depending on
   /    verb used in command line. */
   mdtable ("gen_Anpdata", &data_fields);
   mdtable ("gen_Anpmods", &mod_fields);
   mdtable ("gen_Anprepl", &repl_fields);

   *result = a_mak_rlist("np result");
   itemstr = chxalloc (L(20), THIN, "newparse itemstr");
   word1   = chxalloc (L(20), THIN, "newparse word1");
   since   = chxalloc (L(40), THIN, "newparse since");
   rlcount = 0;

   /*** Initialize empty array of tokens. */
   *errpos = -1;
   for (this=0;   this<MAXTOKEN;   ++this) {
      token[this].num0     =  0;
      token[this].str      =  nullchix;
      token[this].hascolon =  0;
      token[this].pos      = -1;
      token[this].type     =  T_END;
   }

   /*** Pass 1: parse RANGE string into a low-level set of tokens. */
   parse2_tokens (range, token, data_fields);

   /*** Pass 2: collapse numeric tokens into [num0,num1] ranges, replace ALL
   /    with 1-LAST, and mark LAST and THIS entries with special codes. */
   if (NOT collapse_range (token, repl_fields, errchk, errpos))  FAIL;

   /*** Pass 3: merge tokens <item><colon><resp> into <item,hascolon><resp>. */
   if (NOT merge_tokens (token, errchk, errpos))                 FAIL;

   /*** Pass 4: substitute actual item(!) numbers for the codes for
   /    LAST and THIS. */
   for (lastitem=master.items;   lastitem > 0  &&  DELETED(lastitem);
      --lastitem) ;
   for (this=0;   token[this].type != T_END;   ++this) {
      if (token[this].type != T_NUM)                    continue;
      if (this > 0  &&  token[this-1].hascolon)         continue;
      if (token[this].num0 ==   -1)  token[this].num0 = lastitem;
      if (token[this].num1 ==   -1)  token[this].num1 = lastitem;
      if (token[this].num0 == -999)  token[this].num0 = thisitem->number;
      if (token[this].num1 == -999)  token[this].num1 = thisitem->number;
   }


   /*** Pass 5: Scan the tokens for modifiers (such as LIFO, NEW, etc. that
   /    are not used as part of an item:resp range).  If found, mark the
   /    appropriate element in the MODIFIERS array, and remove the
   /    modifier from the token array. */
   if (clearmod)  for (i=0;   i<MODIFIERS;   ++i)  modifier[i] = 0;
   for (this=0;   (type = token[this].type) != T_END;   ++this) {
      if (type != T_WORD  &&  type != T_NAME)                 continue;
      if (              token[this  ].hascolon)               continue;
      if (this > 0  &&  token[this-1].hascolon)               continue;
      chxtoken (word1, nullchix, 1, token[this].str);
      if ( (field = tablematch (mod_fields, word1)) < 0)      continue;
      modifier[field] = 1;
      if (field == M_SINCE) {
         chxcpy (since,  token[this].str);
         sincepos = token[this].pos + chxlen(word1) + 1;
      }
      if (field == M_ATTACH  &&  attach!=nullchix)
         chxtoken (nullchix, attach, 2, token[this].str);
      delete_token (token, this);
      --this;
   }

   /*** NOATTACH modifier wins over ATTACH. */
   if (modifier[M_NOATTACH])    modifier[M_ATTACH] = 0;

   /*** WAIT modifier is assumed to be present unless overridden by NOWAIT. */
   if (NOT modifier[M_NOWAIT])  modifier[M_WAIT]   = 1;
   if (    modifier[M_WAIT])    modifier[M_NOWAIT] = 0;

   /*** Only one of ITEMS or RESPONSES may specified. */
   if (modifier[M_ITEMS])       modifier[M_RESPONSES] = 0;

   /*** Where no explicit response ranges were given, the FIRST response
   /    to be used depends on the use of either ITEM or RESPONSE modifier. */
   first = (modifier[M_RESPONSES] && mod1st ? 1 : 0);

   /*** Pass 6: at this point, each token is either:
   /      (a) a single entity (a numeric range, a named range, etc.)
   /      (b) the "item" part of an "item:resp" entity.
   /      (c) the "resp" part of an "item:resp" entity.
   /      (d) an error.
   /    The loop that follows scans the tokens for cases (a), (b), and
   /    (c), and translates them into a set of RLCOUNT Rlists.
   /
   /    The loop is quite complicated, as it must handle the cross
   /    product of all the possible (b) cases with (c) cases. */
   rlthis  = RLNULL;
   for (this=0;   token[this].type != T_END;   ++this) {
      /*** IPART==1 means this token is an item specification, case (a)
      /    or (b).  Otherwise, this token is case (c). */
      ipart = 1;
      if (this > 0   &&  token[this-1].hascolon) {
         /*** Detect "a:b:c". */
         if (errchk  &&  token[this  ].hascolon)  ERROR (token[this+1].pos);
         ipart = 0;
      }

      /*** If this token is a word, pre-figure its field number in
      /    the table of modifier words. */
      field = -1;
      if (token[this].type == T_WORD  ||  token[this].type == T_NAME) {
         chxtoken (word1, nullchix, 1, token[this].str);
         field = tablematch (mod_fields, word1);
      }
#if DEBUG
      printf ("newparse: this=%d, type=%d, ipart=%d, field=%d, str='%s'\n",
                 this, token[this].type, ipart, field,
                 ascquick(token[this].str));
#endif

      /*** If we have a ":resp" part with no item part, error or skip it. */
      if (NOT ipart  &&  rlthis == RLNULL) {
         if (errchk)  ERROR (token[this].pos);
         continue;
      }

      /*** Item part: numeric range. */
      if (ipart  &&  token[this].type == T_NUM) {
         rlthis = rlset[rlcount++] = a_mak_rlist("np I rlthis");
         rlptr  = rlthis->next     = a_mak_rlist("np I next");
         rlptr->i0 = token[this].num0;
         rlptr->i1 = token[this].num1;
         if (rlptr->i0 < 0)  rlptr->i0 = 0;
         if (rlptr->i1 < 0)  rlptr->i1 = 0;
         rlptr->r0 = first;
         rlptr->r1 = -1;
      }

      /*** Response part: numeric range. */
      else if (NOT ipart  &&  token[this].type == T_NUM) {
         for (rl=rlthis->next;   rl != RLNULL;   rl=rl->next) {
            rl->r0 = RESP_NUM (token[this].num0);
            rl->r1 = RESP_NUM (token[this].num1);
         }
      }

      /*** Item part: named range. */
      else if (ipart  &&  token[this].type == T_NAME) {
         rl = nr_to_rlist (token[this].str, cnum, first, master.items);
  
         /*** Detect bad named range. */
         if (rl == RLNULL)  { if (errchk)  ERROR (token[this].pos); }
         else               rlthis = rlset[rlcount++] = rl;
      }

      /*** Item part: NEW. */
      else if (ipart  &&  field == 2) {
         rlthis = rlset[rlcount++] = a_mak_rlist("np IN rlthis");
         if (master.items > thisconf.items) {
            rlptr  = rlthis->next   = a_mak_rlist("np IN rlptr");
            rlptr->i0 = thisconf.items + 1;
            rlptr->i1 = master.items;
            rlptr->r0 = first;
            rlptr->r1 = -1;
         }
      }

      /*** Response part: NEW.   Build a new Rlist, starting with RLTHIS,
      /    but only incorporating responses that are new.   Then replace
      /    RLTHIS with the new Rlist. */
      else if (NOT ipart  &&  field == 2) {
         rlnew = rlptr = a_mak_rlist("np RN rlnew");
         for (rl=rlthis->next;   rl != RLNULL;   rl=rl->next) {
            for (i=rl->i0;   i<=rl->i1;   ++i) {
               if (thisconf.responses[i] < 0)                     continue;
               if (thisconf.responses[i] >= master.responses[i])  continue;
               rlptr = add_to_rlist (rlptr, i,
                         thisconf.responses[i] - thisconf.backresp[i] + 1,
                         master.responses[i]);
            }
         }
         a_fre_rlist (rlset[rlcount-1]);
         rlset[rlcount-1] = rlthis = rlnew;
      }

      /*** Item part: FRESH. */
      else if (ipart  &&  field == 6) {
         rlthis = rlset[rlcount++] = a_mak_rlist("np F rlthis");
         if (master.items > fresh.items) {
            rlptr  = rlthis->next   = a_mak_rlist("np F rlptr");
            rlptr->i0 = fresh.items + 1;
            rlptr->i1 = master.items;
            rlptr->r0 = first;
            rlptr->r1 = -1;
         }
      }

      /*** Response part: FRESH.   Build a new Rlist, starting with RLTHIS,
      /    but only incorporating responses that are fresh.   Then replace
      /    RLTHIS with the new Rlist. */
      else if (NOT ipart  &&  field == 6) {
         rlnew = rlptr = a_mak_rlist("np RF rlnew");
         for (rl=rlthis->next;   rl != RLNULL;   rl=rl->next) {
            for (i=rl->i0;   i<=rl->i1;   ++i) {
               if (fresh.responses[i] < 0)                     continue;
               if (fresh.responses[i] >= master.responses[i])  continue;
               rlptr = add_to_rlist (rlptr, i,
                         fresh.responses[i] - fresh.backresp[i] + 1,
                         master.responses[i]);
            }
         }
         a_fre_rlist (rlset[rlcount-1]);
         rlset[rlcount-1] = rlthis = rlnew;
      }

      /*** Item part: UNSEEN. */
      else if (ipart  &&  field == 3) {
         rlptr = rlthis = rlset[rlcount++] = a_mak_rlist("np U rlptr");
         for (i=1;   i<=thisconf.items;   ++i) {
            if (thisconf.responses[i] == -1)
               rlptr = add_to_rlist (rlptr, i, first, -1);
         }
      }

      /*** Item part: FORGOTTEN. */
      else if (ipart  &&  field == 5) {
         rlptr = rlthis = rlset[rlcount++] = a_mak_rlist("np FO rlptr");
         for (i=1;   i<=thisconf.items;   ++i) {
            if (thisconf.responses[i] == -2)
               rlptr = add_to_rlist (rlptr, i, first, -1);
         }
         /*** Turn on M_NOFORGET to keep these from being filtered out.
         /    Has the side affect of turning on NOFORGET for *all*
         /    instances, which is not so cool, but what else can we do? */
         modifier[M_NOFORGET] = 1;
      }

      /*** Item part: FROZEN. */
      else if (ipart  &&  field == 7) {
         rlthis = rl_all (master.items);
         and_frozen (cnum, &master, rlthis);
         rlset[rlcount++] = rlthis;
      }

      /*** Response part: SINCE.  "And" RLTHIS with responses since
      /    the date in the token. */
      else if (NOT ipart  &&  field == 11) {
         rlnew = and_rsince (rlthis, token[this].str, cnum);
         a_fre_rlist (rlset[rlcount-1]);
         rlset[rlcount-1] = rlthis = rlnew;
         if (rlnew == RLNULL)  ERROR(sincepos);
      }

      /*** Unknown token.  Probably an error. */
      else if (errchk)   ERROR (token[this].pos);
   }


   /*** If no Rlists were created, then the original RANGE had no
   /    explicit instance i:r tuples.  Assume ALL:ALL... */
   success = 1;
   if (rlcount == 0) {
      rlset[0] = rl_all (master.items);
      rl = rlset[0]->next;
      rlcount   = 1;

      /*** ...unless there were NO modifier-like instances (UNSEEN, FRESH,
      /    NEW, etc.), in which case we empty the Rlist, and fail. */
      if (NOT (modifier[M_UNSEEN]  ||  modifier[M_FRESH]  ||
               modifier[M_FROZEN]  ||  modifier[M_NEW]    ||
               modifier[M_SINCE]   ||  modifier[M_FORGOTTEN]))
         success = rl->i0 = rl->i1 = 0;
   }

   /*** Assemble everything into 1 Rlist RLNEW, removing the deleted
   /    or otherwise non-existent items. */
   rlnew = rlptr = a_mak_rlist("np assem rlnew");
   for (this=0;   this<rlcount;   ++this) {
      for (rl=rlset[this]->next;   rl!=RLNULL;   rl=rl->next) {
         for (i=rl->i0;   i<=rl->i1;   ++i) {
            if (i == 0)                          continue;
            if (i > master.items)                continue;
            if (master.responses[i] >= 0)
               rlptr = add_to_rlist (rlptr, i, rl->r0, rl->r1);
         }
      }
   }

   /*** Pass 7: create the penultimate RESULT list, by filtering out the
   /    items and responses that do not meet the requirements of the
   /    modifiers. */
   rlptr = *result;
   for (rl=rlnew->next;   rl!=RLNULL;   rl=rl->next) {
      for (i=rl->i0;   i<=rl->i1;   ++i) {
         if (i == 0)                            continue;
         first = rl->r0;
         last  = rl->r1;

         /*** Filter out forgotten items (unless NOFORGET or FORGOTTEN
         /    modifiers were specified). */
         if (thisconf.responses[i] == -2) {
            if (NOT modifier[M_NOFORGET]  &&  NOT modifier[M_FORGOTTEN])
               continue;
         }

         /*** Filter out everything *but* forgotten items if FORGOTTEN
         /    modifier specified. */
         else if (modifier[M_FORGOTTEN])  continue;

         /*** Filter: UNSEEN. */
         if (modifier[M_UNSEEN]  &&
            (i > thisconf.items  ||  thisconf.responses[i] != -1))  continue;

         /*** Filter: NEW ITEMS. */
         if ( (NOT modifier[M_RESPONSES])  &&  modifier[M_NEW]  &&
                 i <= thisconf.items)           continue;

         /*** Filter: NEW RESPS. */
         if (modifier[M_RESPONSES]  &&  modifier[M_NEW]) {
            if (thisconf.responses[i] < 0)    /* UNSEEN   */   continue;
            if (thisconf.items        < i)    /* NEW ITEM */   continue;
            if (thisconf.responses[i] >= master.responses[i])  continue;
            first = max (first, thisconf.responses[i]
                              - thisconf.backresp[i] + 1);
            last  = min (last,  master.responses[i]);
            if (first > last  &&  last != -1)                  continue;
         }
  
         /*** Filter: FRESH ITEMS. */
         if ( (NOT modifier[M_RESPONSES])  &&  modifier[M_FRESH]  &&
                 i <= fresh.items)                             continue;

         /*** Filter: FRESH RESPS. */
         if (modifier[M_RESPONSES]  &&  modifier[M_FRESH]) {
            if (fresh.responses[i] < 0)  /* UNSEEN     */      continue;
            if (fresh.items        < i)  /* FRESH ITEM */      continue;
            if (fresh.responses[i] >= master.responses[i])     continue;
            first = max (first, fresh.responses[i]
                              - fresh.backresp[i] + 1);
            last  = min (last,  master.responses[i]);
            if (first > last  &&  last != -1)                  continue;
         }

         /*** Finally, after having passed all the filters, add this
         /    item:resp instance to the new Rlist. */
         rlptr = add_to_rlist (rlptr, i, first, last);
      }
   }
   a_fre_rlist (rlnew);

   /*** Pass 8: if FROZEN was specified as a modifier, reduce RESULT to only
   /    those items which are frozen. */
   if (modifier[M_FROZEN])   and_frozen (cnum, &master, *result);

   /*** Pass 9: the very last step is to filter RESULT against the
   /    "ITEM SINCE" or "RESP SINCE" modifier, if present. */
   if (modifier[M_SINCE]  &&  NOT EMPTYCHX(since)) {
      if (modifier[M_RESPONSES])  rlnew = and_rsince (*result, since, cnum);
      else                        rlnew = and_isince (*result, since, cnum);
      a_fre_rlist (*result);
      *result = rlnew;
      if (rlnew == RLNULL)  ERROR(sincepos);
   }

 done:
   for (this=0;   this<rlcount;   ++this)  a_fre_rlist (rlset[this]);
   while   (token[0].type != T_END)  delete_token (token, 0);
   chxfree (itemstr);
   chxfree (word1);
   chxfree (since);
   RETURN  (success);
}

#if DEBUG

FUNCTION  debug_token (token, str)
   Token  token[];
   char  *str;
{
   int    this;
   char   junk[20];

   printf ("--------------------------------------------------\n");
   printf ("%s\n", str);

   /*** Debugging code to display TOKEN array. */
   for (this=0;   token[this].type != T_END;   ++this) {
      if (this > 0  &&  token[this-1].hascolon)  printf ("      ");
      if (token[this].type == T_NUM)
         printf ("   %d (%d,%d)\n", token[this].type,
                                    token[this].num0, token[this].num1);
      else
         printf ("   %d %s\n", token[this].type, ascquick(token[this].str));
   }
   fflush (stdout);
   gets   (junk);
}
#endif
