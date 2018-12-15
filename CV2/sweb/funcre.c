
/*** FUNC_RE.   Return the resulting value of a re_xxx() function.
/
/    func_re (result, what, arg, int reload)
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/       int                 reload;  (force cache reload for response text)
/
/    Purpose:
/       Append value of re_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funcre.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/31/95 15:29 New function. */
/*: CR  3/20/00 14:07 Sanity-check cnum argument. */
/*: CR  4/13/00 12:44 Test new member resp->exists in all cases. */
/*: CR 01/13/06 re_exists() checks for at least an author or some text */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern int    cml_debug;
extern Vartab g_icache;

extern Cstat  g_cstat;

FUNCTION  func_re (Chix result, char *what, Chix arg, int reload)
{
   static Resp   resp  = (Resp)  NULL;
   static Chix   empty = (Chix) NULL;
   static Chix   word;
   static int4   cnum, inum, rnum;
   char          temp[200];
   int4          cnew, pos, pos0;
   int           code, slot, rc;
   int4          epoch_of();
   ENTRY ("func_re", "");

   /*** Initialization. */
   if (resp  == (Resp) NULL) {
      resp  = a_mak_resp   ("func_re resp");
      empty = chxalloc     ( L(4), THIN, "func_re empty");
      word  = chxalloc     (L(80), THIN, "func_re word");
   }

   /*** Clear cached response object. */
   if (streq (what, "re_clear")) { resp->cnum = -1;    RETURN(1); }

   /*** All re_xxx() functions take a conference number, item number,
   /    and response number.  Decode them. */
   pos  = L(0);
   cnew = chxint4 (arg, &pos);

   /*** If no args, attempt to use same ones as last call, unless
   /    this is clearly impossible. */
   if (pos  == L(0)  &&  resp->inum < 0)  { 
      if (streq (what, "re_exists"))  chxcatval (result, THIN, L('0'));
      RETURN (1);
   }

   /*** If args are specified, decode them. */
   if (pos != L(0)) {
      cnum = cnew;
      inum = chxint4 (arg, &pos);
      pos0 = pos;
      rnum = chxint4 (arg, &pos);
      if (cnum <= 0  ||  pos == pos0)  { 
         if (streq (what, "re_exists"))  chxcatval (result, THIN, L('0'));
         RETURN (1);
      }
   }
   
   /*** $re_title() is special, it has its own cache. */
   if (streq (what, "re_title")) {
      slot = item_cache (cnum, inum, g_icache, resp);
      if (slot >= 0) chxcat (result, g_icache->name[slot]);
      RETURN(1);
   }

   /*** So is $re_owner(), for items. */
   if (streq (what, "re_owner")  &&  rnum==0) {
      slot = item_cache (cnum, inum, g_icache, resp);
      if (slot >= 0) {
         chxtoken (word, nullchix, 1, g_icache->value[slot]);
         chxcat   (result, word);
      }
      RETURN(1);
   }

   /*** So is $re_author(), for items. */
   if (streq (what, "re_author")  &&  rnum==0) {
      slot = item_cache (cnum, inum, g_icache, resp);
      if (slot >= 0) {
         chxtoken (nullchix, word, 2, g_icache->value[slot]);
         chxcat   (result, word);
      }
      RETURN(1);
   }

   /*** If a different response number, or a forced reload, or a
   /    non-existent response from more than 5 seconds ago... load resp! */
   if (resp->cnum != cnum  ||  resp->inum != inum  ||  resp->rnum != rnum  ||
       reload  ||  (NOT resp->exists && systime() > resp->lastcall + 5) ) {
      resp->cnum = cnum;
      resp->inum = inum;
      resp->rnum = rnum;
      rc = a_get_resp (resp, P_EVERY, A_WAIT);
/*    sprintf (temp, "(%d %d %d) = %d", cnum, inum, rnum, rc); */
/*    logger  (1, LOG_FILE, temp); */

      if (rc == A_OK)  
           removeid (resp->author, resp->author, resp->owner, empty);
   }


   if      (streq (what, "re_exists")) chxcatval (result, THIN, 
        (resp->exists  &&  
           ( (chxvalue (resp->owner, L(0)) != 0)  ||  resp->text->lines > 0) 
        ) ? '1' : '0');

   else if (streq (what, "re_bits")) {
      sprintf (temp, "%d", resp->exists ? resp->bits : 0);
      chxcat  (result, CQ(temp));
   }

   else if (streq (what, "re_copier")) {
      if (resp->exists) {
         chxtoken (word, nullchix, 1, resp->copied);
         chxcat   (result, word);
      }
   }

   else if (streq (what, "re_copied")) {
      if (resp->exists) {
         chxtoken (nullchix, word, 2, resp->copied);
         chxcat   (result, word);
      }
   }

   else if (streq (what, "re_time")) {
      if (resp->exists) chxcat (result, resp->date);
   }

   else if (streq (what, "re_author")) { 
      if (resp->exists) chxcat (result, resp->author);
   }

   else if (streq (what, "re_epoch")) {
      sprintf (temp, "%d", resp->exists ? epoch_of (resp->date) : 0);
      chxcat  (result, CQ(temp));
   }

   else if (streq (what, "re_text"))  {
      if (resp->exists)  chxcat (result, resp->text->data);
   }

   else if (streq (what, "re_owner")) {
      if (resp->exists)  chxcat (result, resp->owner);
   }

   else if (streq (what, "re_prop"))  {
      sprintf (temp, "%d", (resp->exists ? resp->textprop & M_TEXTTYPE : 0));
      chxcat  (result, CQ(temp));
   }

   else if (streq (what, "re_delete")) {
      code = a_del_resp (resp, P_EVERY, A_WAIT);
      if (code == A_OK  &&  resp->rnum == 0) {
         g_cstat->cnum = 0;
         touch_it (cnum, 0, 0);
      }
   }

   RETURN(1);
}
