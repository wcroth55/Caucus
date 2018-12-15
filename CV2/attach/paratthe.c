/*** PAR_ATT_HEADER    Parse an attachment header line in a response partfile.
/
/    ok = par_att_header (attachment, line);
/
/    Parameters:
/       int         ok;          (success?)
/       Attachment  attachment;  (object to fill in)
/       Chix        line;        (line to parse
/
/    Purpose:
/       Parse a line from a response partfile which contains attachment info.
/
/    How it works:
/
/    Returns: 1 on success
/             0 if line is bad (DB error)
/
/    Error Conditions: 
/  
/    Side effects: 
/
/    Related functions:  make_attach()  free_attach() 
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  attach/paratthe.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  4/02/93 10:32 Create this. */
/*: CP  8/25/93 12:47 Use attach->db_name instead of attach->name. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "done.h"

FUNCTION  par_att_header (attach, line)
   Attachment attach;
   Chix       line;
{
   int  success;
   int4 rec_len;
   Chix format, size, rec_len_c;

   size      = chxalloc (L(8),  THIN, "par_att_h size"); 
   format    = chxalloc (L(40), THIN, "par_att_h format");
   rec_len_c = chxalloc (L(8),  THIN, "par_att_h rec_len_c");;

   if (attach == (Attachment) NULL)  FAIL;

   chxtoken (format, nullchix, 2, line);
   if ((attach->format = get_attach_def (format)) == (Attach_Def) NULL)
      FAIL;
   chxtoken (size, nullchix, 3, line);
   chxnum   (size, &attach->size);
   chxtoken (attach->db_name, nullchix, 4, line);
   if (NOT chxtoken (rec_len_c, nullchix, 5, line)) FAIL;
   chxnum   (rec_len_c, &rec_len);
   attach->rec_len = rec_len;

   SUCCEED;

done:
   chxfree (size);
   chxfree (format);
   chxfree (rec_len_c);

   return  (success);
}

