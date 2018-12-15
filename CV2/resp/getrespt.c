/*** GET_RESP_TEXT.   Get the text of a response from the user.
/
/    ok = get_resp_text (context);
/
/    Parameters:
/       int   ok;         (success?)
/       int   context;    (context on screen?)
/                          
/    Purpose:
/       Prompt the user for the text of a response, read the text,
/       and store it in file XITX.
/
/    How it works:
/       If CONTEXT is true, it means that there is a context, i.e. text
/       already on the screen that the user would like to (continue to)
/       see, if possible, while entering the new response.  (Doing this
/       is actually handled by textenter().)
/
/    Returns: 1 on success.
/             0 on error, or cancellation by user.
/
/    Side effects:      none
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  resp/getrespt.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CK  9/15/93 14:53 New function, spun-off from enterresp(). */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"
#include "xaction.h"
#include "derr.h"

extern union  null_t           null;
extern Userreg                 thisuser;

FUNCTION  get_resp_text (context)
   int    context;
{
   int    u_data, u_sess, success;

   ENTRY ("get_resp_text", "");

   u_data = input_control (IC_ISUNIT | IC_DATA, nullchix);
   u_sess = input_control (IC_ISUNIT | IC_SESS, nullchix);

   if (u_data == u_sess) {
      if (thisuser->verbose)  mdwrite (XWTX, "ent_Tresverb", null.md);
      else                    mdwrite (XWTX, "ent_Tenresp",  null.md);
   }
   unit_lock (XITX, WRITE, L(0), L(0), L(0), L(0), CQ(""));
   unit_kill (XITX);
   if (NOT textenter (XITX, (int) thisuser->truncate, 0, 0, context))  FAIL;
   SUCCEED;

done:
   unit_unlk (XITX);
   RETURN    (success);
}
