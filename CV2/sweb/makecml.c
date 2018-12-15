
/*** MAKE_CML.   Make a new CML_File object. 
/
/    cf = make_cml(errtext);
/   
/    Parameters:
/       Chix  errtext;    (reference to chix containing accumulated err text)
/
/    Purpose:
/       Make and initialize a new CML_File object.
/
/    How it works:
/
/    Returns:
/
/    Home:  sweb/makecml.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/22/98 23:08 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  CML_File make_cml (Chix errtext)
{
   CML_File  new;
   int       num;
   char     *sysmem();

   ENTRY ("make_cml", "");

   new = (CML_File) sysmem (sizeof (struct cmlfile_t), "cmlfile_t");
   
   for (num=  0;   num<200;      ++num)   
      new->lines[num] = chxalloc (L(200), THIN, "make_cml lines[]");

   for (num=200;   num<MAXCML;   ++num) 
      new->lines[num] = (Chix) NULL;

   new->size    = 0;
   new->errtext = errtext;

   RETURN (new);
}
