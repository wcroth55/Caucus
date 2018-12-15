
/*** FUNC_MGR.   Return the resulting value of a mgr_xxx() function.
/
/    func_mgr (result, what, arg, protect);
/   
/    Parameters:
/       Chix    result;  (append resulting value here)
/       char   *what;    (function name)
/       Chix    arg;     (function argument)
/       int     protect; (protected mode on?)
/
/    Purpose:
/       Append value of mgr_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/
/    Returns: mask value as an integer
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funcmgr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/03/97 17:51 New function. */
/*: CR  5/18/01 13:30 Add processing of '<file' include files in mgr_bits(). */
/*: CR 10/16/01 13:50 Add groupmat (permnames, searchall). */
/*: CR 08/24/05 Add C5 mgr_groups(). */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"
#include "sqlcaucus.h"

extern int       cml_debug;
extern AsciiBuf  sqlbuf;

FUNCTION  int4 func_mgr (Chix result, char *what, Chix arg, int protect)
{
   static Chix    mgrCache = nullchix;
   static Chix    mgrId, word1, word2;
   static int     mgrTime  = 0;
   char           temp[400];
   int            rc, which;
   int4           pos;
#if CAUCUS4
   static Vartab  cache = (Vartab) NULL;
   static Chix    primgr;
   int            mp;
   int4           mask, this;
   int4           mgr_bits();
#endif
   ENTRY ("func_mgr", "");

   /*** Initialization. */
   if (mgrCache == nullchix) {
      mgrCache = chxalloc (L(80), THIN, "func_mgr mgrCache");
      mgrId    = chxalloc (L(40), THIN, "func_mgr mgrId");
      word1    = chxalloc (L(40), THIN, "func_mgr word1");
      word2    = chxalloc (L(40), THIN, "func_mgr word2");
#if CAUCUS4
      cache = a_mak_vartab ("func_mgr cache");
      cache->id1 = -1;
      a_get_vartab (cache, P_EVERY, A_WAIT);
      primgr = chxalloc (L(40), THIN, "func_mgr primgr");
      if (cache->name[0] != nullchix)  chxcpy (primgr, cache->name[0]);
      else                          chxofascii (primgr, "caucus");
#endif
   }

   chxtoken (word1, nullchix, 1, arg);

#if CAUCUS4
   if (streq (what, "mgr_list")) {
      if (NOT protect) {
         /*** $mgr_list()   (no args) */
         if (chxvalue (word1, L(0)) == 0) {
            for (mp=0;   mp<cache->used;   ++mp) {
               if (chxvalue (cache->name[mp], L(0)) == 0)  continue;
               chxcat  (result, cache->name[mp]);
               chxnum  (cache->value[mp], &this);
               this = this | MGR_IS;
               sprintf (temp, " %d ", this);
               chxcat  (result, CQ(temp));
            }
         }
   
         /*** $mgr_list(userid) */
         else {
            mask = mgr_bits (word1, cache, word2);
            chxcat    (result, word1);
            chxcatval (result, THIN, L(' '));
            sprintf   (temp, "%d ", mask);
            chxcat    (result, CQ(temp));
         }
      }
      RETURN (mask);
   }
#endif

   /*** Update mgrCache (of mgr groups userid belongs to) as needed. */
   if (streq (what, "mgr_groups")  ||  streq (what, "mgr_in_group")) {
      if (time(NULL) > mgrTime + 60  ||  NOT chxeq (word1, mgrId)) {
         chxcpy   (mgrId, word1);
         chxclear (mgrCache);
         sprintf  (temp, 
            "SELECT name, access FROM groups "
            " WHERE owner='MGR' AND userid='%s' AND active > 0"
            "   AND access > 0", ascquick(mgrId));
         rc = sql_exec (0, temp);
         if (rc == SQL_SUCCESS) {
            while ( (rc = sql_fetch(0)) != SQL_NO_DATA) {
               while ( (rc = sql_getchardata (0, sqlbuf)) != SQL_NO_DATA) {
                  chxcat    (mgrCache, CQ(sqlbuf->str));
                  chxcatval (mgrCache, THIN, L(' '));
               }
            }
         }
         mgrTime = time(NULL);
      }
   }

   if (streq (what, "mgr_groups")) {
      chxcat (result, mgrCache);
   }

   else if (streq (what, "mgr_in_group")) {
      pos = 0;
      chxnextword (word2, arg, &pos);
      while (chxnextword (word2, arg, &pos) > 0) {
         if ( (which = tablefind (mgrCache, word2)) >= 0) {
            chxtoken (word2, nullchix, which+2, mgrCache);
            chxcat   (result, word2);
            pos = 0;
            RETURN   (chxint4 (word2, &pos));
         }
      }
      chxcatval (result, THIN, L('0'));
   }

   RETURN (0);
}

/*** C-level equivalent of calling $mgr_in_group ($userid() groups...) */
FUNCTION int mgr_in_group (char *groups) {
   static Chix args = nullchix;
   static Chix ignore;
   int         access;

   if (args == nullchix) {
      args   = chxalloc (L(80), THIN, "mgr_in_group args");
      ignore = chxalloc (L(10), THIN, "mgr_in_group ignore");
   }

   sysuserid (args);
   chxcatval (args, THIN, L(' '));
   chxcat    (args, CQ(groups));
   access = func_mgr (ignore, "mgr_in_group", args, 0);
   return (access);
}
   

/*** Return mask of permission bits for userid 'who'. */
FUNCTION int4 mgr_bits (Chix who, Vartab list, Chix temp)
{
   static Chix wildcard = nullchix;
   static Chix incfile;
   int4  mask, last, this;
   int   mp, match, c1;
   Namelist groups, nlnode();
   ENTRY ("mgr_bits", "");

   if (wildcard == nullchix) {
      wildcard = chxalsub (CQ("*"), 0, 1);
      incfile  = chxalloc (L(40), THIN, "mgr_bits incfile");
   }

   for (mask=0, mp=0;   mp<list->used;   ++mp) {
      match = 0;
      last  = chxlen(list->name[mp]) - 1;
      c1    = chxvalue (list->name[mp], L(0));

      /*** Wildcard match for everyone (!) */
      if      (c1 == '*')   match = 1;

      /*** Trailing wildcard match for a bunch of people ('roth*') */
      else if (chxvalue (list->name[mp], last) == '*') {
         chxclear   (temp);
         chxcatsub  (temp, list->name[mp], L(0), last);
         match = chxindex (who, L(0), temp) == 0;
      }

      /*** Everyone in a groups file. */
      else if (c1 == '<') {
         groups = nlnode(1);
         chxclear  (incfile);
         chxcatsub (incfile, list->name[mp], 1, ALLCHARS);
         chxclear  (temp);
         if (groupmatch (incfile, groups, who, wildcard, temp, 0))  match = 1;
         nlfree (groups);
      }

      /*** An exact match against a userid. */
      else match = chxeq (list->name[mp], who);

      if (match) {
         chxnum (list->value[mp], &this);
         mask = mask | this | MGR_IS;
      }
   }

   RETURN (mask);
}
