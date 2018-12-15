/*** COPYREG.   Duplicate a user's registration structure
/
/    ok = copyreg (dest, source);
/
/    Parameters:
/       int     ok;           (Success?)
/       Userreg dest,         (user registration info struct)
/               source;       (source of user reg info)
/
/    Purpose:
/       Copy the contents of one Userreg into another.
/
/    How it works:
/       Caller must have allocated both structures with make_userreg().
/
/    Returns: 1 on success
/             0 on any error condition
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/copyreg.c
/  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JP  5/06/93 13:22 Create this. */
/*: CN  5/15/93 17:36 Fix nlcopy calls (has 1 arg, returns result). */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  copyreg  (dest, source)
   Userreg dest, source;
{
   Namelist nlcopy();
   int      success=0;

   ENTRY ("copyreg", "");

   /*** First copy over data actually stored contiguously in the struct. */
   dest->truncate = source->truncate;
   dest->tablength = source->tablength;
   dest->newpage = source->newpage;
   dest->verbose = source->verbose;
   dest->dict = source->dict;
   dest->expwarn = source->expwarn;
   dest->texten = source->texten;
   dest->receiptsize = source->receiptsize;
   dest->mytext = source->mytext;
   dest->read_only = source->read_only;
   dest->import_mail = source->import_mail;
   dest->export_mail = source->export_mail;
   dest->is_organizer = source->is_organizer;
   dest->printcode = source->printcode;
   dest->debug = source->debug;
   dest->lines = source->lines;
   dest->width = source->width;
   dest->sysi1 = source->sysi1;

   /* Then the Chix. */
   chxcpy (dest->name, source->name);
   chxcpy (dest->lastname, source->lastname);
   chxcpy (dest->id, source->id);
   chxcpy (dest->setopts, source->setopts);
   chxcpy (dest->sysopts, source->sysopts);
   chxcpy (dest->phone, source->phone);
   chxcpy (dest->laston, source->laston);
   chxcpy (dest->editor, source->editor);
   chxcpy (dest->eot, source->eot);
   chxcpy (dest->startmenu, source->startmenu);

   /* Finally the two namelists. */
   nlfree (dest->briefs);   dest->briefs = nlcopy (source->briefs);
   nlfree (dest->print);    dest->print  = nlcopy (source->print);

   SUCCEED;

 done:
   RETURN (success);
}
