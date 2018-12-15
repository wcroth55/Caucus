
/*** FUNC_OB.    Handle the object functions "ob_xxx".
/
/    func_ob (result, what, arg, conf, cfile, otab);
/ 
/    Parameters:
/       Chix     result;  (put resulting value here)
/       char    *what;    (CML function name);
/       Chix     arg;     (function argument)
/       SWconf   conf;    (server configuration info)
/       CML_File cfile;   
/       Obtab    otab;    (table of known objects)
/
/       char         *cmlargs; (CML script arguments)
/       Vartab        vars;    (CML variable list)
/       Vartab        form;    (form data variable list)
/       int           protect; (only allow "safe" functions)
/       Chix          incvars; (text of 'include' variables)
/       Vartab        macs;    (defined macros)
/       CML_File      cfile;   (original cml file)
/       Obtab         otab;    (object table)
/
/    Purpose:
/       Interpret CML function object $ ob_xxx() WHAT, put
/       result in RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sweb/funcob.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/98 15:46 New function. */

#include <stdio.h>
#include <time.h>
#include "chixuse.h"
#include "sweb.h"
#include "unitcode.h"

#define   ZERO(x)  (!(x[0]))

FUNCTION  func_ob (Chix result, char *what, Chix arg, SWconf conf, 
                   CML_File cfile, Obtab otab)
{
   Chix   word1, word2;
   char  *mode, *tp;
   char  *strtoken();
   char   oname[100], nname[100], temp[2000], memname[100];
   long   tstart;
   int4   val1, pos, pos2, len, slot, slot2, op1;
   int    mnum, success, lkcode, mnew, mold;
   Object obj, new, new2, make_obj(), load_obj();
   ENTRY ("func_ob", "");

   word1   = chxalloc ( L(100), THIN, "func_ob word1");
   word2   = chxalloc ( L(100), THIN, "func_ob word2");

   if (streq (what, "ob_class")) {
      chxtoken   (word1, nullchix, 1, arg);
      chxtoken   (nullchix, word2, 2, arg);
      vartab_add (otab->class, word1, word2);
   }

   else if (streq (what, "ob_named")) {
      chxcatval (result, THIN, 0177);
      chxtoken  (word1,  nullchix, 1, arg);
      chxcat    (result, word1);
   }

   else if (streq (what, "ob_new"))  ob_new (result, arg, otab, cfile);

   else if (streq (what, "ob_isobj")) {
      val1 = '0';
      chxtoken (word1, nullchix, 1, arg);
      if (chxtoken (word2, nullchix, 2, arg) < 0  &&
          chxvalue (word1, L(0)) == 0177) {
         val1 = '1';
         ascofchx (temp, word1, L(1), L(100));
         for (tp=temp;   *tp;   ++tp) {
            if (NOT isalnum(*tp)  &&  *tp!='-'  &&  *tp!='_')
               { val1 = '0';   break; }
         }
      }
      chxcatval (result, THIN, val1);
   }

   else if (streq (what, "ob_iserr")) {
      chxtoken  (word1, nullchix, 1, arg);
      chxcatval (result, THIN, 
                 streq (ascquick(word1), "\177ERR") ? L('1'): L('0') );
   }

   else if (streq (what, "ob_exists")) {
      chxtoken  (word1, nullchix, 1, arg);
      ascofchx  (oname,  word1, L(0), L(100));
      success = (oname[0]==0177  &&  obtab_is (otab, oname) >= 0);
      chxcatval (result, THIN, success ? L('1') : L('0') );
   }

   /*** $ob_copy (new old) */
   else if (streq (what, "ob_copy")) {

      chxtoken (word1, nullchix, 1, arg);
      ascofchx (nname, word1, L(0), L(100));
      chxtoken (word2, nullchix, 2, arg);
      ascofchx (oname, word2, L(0), L(100));
      val1 = '0';

      /*** If both objects exist... */
      if ( (slot  = obtab_is (otab, nname)) >= 0  &&
           (slot2 = obtab_is (otab, oname)) >= 0) {
         new = otab->obj[slot];
         obj = otab->obj[slot2];

         /*** If NEW is persistent and not $ob_wait'd, and has some
         /    members that will not be copied over, reload it from disk. */
         if (new->persists  &&  NOT new->wait  &&
             NOT streq (new->memnames, obj->memnames)) {
            if ( (new2 = load_obj (new->name, otab->class)) != NULL) {
               new2->lockfd   = new->lockfd;
               new2->persists = new->persists;
               free_obj (new);
               new = otab->obj[slot] = new2;
            }
         }

         /*** Copy over all the common members. */
         for (mold=0;   mold < obj->members;   ++mold) {
            if (strtoken (memname, mold+1, obj->memnames) == NULL)  break;

            if ( (mnew = strtable (new->memnames, memname)) < 0)    continue;
            chxcpy (new->value[mnew], obj->value[mold]);
            if (new->wait)  new->taint[mnew] = 1;
         }
         new->updated = time(NULL);
         if (new->persists  &&  NOT new->wait)  store_obj (new);
         val1 = '1';
      }
      chxcatval (result, THIN, val1);
   }

   else if (streq (what, "ob_wait")) {
      pos = 0;
      while (chxnextword (word1, arg, &pos) >= 0) {
         ascofchx (temp, word1, L(0), L(100));
         if ( (slot = obtab_is (otab, temp)) >= 0)
            otab->obj[slot]->wait = 1;
      }
   }

   else if (streq (what, "ob_commit")) {

      /*** For each object named in $ob_commit(...) that exists and
      /    was set to "wait" by $ob_wait()... */
      for (pos=0;   chxnextword (word1, arg, &pos) >= 0;  ) {
         ascofchx (oname, word1, L(0), L(100));
         if ( (slot = obtab_is (otab, oname)) >= 0  &&
               otab->obj[slot]->wait) {

            /*** Load a new copy of the object. */
            if ( (new = load_obj (oname, otab->class)) == NULL) {
               sprintf (temp, "\n%s:%d No object '%s'\n",
                        cfile->filename, cfile->lnums[cfile->cdex], oname+1);
               chxcat  (cfile->errtext, CQ(temp));
            }

            /*** Copy the 'tainted' values to the new object, and
            /    store *it*. */
            else {
               obj = otab->obj[slot];
               for (mnum=0;   mnum < obj->members;   ++mnum) {
                  if (obj->taint[mnum]) {
                     chxcpy (new->value[mnum], obj->value[mnum]);
                  }
               }
               new->updated = time(NULL);
               new->lockfd  = obj->lockfd;
               store_obj (new);
               free_obj  (otab->obj[slot]);
               otab->obj[slot] = new;
            }
         }
      }
   }

   else if (streq (what, "ob_reload")) {

      /*** For each object named in $ob_reload()... */
      for (pos=0;   chxnextword (word1, arg, &pos) >= 0;  ) {
         ascofchx (oname, word1, L(0), L(100));
         len = otab->used;

         /*** If we didn't just load it now, reload it and throw
         /    away the old version. */
         if ( (slot = obtab_is (otab, oname)) >= 0  &&
              otab->used == len) {
            if ( (obj = load_obj (oname, otab->class)) != NULL) {
               obj->lockfd = otab->obj[slot]->lockfd;
               free_obj (otab->obj[slot]);
               otab->obj[slot] = obj;
            }
         }
      }
   }

   else if (streq (what, "ob_lock")) {
      /*** Get timeout value OP1, object name TEMP. */
      chxtoken (word2, nullchix, 2, arg);
      pos = 0;       op1 = chxint4 (word2, &pos);
      if (op1 <= 0)  op1 = 60;
      chxtoken (word1, nullchix, 1, arg);
      ascofchx (oname, word1, L(0), L(100));

      success = 0;
      if ( (slot = obtab_is (otab, oname)) >= 0) {
         for (tstart = time(NULL);   time(NULL) - tstart < op1; ) {
            lkcode = sysoblock (otab->obj[slot], 1, 1, conf->caucus_path);
            if (lkcode ==  1)     { success = 1;   break; }
            if (lkcode == -1) { 
               sprintf (temp, "\n%s:%d $ob_lock(%s), system error\n",
                        cfile->filename, cfile->lnums[cfile->cdex], oname+1);
               chxcat  (cfile->errtext, CQ(temp));
               break;
            }
            syssleep (2);
         }
      }
      else {
         sprintf (temp, "\n%s:%d $ob_lock(%s), no such object\n",
                        cfile->filename, cfile->lnums[cfile->cdex], oname+1);
         chxcat  (cfile->errtext, CQ(temp));
      }
      chxcatval (result, THIN, (success ? L('1') : L('0')));
   }

   else if (streq (what, "ob_unlock")) {
      chxtoken (word1, nullchix, 1, arg);
      ascofchx (oname, word1, L(0), L(100));

      success = 0;
      if ( (slot = obtab_is (otab, oname)) >= 0)
         sysoblock (otab->obj[slot], 0, 1, conf->caucus_path);
   }

   else if (streq (what, "ob_delete")) {
      pos = 0;
      while (chxnextword (word1, arg, &pos) >= 0) {
         ascofchx (temp, word1, L(0), L(100));
         if ( (slot = obtab_is (otab, temp)) >= 0) {
            obj = otab->obj[slot];
            if (obj->lockfd)    sysoblock (obj, 0, 1, conf->caucus_path);
            if (obj->persists)  kill_obj  (obj->name);
            free_obj (obj);
            otab->obj[slot] = otab->obj[otab->used-1];
            otab->obj[otab->used-1] = NULL;
            otab->used--;
         }
      }
   }

   chxfree (word1);
   chxfree (word2);

   return  (1);
}
