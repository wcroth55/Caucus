
/*** OB_VALUE.   Return the value of a full object.member reference.
/
/    ok = ob_value (result, name, otab, cfile);
/ 
/    Parameters:
/       int        ok;      (1=>found, 0=>not found)
/       Chix       result;  (append resulting value here)
/       Chix       name;    (name of object)
/       Obtab      otab;    (object table)
/       CML_File   cfile;   (original cml file)
/
/    Purpose:
/       Given a full object.member reference (including forms
/       like object1.object2.member), append the value of the
/       member to RESULT.
/
/    How it works:
/       If an object name (with no ".members" is supplied),
/       just return the object name.
/
/       Otherwise, step down the references until the final member
/       is found, checking for validity all the way.
/
/    Returns: 1 on success, appends value to RESULT.
/             0 on failure (object or member not found, or any
/                  reference to the err object), does nothing to RESULT.
/
/    Known bugs:
/
/    Home:  sweb/obvalue.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/30/98 16:17 New function. */
/*: CR  4/04/00 12:48 Make temp[] big enough for any ascquick(). */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  FAIL      goto failed

FUNCTION  ob_value (Chix result, Chix name, Obtab otab, CML_File cfile)
{
   char   fullref[400], oname[100], mname[100], mvalue[100], temp[2000];
   char  *sp, *strtoken();
   int    level, mnum, slot;
   Object this;

   ENTRY ("ob_value", "");

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
   if (strtoken (mvalue, 1, fullref) == NULL)        RETURN (0);
   if (streq    (mvalue, otab->err))                 RETURN (0);

   /*** If there's just an object, with no ".member", return the
   /    object name. */
   if (strtoken (mname,  2, fullref) == NULL) {
      chxcat (result, name);
      RETURN (1);
   }

   /*** Parse down the object reference (e.g., object.x.y.z), dereferencing
   /    each member into an object as we go.  The bottom member can be
   /    anything, i.e. an object or just plain text. */
   for (level=2;   (1);   ++level) {

      /*** If we've reached the "bottom" of the reference (e.g.
      /    "this.member" in "that.this.member"), return the value
      /    of the current "member". */
      if (strtoken (mname, level, fullref) == NULL)  {
         chxcat (result, this->value[mnum]);
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
      this = otab->obj[slot];
      mnum = strtable (this->memnames, mname);

      /*** If this is a regular member, get its value and go 'round
      /    the loop again. */
      if (mnum >= 0)   ascofchx (mvalue, this->value[mnum], L(0), L(100));

      /*** Otherwise, if this is the "bottom" member of the reference,
      /    and it's one of the standard object properties, evaluate the
      /    property and return. */
      else if (strtoken (temp, level+1, fullref) == NULL  &&
              (mnum = strtable ("CLASS MEMBERS PERSISTS", mname)) >= 0) {
         if      (mnum == 0)  chxcat    (result, CQ(this->class)); 
         else if (mnum == 1)  chxcat    (result, CQ(this->memnames));
         else if (mnum == 2)  chxcatval (result, THIN, L('0') + 
                                                 (this->persists ? 1 : 0));
         RETURN (1);
      }

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
