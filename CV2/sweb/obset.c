
/*** OB_SET.   Set the value of a full object.member reference.
/
/    ok = ob_set (name, value, otab, cfile);
/ 
/    Parameters:
/       int        ok;      (1=>object/member exists, 0=>does not exist)
/       Chix       name;    (name of object/member)
/       Chix       value;   (set to this value)
/       Obtab      otab;    (object table)
/       CML_File   cfile;   (original cml file)
/
/    Purpose:
/       Given a full object.member reference (including forms
/       like object1.object2.member), set it to VALUE.
/
/    How it works:
/
/    Returns: 1 on success
/             0 on failure (object or member not found, or any
/                  reference to the err object)
/
/    Known bugs:
/
/    Home:  sweb/obset.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/02/98 12:53 New function. */
/*: CR  4/04/00 12:48 Make temp[] big enough for any ascquick(). */

#include <stdio.h>
#include <time.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  FAIL      goto failed

FUNCTION  ob_set (Chix name, Chix value, Obtab otab, CML_File cfile)
{
   char   fullref[400], oname[100], mname[100], mvalue[100], temp[2000];
   char  *sp, *strtoken();
   int    level, mnum, slot;
   Object new, old, load_obj();

   ENTRY ("ob_set", "");
             
   /*** Get plain ascii copy of the object reference, replace dots w spaces. */
   ascofchx (fullref, name, L(0), L(500));
   if (fullref[0] == '.') {
      sprintf (temp, "\n%s:%d No object name in '%s'\n",
               cfile->filename, cfile->lnums[cfile->cdex], fullref);
      FAIL;
   }
   for (sp=fullref;   *sp;   ++sp)  if (*sp == '.')  *sp = ' ';

   /*** Start the loop by putting the object name in MVALUE (which immediately
   /    gets moved to ONAME inside the loop).  Any reference to the
   /    error object returns immediately. */
   if (strtoken (mname,  2, fullref) == NULL)        RETURN (0);
   if (strtoken (mvalue, 1, fullref) == NULL)        RETURN (0);
   if (streq    (mvalue, otab->err))                 RETURN (0);

   /*** Parse down the object reference (e.g., object.x.y.z), dereferencing
   /    each member into an object as we go.  The bottom member can be
   /    anything, i.e. an object or just plain text. */
   for (level=2;   (1);   ++level) {

      /*** If we've reached the "bottom" of the reference (e.g.
      /    "this.member" in "that.this.member"), set the value
      /    of the current "member". */
      if (strtoken (mname, level, fullref) == NULL)  {
         /*** Persistent object. */
         if (otab->obj[slot]->persists  &&  NOT otab->obj[slot]->wait) {
            if ( (new = load_obj (oname, otab->class)) == NULL) {
               sprintf (temp, "\n%s:%d No object '%s'\n",
                        cfile->filename, cfile->lnums[cfile->cdex], oname);
               FAIL;
            }
            chxcpy (new->value[mnum], value);
            new->updated = time(NULL);
            new->lockfd  = otab->obj[slot]->lockfd;
            store_obj (new);
            free_obj  (otab->obj[slot]);
            otab->obj[slot] = new;
         }
         else if (otab->obj[slot]->persists) {
            chxcpy (otab->obj[slot]->value[mnum], value);
            otab->obj[slot]->taint[mnum] = 1;
         }
         else {  /* Temporary object. */
            chxcpy (otab->obj[slot]->value[mnum], value);
            otab->obj[slot]->updated = time(NULL);
         }
         RETURN (1);
      }

      /*** OK, we're not at the bottom, the previous member value
      /    becomes the current object. */
      strcpy (oname, mvalue);
      if (streq (oname, otab->err))         RETURN (0);
      slot = obtab_is (otab, oname);
      if (slot < 0) {
         sprintf (temp, "\n%s:%d No object '%s'\n",
                  cfile->filename, cfile->lnums[cfile->cdex], oname);
         FAIL;
      }

      /*** Find the member of this object... */
      mnum = strtable (otab->obj[slot]->memnames, mname);

      /*** If this is a regular member, get its value and go 'round
      /    the loop again. */
      if (mnum >= 0)
         ascofchx (mvalue, otab->obj[slot]->value[mnum], L(0), L(100));

      /*** Otherwise, this is an erroneous object.member reference. */
      else {
         sprintf (temp, "\n%s:%d No member '%s' in object '%s'\n",
                  cfile->filename, cfile->lnums[cfile->cdex], mname, 
                  ascquick(name));
         FAIL;
      }
   }

failed:
   chxcat (cfile->errtext, CQ(temp));
   RETURN (0);
}
