
/*** FUNC_IT.   Return the resulting value of a it_xxx() function.
/
/    func_it (result, what, arg, conf, protect)
/   
/    Parameters:
/       Chix    result;  (append resulting value here)
/       char   *what;    (function name)
/       Chix    arg;     (function argument)
/       SWconf  conf;
/       int     protect; (Is this a "protected", e.g. inside a CML response,
/                         call that should not allow certain functions
/                         to work?)
/    Purpose:
/       Append value of it_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funcit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/31/95 15:29 New function. */
/*: CR  3/20/00 13:30 Sanity check cnum arg. */
/*: CR  4/13/00 12:44 Test new member resp->exists. */
/*: CR  5/03/0l Return rational values if cnum is bad, instead of "NO CONF"! */
/*: CR  5/02/04 Add $it_cnums(userid), return list of confs userid belongs to */
/*: CR  8/20/04 Add $it_hidden(), $set_it_hidden(). */
/*: CR  8/25/04 Apply hidden status to it_wnew, it_inew, etc. */
/*: CR  8/01/05 Add "hidden" value -3 => retired */
/*: CR 10/09/05 Major rewrite of it_resign. */
/*: CR 10/28/05 Allow $it_last(), $it_exists() even if not a member. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"
#include "unitcode.h"

extern Cstat g_cstat;
extern Pstat g_pstat, g_pstat2, g_hidden;

extern FILE *sublog;
extern int   cml_debug;

FUNCTION  func_it (Chix result, char *what, Chix arg, SWconf conf, int protect)
{
   static Resp  resp  = (Resp)  NULL;
   static Namex namex = (Namex) NULL;
   static Chix  word  = nullchix;
   static Chix  uid;
   Rlist  rl;
   int   *cnumlist;
   char   temp[200];
   static int4 cnum = 0;
   static int4 inum;
   static int4 rnum;
   Pstat       pstat;
   int4   pos, pos1, cnew, hidevalue;
   int    yes, new, this, sum, rlast, status, err, i;
   int   *a_lis_pstat();
   ENTRY ("func_it", "");

   if (resp  == (Resp)  NULL)  resp  = a_mak_resp  ("func_it resp");
   if (namex == (Namex) NULL)  namex = a_mak_namex ("func_it namex");
   if (word  == nullchix) {
      word  = chxalloc (L(30), THIN, "func_it word");
      uid   = chxalloc (L(30), THIN, "func_it uid");
   }

   /*** $it_cnums(userid) is unique in its calling sequence. */
   if (streq (what, "it_cnums")) {
      cnumlist = a_lis_pstat (arg);
      for (i=0;   cnumlist[i];   ++i) {
         sprintf (temp, "%d ", cnumlist[i]);
         chxcat  (result, CQ(temp));
      }
      sysfree ( (char *) cnumlist);
      RETURN(1);
   }

   /*** All remaining it_xxx() functions take a conference number (and often
   /    an item number) as argument(s).  Decode them. */
   pos  = L(0);
   cnew = chxint4 (arg, &pos);
   pos1 = pos;

   /*** If no args, try to use same ones as last call. */
   err = 0;
   if      (pos == L(0)  &&  cnum == 0)   err = 1;
   else if (pos != L(0)) {
      cnum = cnew;
      inum = chxint4 (arg, &pos);
      if (pos == pos1)  inum = 0;
      rnum = chxint4 (arg, &pos);
      if (cnum <= 0)                      err = 1;
   }

   if (NOT err  &&  NOT touch_it (cnum, conf->reload, conf->cache_timeout)) 
      err = 1;

   /*** $IT_HOWMUCH (cnum inum uid) */
   if (streq (what, "it_howmuch")) {
      if (err) this = -1;
      else {
         chxtoken  (word, nullchix, 3, arg);
         if (chxeq (word, g_pstat->owner))  pstat = g_pstat;
         else {
            if (NOT chxeq (word, g_pstat2->owner)  ||  g_pstat2->cnum != cnum) {
               chxcpy (g_pstat2->owner, word);
               g_pstat2->cnum  = cnum;
               g_pstat2->items = 0;
               if (a_get_pstat (g_pstat2, P_EVERY, A_WAIT) != A_OK)  
                  g_pstat2->items = -1;
            }
            pstat = g_pstat2;
         }
   
         if (inum <= pstat->items)  this = pstat->resps[inum];
         else                       this = -1;
      }

      sprintf (temp, "%d", this);
      chxcat  (result, CQ(temp));
   }

   /*** $IT_MEMBER(cnum). */
   if      (streq (what, "it_member"))
      chxcatval (result, THIN, L(err || g_pstat->items < 0 ? '0' : '1'));

   /*** $IT_JOIN(cnum).   Join conference if (a) not already a member,
   /    and (b) access is allowed. */
   else if (streq (what, "it_join")) {
      if (err)  yes = 0;
      else {
         yes = 1;
         if (g_pstat->items < 0) {
            if (conf_access (cnum) >= privValue("minimum"))
                 yes = (a_add_pstat (g_pstat, P_EVERY, A_WAIT) == A_OK);
            else yes = 0;
         }
      }
      chxcatval (result, THIN, L(yes ? '1' : '0'));
   }

   /*** $IT_LAST(cnum). */
   else if (streq (what, "it_last")) {
/*    this = (NOT err  &&  g_pstat->items >= 0 ? g_cstat->items : 0); */
      this = (NOT err ? g_cstat->items : 0);
      sprintf (temp, "%d", this);
      chxcat  (result, CQ(temp));
   }

   /*** $IT_INEW(cnum). */
   else if (streq (what, "it_inew")) {
      sum = 0;
      if (NOT err  &&  g_pstat->items >= 0) {
         for (this=g_pstat->items + 1;  this <= g_cstat->items;  ++this) {
            if (g_cstat->resps[this]  < 0)        continue;
            if (g_hidden->items       >= this &&  
                g_hidden->resps[this] <= -2)      continue;
            ++sum;
         }
      }
      sprintf (temp, "%d", sum);
      chxcat  (result, CQ(temp));
   }

   /*** $IT_IUNSEEN(cnum). */
   else if (streq (what, "it_iunseen")) {
      sum = 0;
      if (NOT err) {
         for (this=1;  this <= g_pstat->items;  ++this) {
            if (g_cstat->resps[this]   <  0)      continue;
            if (g_pstat->resps[this]  != -1)      continue;
            if (g_hidden->items       >= this &&  
                g_hidden->resps[this] <= -2)      continue;
            ++sum;
         }
      }
      sprintf (temp, "%d", sum);
      chxcat  (result, CQ(temp));
   }

   /*** $IT_ICOUNT(cnum). */
   else if (streq (what, "it_icount")) {
      sum = 0;
      if (NOT err) {
         for (this=1;   this <= g_cstat->items;   ++this) {
            if (g_cstat->resps[this] >= 0)  ++sum;
         }
      }
      sprintf (temp, "%d", sum);
      chxcat  (result, CQ(temp));
   }

   /*** $IT_IFORGOT(cnum). */
   else if (streq (what, "it_iforgot")) {
      sum = 0;
      if (NOT err) {
         for (this=1;   this <= g_pstat->items;   ++this) {
            if (g_pstat->resps[this] == -2)  ++sum;
         }
      }
      sprintf (temp, "%d", sum);
      chxcat  (result, CQ(temp));
   }

   /*** $IT_RNEW(cnum). */
   else if (streq (what, "it_rnew")) {
      sum = 0;
      if (NOT err) {
         for (this=1;   this <= g_pstat->items;   ++this) {
            if (g_cstat->resps[this]  < 0)         continue;
            if (g_pstat->resps[this]  < 0)         continue;
            if (g_hidden->items       >= this &&  
                g_hidden->resps[this] <= -2)       continue;

            sum += (g_cstat->resps[this] - g_pstat->resps[this]);
            /* what about 'back' elements? */
         }
      }
      sprintf (temp, "%d", sum);
      chxcat  (result, CQ(temp));
   }

   /*** $IT_WNEW(cnum). */
   else if (streq (what, "it_wnew")) {
      sum = 0;
      if (NOT err) {
         for (this=1;   this <= g_pstat->items;   ++this) {
            if (g_cstat->resps[this]  < 0)                     continue;
            if (g_pstat->resps[this]  < 0)                     continue;
            if (g_hidden->items       >= this &&  
                g_hidden->resps[this] <= -2)                   continue;
            if (g_pstat->resps[this]  < g_cstat->resps[this])  ++sum;
            /* what about 'back' elements? */
         }
      }
      sprintf (temp, "%d", sum);
      chxcat  (result, CQ(temp));
   }

   /*** $IT_LISTINEW(cnum). */
   else if (streq (what, "it_listinew")  &&  NOT err) {
      if (g_pstat->items >= 0) {
         for (this=g_pstat->items + 1;   this <= g_cstat->items;  ++this) {
            if (g_cstat->resps[this] < 0)         continue;
            if (g_hidden->items       >= this &&  
                g_hidden->resps[this] <= -2)      continue;
            sprintf (temp, "%d %d 0 ", cnum, this);
            chxcat  (result, CQ(temp));
         }
      }
   }

   /*** $IT_LISTIUNSEEN(cnum). */
   else if (streq (what, "it_listiunseen")  &&  NOT err) {
      if (g_pstat->items >= 0) {
         for (this=1;  this <= g_pstat->items;  ++this) {
            if (g_cstat->resps[this]   <  0)      continue;
            if (g_pstat->resps[this]  != -1)      continue;
            if (g_hidden->items       >= this &&  
                g_hidden->resps[this] <= -2)      continue;
            sprintf (temp, "%d %d 0 ", cnum, this);
            chxcat  (result, CQ(temp));
         }
      }
   }

   /*** $IT_LISTRNEW(cnum). */
   else if (streq (what, "it_listrnew")  &&  NOT err) {
      for (this=1;   this <= g_pstat->items;   ++this) {
         if (g_cstat->resps[this]  <  0)                     continue;
         if (g_pstat->resps[this]  <  0)                     continue;
         if (g_pstat->resps[this]  >= g_cstat->resps[this])  continue;
         if (g_hidden->items       >= this &&  
             g_hidden->resps[this] <= -2)                    continue;
         sprintf (temp, "%d %d %d ", cnum, this, g_pstat->resps[this] + 1);
         chxcat  (result, CQ(temp));
         /* what about 'back' elements? */
      }
   }

   /*** $IT_LISTTITLE(cnum title_words) */
   else if (streq (what, "it_listtitle")  &&  NOT err) {
      namex->cnum = cnum;
      chxclear  (namex->namepart);
      chxcatsub (namex->namepart, arg, pos1, ALLCHARS);
      if (chxvalue (namex->namepart, L(0)) != 0  &&  
          a_get_namex (namex, P_TITLE, A_WAIT) == A_OK) {
         for (rl=namex->rlist->next;   rl != (Rlist) NULL;   rl=rl->next) {
            for (this=rl->i0;   this<=rl->i1;   ++this) {
               sprintf (temp, "%d %d 0 ", cnum, this);
               chxcat  (result, CQ(temp));
            }
         }
      }
   }

   /*** $IT_PARSE(cnum item_range). */
   else if (streq (what, "it_parse")  &&  NOT err)   it_parse (result, arg);

   /*** $IT_PACK (cnum item_range). */
   else if (streq (what, "it_pack" )  &&  NOT err)   it_pack  (result, arg);
  

   /*** $IT_EXISTS(cnum inum). */
   else if (streq (what, "it_exists")) {
/*    yes = (NOT err  &&  g_pstat->items >=  0   &&   */
      yes = (NOT err  &&
                  inum <= g_cstat->items  &&  g_cstat->resps[inum] >=  0);
      chxcatval (result, THIN, L(yes ? '1' : '0'));
   }

   /*** $IT_VISIB(cnum inum). */
   else if (streq (what, "it_visib")) {
      yes = (NOT err                       &&
             g_pstat->items       >=  0    &&    /* user allowed in conf */
             g_cstat->items       >= inum  &&    /* item # exists */
             g_cstat->resps[inum] >=  0    &&    /* item not deleted */
            (g_pstat->items < inum  ||           /* new item or not forgotten */
             g_pstat->resps[inum] >= -1 )  &&
            (g_hidden->items < inum  ||          /* not hidden */
             g_hidden->resps[inum] >= -1 ));
      chxcatval (result, THIN, L(yes ? '1' : '0'));
   }

   /*** $IT_HIDDEN(cnum inum). */
   else if (streq (what, "it_hidden")) {
      hidevalue = '0';
      if (NOT err                       &&
          g_cstat->items       >= inum  &&    /* item # exists */
          g_cstat->resps[inum] >=  0    &&    /* item not deleted */
          g_hidden->items >= inum ) {         /* in list of possible hidden*/
         if      ( g_hidden->resps[inum] == -2 )  hidevalue = '1';
         else if ( g_hidden->resps[inum] == -3 )  hidevalue = '2';
      }
      chxcatval (result, THIN, L(hidevalue));
   }

   /*** $SET_IT_HIDDEN(cnum inum hidden). */
   else if (streq (what, "set_it_hidden")) {
      if      (rnum == 1)  hidevalue = -2;
      else if (rnum == 2)  hidevalue = -3;
      else                 hidevalue = -1;
      if (mod_mark_pstat (g_hidden, (int) inum, hidevalue)) {
         a_chg_pstat (g_hidden, P_EVERY, A_WAIT);
      }
   }

   /*** $IT_FROZEN(cnum inum). */
   else if (streq (what, "it_frozen")) {
      if (NOT err) {
         resp->cnum = cnum;
         resp->inum = inum;
         resp->rnum = 0;
         a_get_resp (resp, P_FROZEN, A_WAIT);
      }
      chxcatval  (result, THIN, 
         L(NOT err  &&  resp->exists && resp->frozen ? '1' : '0'));
   }

   /*** $SET_IT_FROZEN(cnum inum flag). */
   else if (streq (what, "set_it_frozen")) {
      yes = 0;
      if (NOT err) {
         resp->cnum   = cnum;
         resp->inum   = inum;
         resp->rnum   = 0;
         resp->frozen = rnum;
         yes = (a_chg_resp (resp, P_FROZEN, A_WAIT) == A_OK);
      }
      chxcatval (result, THIN, L(yes ? '1' : '0'));
   }

   /*** $IT_NEW(cnum inum). */
   else if (streq (what, "it_new")) {
      yes = (inum > g_pstat->items);
      chxcatval (result, THIN, L(yes ? '1' : '0'));
   }

   /*** $IT_UNSEEN(cnum inum). */
   else if (streq (what, "it_unseen")) {
      yes = (NOT err &&  inum <= g_pstat->items  &&  g_pstat->resps[inum] < 0);
      chxcatval (result, THIN, L(yes ? '1' : '0'));
   }

   /*** $IT_RESPS(cnum inum). */
   else if (streq (what, "it_resps")) {
      sprintf (temp, "%d", 
         (err  ||  inum > g_cstat->items ? -1 : g_cstat->resps[inum]));
      chxcat  (result, CQ(temp));
   }

   /*** $IT_LASTRESP (cnum inum) */
   else if (streq (what, "it_lastresp")) {
      if (err  ||  inum > g_cstat->items  || 
                   g_cstat->resps[inum] < 0)    rlast = -1;
      else for (rlast = g_cstat->resps[inum];   rlast >= 0;   --rlast) {
         resp->cnum = cnum;
         resp->inum = inum;
         resp->rnum = rlast;

         status = a_get_resp (resp, 0, A_WAIT);
         if (status                      != A_OK)  continue;
         if (chxvalue (resp->date, L(0)) ==    0)  continue;
         break;
      }
      sprintf (temp, "%d", rlast);
      chxcat  (result, CQ(temp));
   }

   /*** $IT_NEWR(cnum inum).   # of 1st new response. */
   else if (streq (what, "it_newr")) {
      if      (err || inum > g_cstat->items) new = 0;   /* error */
      else if (inum > g_pstat->items)        new = 0;   /* new item */
      else if (g_cstat->resps[inum] < 0)     new = 0;   /* deleted item */
      else if (g_pstat->resps[inum] < 0)     new = 0;   /* unseen item */
      else if (g_pstat->resps[inum] < g_cstat->resps[inum])
                                             new = g_pstat->resps[inum] + 1;
      else                                   new = g_cstat->resps[inum] + 1;
      sprintf (temp, "%d", new);
      chxcat  (result, CQ(temp));
   }

   /*** $SET_IT_SEEN(cnum inum rnum).  Set rnum as "seen". */
   else if (streq (what, "set_it_seen")  &&  NOT err) {
      if (mod_mark_pstat (g_pstat, (int) inum, (int) rnum)) {
         a_chg_pstat (g_pstat, P_EVERY, A_WAIT);
      }
   }


   RETURN(1);
}
