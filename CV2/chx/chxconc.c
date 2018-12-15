
/*** CHXCONC.  Concatenate two strings together to form a third string.
/
/    chxconc (a, b, c);
/
/    Parameters:
/       Chix  a, b, c;
/
/    Purpose:
/       Set chix A to the concatenation of B with C.
/       B or C (or both) may be A, i.e. chxconc (a, b, a) is
/       perfectly valid.
/
/    How it works:
/       The type of A becomes the largest of the type of B or C.
/
/    Returns: 1
/
/    Error Conditions:  if A, B, or C is not a chix, nothing
/       happens (and chxerror() is called).
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies:
/       uses sysfree().
/
/    Known bugs:
/
/    Home:  chx/chxconc.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR  7/28/91 15:48 chxalloc() first argument is long. */
/*: OQ 02/09/98 replace chxcheck call to by inline commands*/

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxconc (a, b, c)
   Chix   a, b, c;
{
   Chix   z;

   ENTRY ("chxconc", "");

   /*if (NOT chxcheck (a, "chxconc")  ||
       NOT chxcheck (b, "chxconc")  ||
       NOT chxcheck (c, "chxconc"))  RETURN(1);*/

   if (a == nullchix || b == nullchix  || c == nullchix )
      { chxerror (CXE_NULL, "chxconc" , "");        RETURN(1); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxconc" , a->name);  RETURN(1); }
   if (FREED(b))    
      { chxerror (CXE_FREED, "chxconc" , b->name);  RETURN(1); }
   if (FREED(c))    
      { chxerror (CXE_FREED, "chxconc" , c->name);  RETURN(1); }
   if ( (NOT ISCHIX(a)) || (NOT ISCHIX(b))  || (NOT ISCHIX(c)) )
      { chxerror (CXE_NOTCHX, "chxconc" , "");      RETURN(1); } 
   
   z = chxalloc ((int4) b->maxlen, b->type, "chxconc z");
   chxcpy (z, b);
   chxcat (z, c);

   sysfree ( (char *) a->p);
   *a = *z;
   sysfree ( (char *) z);

   RETURN (1);
}
