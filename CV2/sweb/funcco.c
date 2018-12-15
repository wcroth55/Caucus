
/*** FUNC_CO.   Return the resulting value of a co_xxx() function.
/
/    func_co (result, what, arg, conf, protect);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/       struct sweb_conf_t *conf;    (server configuration info)
/       int                 protect; (protected mode on?)
/
/    Purpose:
/       Append value of co_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/       Need to modify this to allow org to delete own conference!
/
/    Home:  funcco.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/28/95 12:00 New function. */
/*: CR  3/11/01 13:45 Add handling for MGR_RMCOORG bit. */
/*: CR 10/09/02 Allow "@" in conference organizer userid. */
/*: CR 04/12/04 Add co_priv() function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern int       cml_debug;
extern AsciiBuf  sqlbuf;

FUNCTION  func_co (
   Chix   result, 
   char  *what, 
   Chix   arg,
   struct sweb_conf_t *conf,
   int    protect)
{
   static   Conf  cache = (Conf) NULL;
   static   Chix  name, word;
   int4     cnum, atsign, pos;
   int      success, code, token;
   char     temp[512], cname[256], query[512];
   Namelist groups, nlnode();
   ENTRY ("func_co", "");

   /*** Initialization. */
   if (cache == (Conf) NULL) {
      cache = a_mak_conf ("func_co cache");
      word  = chxalloc (L(40), THIN, "func_co word");
      name  = chxalloc (L(40), THIN, "func_co name");
   }

   /*** All (remaining) co_xxx() functions take a conference number as an
   /    argument.  Decode it, and check the Conf cache. */
   pos  = L(0);
   cnum = chxint4 (arg, &pos);
   if (pos == L(0)  ||  cnum <= 0) {
      cache->cnum = 0;
      BAD("NO CNUM");
      RETURN(1);
   }

   if (cache->cnum != cnum) {
      cache->cnum = cnum;
      code = a_get_conf (cache, P_EVERY, A_WAIT);
/*
      if (code != A_OK) {
         cache->cnum = 0;
         if (cml_debug) {
            sprintf (temp, "func_co: a_get_conf() failed, conf=%d, code=%d\n",
                     cnum, code);
            logger  (0, LOG_FILE, temp);
         }
         RETURN(1);
      }
*/
   }

   /*** $CO_ORG(). */
   if      (streq (what, "co_org")) {
      sprintf (query, 
         "SELECT userid FROM grouprules WHERE owner='CONF' AND name='%d' "
         "   AND access >= %d  ORDER BY access DESC LIMIT 1", cnum, privValue("organizer"));
      sql_quick (0, query, sqlbuf, 1);
      strtoken (temp, 1, sqlbuf->str);
      chxcat   (result, CQ(temp));
   }

   /*** $CO_INTRO(). */
   else if (streq (what, "co_intro"))  chxcat (result, cache->intro->data);

   /*** $CO_GREET(). */
   else if (streq (what, "co_greet"))  chxcat (result, cache->greet->data);

   /*** $CO_ADD(). */
   else if (streq (what, "co_add"))
      chxcatval (result, THIN, L(cache->add ? '1' : '0'));

   /*** $SET_CO_ADD(). */
   else if (streq (what, "set_co_add")) {
      cache->add = chxint4 (arg, &pos);
      success = (a_chg_conf (cache, P_FADD, A_WAIT) == A_OK);
      chxcatval (result, THIN, L(success ? '1' : '0'));
   }

   /*** $CO_CHANGE(). */
   else if (streq (what, "co_change"))
      chxcatval (result, THIN, L(cache->change ? '1' : '0'));

   /*** $SET_CO_CHANGE(). */
   else if (streq (what, "set_co_change")) {
      cache->change = chxint4 (arg, &pos);
      success = (a_chg_conf (cache, P_FCHANGE, A_WAIT) == A_OK);
      chxcatval (result, THIN, L(success ? '1' : '0'));
   }

   /*** $CO_USERLIST(). */
   else if (streq (what, "co_userlist")) chxcat (result, cache->userlist->data);


   /*** $CO_REMOVE(). */
   else if (streq (what, "co_remove")) {
      success = 0;
      if (NOT protect) {
         cl_name (cname, 256, cnum);
         sprintf (temp, "cname='%s', cnum=%d", cname, cnum);
         logger (1, LOG_FILE, temp);

         /*** Make sure this user has either the manager priv for deleting 
         /    any conf, or is organizer of conf. */
         sysuserid (name);
         logger (1, LOG_FILE, ascquick(name));
         sprintf (temp, "mgr_in_group=%d",
                 mgr_in_group ("conferenceDelete conferenceAllPrivileges "
                           "systemAllPrivileges"));
         logger (1, LOG_FILE, temp);
         if (mgr_in_group ("conferenceDelete conferenceAllPrivileges "
                           "systemAllPrivileges")  ||
                 allowed_on(name, groups, cnum) >= privValue("organizer")) {
            success = rmconf (cnum);
            sprintf (temp, "%d=rmconf(%d)", success, cnum);
            logger  (1, LOG_FILE, temp);
         }
      }
      chxcatval (result, THIN, L(success ? '1' : '0'));

      /*** If conference was removed, also remove its upload library. */
      if (success) {
         strlower  (cname);
         sprintf   (temp, "%s/%s", conf->caucuslib, cname);
         sysrmdir  (temp);
      }
   }

   /*** $CO_RENAME(cnum newname) */
   else if (streq (what, "co_rename")) {
      success = 0;

      if (NOT protect) {
         /*** Make sure this user has the manager bit for either 
         /    creating or deleting confs. */
         sysuserid (name);
         if (mgr_in_group ("conferenceDelete conferenceCreate "
                           "conferenceAllPrivileges systemAllPrivileges")) {
            chxtoken   (word, nullchix, 2, arg);
            if (NOT okconf (word))  success = -1;
            else {
               success = mv_conf (cnum, word, conf->caucuslib); 
               if (success <= 0)  success = success - 2;
            }
         }
      }
      sprintf (temp, "%d", success);
      chxcat  (result, CQ(temp));
   }

   /*** $CO_MAKEORG() */
   else if (streq (what, "co_makeorg")) {
      /*** Make sure this user has the manager bit for organizing confs. */
      success = 0;
      if (NOT protect) {
         sysuserid (name);
         if (mgr_in_group ("conferenceChange conferenceAllPrivileges "
                           "systemAllPrivileges")) {
            chxcat (cache->userlist->data, CQ("\n:organizer\n"));
            chxcat (cache->userlist->data, name);
            success = (a_chg_conf (cache, P_USERLIST | P_OVERRIDE, 
                                   A_WAIT) == A_OK); 
   
            if (success) {
                 /* Give them org access immediately! */
            }
         }
      }
      chxcatval (result, THIN, L(success ? '1' : '0'));
   }

   RETURN(1);
}
