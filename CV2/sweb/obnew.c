
/*** OB_NEW.  Create new object.
/
/    ob_new (result, arg, otab, cfile);
/ 
/    Parameters:
/       Chix          result;  (put resulting value here)
/       Chix          arg;     (function argument)
/       Obtab         otab;    (object table)
/       CML_File      cfile;   (original cml file)
/
/    Purpose:
/       Implement CML function $ob_new (class persist [name]).
/       Create a new object, add it to the object table.
/
/    How it works:
/       Puts the reference to the new object in RESULT, adds the
/       new object to the end of OTAB.
/
/    Returns: 1 on success
/             0 on failure (also sets RESULT to error object)
/
/    Known bugs:
/       BUG: What to do about checking for pre-existence of a persistent
/       object?
/
/    Home:  sweb/obnew.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/29/98 12:58 New function. */
/*: CR  4/04/00 12:48 Make temp[] big enough for any ascquick(). */

#include <stdio.h>
#include <ctype.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  ZERO(x)   (!(x[0]))
#define  FAIL      goto failed

FUNCTION  ob_new (Chix result, Chix arg, Obtab otab, CML_File cfile)
{
   static Chix word = nullchix;
   char        temp[2000], oname[100];
   char       *op;
   int         slot, new;
   int4        persist;
   Object      obj, make_obj();

   ENTRY ("ob_new", "");

   if (word == nullchix) word = chxalloc (L(80), THIN, "ob_new word");

   /*** Get object name; make sure it is legal, and does not already exist. */
   chxtoken (word, nullchix, 3, arg);
   if (chxvalue (word, L(0)) == 0177)  ascofchx (oname, word, 1, 100);
   else                                ascofchx (oname, word, 0, 100);
   for (op=oname;   *op;   ++op) {
      if (NOT isalnum(*op)  &&  *op!='_'  &&  *op!='-')  {
         sprintf (temp, "\n%s:%d Bad object name '%s'\n", 
                  cfile->filename, cfile->lnums[cfile->cdex], oname);
         FAIL;
      }
   }
   if (NOT ZERO(oname)  &&  obtab_is (otab, oname) >= 0) {
      sprintf (temp, "\n%s:%d Can't create existing object '%s'\n", 
               cfile->filename, cfile->lnums[cfile->cdex], oname);
      FAIL;
   }

   /*** Find the class for the new object. */
   chxtoken (word, nullchix, 1, arg);
   slot = vartab_is (otab->class, word);
   if (slot < 0) {
      sprintf (temp, "\n%s:%d No class '%s' for object '%s'\n", cfile->filename,
               cfile->lnums[cfile->cdex], ascquick(word), oname);
      FAIL;
   }

   /*** Persistent? */
   chxtoken (word, nullchix, 2, arg);
   persist = 0;
   if (NOT chxnum (word, &persist)) {
      sprintf (temp, "\n%s:%d Bad persistence value in $ob_new()\n", 
               cfile->filename, cfile->lnums[cfile->cdex]);
      chxcat  (cfile->errtext, CQ(temp));
   }

   /*** Persistent objects must have names. */
   if (persist  &&  ZERO(oname)) {
      sprintf (temp, "\n%s:%d $ob_new(%s) must have object name\n", 
               cfile->filename, cfile->lnums[cfile->cdex], ascquick(arg));
      FAIL;
   }

   /*** Create the object. */
   obj = make_obj (oname, ascquick(otab->class->value[slot]), persist);
   if (obj == NULL) {
      sprintf (temp, "\n%s:%d Out of memory creating object '%s'\n", 
               cfile->filename, cfile->lnums[cfile->cdex], oname);
      FAIL;
   }
   ascofchx (obj->class, otab->class->name[slot], L(0), L(100));

   /*** Put the object in a slot in the object table.  If the table
   /    is nearly full, attempt to expand it. */
   new = otab->used++;
   if (new > otab->max - 5  &&  NOT obtab_exp (otab))  {
      sprintf (temp, "\n%s:%d Out of memory indexing object '%s'\n", 
               cfile->filename, cfile->lnums[cfile->cdex], oname);
      FAIL;
   }
   otab->obj[new] = obj;
   if (persist)  store_obj (obj);
   chxofascii (result, obj->name);
   RETURN (1);

failed:
   chxcat  (cfile->errtext, CQ(temp));
   chxofascii (result, otab->err);
   RETURN (0);
}

