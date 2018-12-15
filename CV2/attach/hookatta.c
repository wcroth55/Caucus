/*** HOOK_ATTACHMENT.   Hook an attachment onto a specific response.
/
/    hook_attachment (cnum, inum, rnum, nametype, red, new, error);
/
/    Parameters:
/       int       cnum;      (conference number)
/       int       inum;      (item number)
/       int       rnum;      (response number)
/       Chix      nametype;  ("name type" string as parsed from command line)
/       Redirect *red;       (redirection information, if specified)
/       int       new;       (1=must be new attach, 0=must be replacement)
/       int      *error;     (returned error code)
/
/    Purpose:
/       Add an attachment to response (CNUM,INUM,RNUM).
/
/    How it works:
/       The source of the attachment (the user's file) is named in
/       either (or both) of NAMETYPE and RED.  (The name in NAMETYPE
/       is 1st choice for the "remembered" name of the file in the
/       database, the name in RED is the 1st choice for where the file
/       really lives right now.)
/
/       If NEW is 1, this *must* be a new attachment.  If NEW is 0, this
/       *must* be a replacement attachment.  Otherwise an error code is
/       returned.
/
/    Returns: 1 on success
/             0 on error
/
/    Error Conditions: (returned value of error)
/      -1   response does not exist.
/      -2   response already has an attachment (new==1)
/      -3   no file name in NAMETYPE or RED
/      -4   file does not exist (cannot be read)
/      -5   can't get file size
/      -6   response does not have an attachment (new==0)
/      -7   user is not owner of response!
/      >0   error return from a_chg_resp()
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  additem()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  cmd1/hookatta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  8/10/93 13:16 New function. */
/*: CP  8/12/93 17:05 Add arg 'error', more return code information. */
/*: CK  8/13/93 17:06 Rename to hook_attachment(), add 'new' arg. */
/*: CP  8/25/93 21:48 Add error code -7, user not owner. */
/*: CR  7/05/94 13:24 Use trim_filename() to remove full pathname spec. */
/*: CR  2/04/95 18:56 Use real userid when getting attachment file size. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"
#include "redirect.h"
#include "api.h"
#define  ATTNULL   ((Attachment) NULL)

FUNCTION  hook_attachment (cnum, inum, rnum, nametype, red, new, error)
   int       cnum, inum, rnum, new, *error;
   Chix      nametype;
   Redirect *red;
{
   int       success, syserr, status, exists, ok;
   Resp      resp;
   Chix      realname, dbname, filetype;

   ENTRY ("hook_attach", "");

   *error = 0;
   realname = chxalloc (L(40), THIN, "hook_atta realname");
   dbname   = chxalloc (L(40), THIN, "hook_atta dbname");
   filetype = chxalloc (L(40), THIN, "hook_atta filetype");

   resp = a_mak_resp ("hook_att resp");
   resp->cnum = cnum;
   resp->inum = inum;
   resp->rnum = rnum;

   if (a_get_resp (resp, P_TEXT | P_TITLE, A_WAIT) != A_OK)
                                                    { *error = -1;  FAIL; }

   /*** Does an attachment already exist?  Compare against NEW argument. */
   exists = (resp->attach != ATTNULL  &&  resp->attach->size > 0);
   if (new==1  &&      exists)                      { *error = -2;  FAIL; }
   if (new==0  &&  NOT exists)                      { *error = -6;  FAIL; }

   chxtoken (dbname,   nullchix, 1, nametype);
   chxtoken (filetype, nullchix, 2, nametype);
   if (red->unit == XIRE  &&  NOT NULLSTR(red->fname))
                               chxofascii (realname, red->fname);
   if (EMPTYCHX  (realname))   chxcpy     (realname, dbname);
   if (EMPTYCHX  (dbname))     chxcpy     (dbname,   realname);
   trim_filename (dbname);
   if (EMPTYCHX  (dbname))                           { *error = -3;  FAIL; }

   if (resp->attach == ATTNULL)  resp->attach = make_attach();
   resp->attach->format = analyze_file (realname, filetype);
   if (resp->attach->format == (Attach_Def) NULL)   { *error = -4;  FAIL; }
/* print_adef (resp->attach->format); */

   /*** To get attachment file size, temporarily revert to real userid. */
   sysprv (0);
   ok = sysfsize (ascquick(realname), &resp->attach->size, &syserr);
   sysprv (1);
   if (NOT ok)   { *error = -5;  FAIL; }

   chxcpy   (resp->attach->db_name, dbname);
   chxcpy   (resp->attach->name,    realname);
   status = a_chg_resp (resp, P_ATTACH | P_TEXT, A_WAIT);
   if (status == A_NOTOWNER)                        { *error = -7;  FAIL; }
   if (status != A_OK)                        { *error = status;    FAIL; }
   SUCCEED;

done:
   a_fre_resp (resp);
   chxfree (realname);
   chxfree (dbname);
   chxfree (filetype);

   RETURN  (success);
}
