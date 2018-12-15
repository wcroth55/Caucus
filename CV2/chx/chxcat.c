
/*** CHXCAT.  Concatenate one chix onto the end of another.
/
/    chxcat (a, b);
/
/    Parameters:
/       Chix  a, b;
/
/    Purpose:
/       Concatenate chix B onto the end of chix A.
/
/    How it works:
/       If type(A) < type(B), A is first promoted to B's type and
/       then B is concatenated onto the end of A.
/
/       If type(A) > type(B), B's characters are promoted to A's
/       type as they are added onto the end of A.
/
/    Returns: 1 on success, 0 on error.
/
/    Error Conditions:  A or B not chix.
/                       Cannot expand A.
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Speed: don't need size comparisons for 1 byte; may be able
/       to use memcpy instead of chxcapc.
/
/    Home:  chx/chxcat.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR  7/08/91 15:32 Fix chxcheck() call. */
/*: CR 10/07/91 17:44 Use and set actlen. */
/*: CR  1/03/92 13:27 Return 0 on errors. */
/*: CR 12/11/92 11:05 chxcapc last arg is long. */
/*: OQ 02/09/98 replace chxcheck call by in line commands */
#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxcat (a, b)
   Chix   a, b;
{
   int4   a_len, b_len;

   ENTRY ("chxcpy", "");

   /*if (NOT chxcheck (a, "chxcat")  ||  NOT chxcheck (b, "chxcat"))  RETURN(0);*/
   if (a == nullchix || b == nullchix )
      { chxerror (CXE_NULL, "chxcat" , "");          RETURN(0); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxcat" , a->name);    RETURN(0); }
   if (FREED(b))    
      { chxerror (CXE_FREED,  "chxcat" , b->name);   RETURN(0); }
   if ( (NOT ISCHIX(a)) || (NOT ISCHIX(b)) )
      { chxerror (CXE_NOTCHX, "chxcat" , "");        RETURN(0); }
   
   b_len = chxlen (b);
   a_len = chxlen (a);

   if (a->type < b->type  ||  a->maxlen < a_len + b_len + 1)
      if (NOT chxresize (a, b->type, a_len + b_len + 5))  RETURN (0);

   chxcapc (a->p + (a_len * a->type), a->type, b->p, b->type, -1L);
/* a->actlen = -1L; */
   a->actlen = a_len + b_len;

   RETURN  (1);
}
