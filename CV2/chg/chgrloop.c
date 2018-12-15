
/*** CHG_RLOOP.  Offer user chance to change a set of responses.
/
/    chg_rloop (item, first, last, justone, in);
/
/    Parameters:
/       int    item;        (item number)
/       int    first;       (resp # of lowest  response to change)
/       int    last;        (resp # of highest response to change)
/       int    justone;     (only change 1 response?)
/       int    in;          (unit # of source of input)
/
/    Purpose:
/       Interact with user, offering chance to change one or more responses
/       in a given range.
/
/    How it works:
/       Display responses (that user is allowed to change) in range 
/       (FIRST,LAST) in reverse order.  For each such response, offer
/       chance to change it.  User may:
/          proceed to previous response
/          delete  this response
/          replace this response
/          edit    this response
/          examine a specific response
/          stop
/
/       If JUSTONE is true, chg_rloop() returns after first successful
/       delete/replace/edit.
/
/       A user is allowed to change a response if they own the response,
/       or if they have organizer privilege.
/
/       If IN is other than XKEY, automatically use the contents of
/       that file (already opened by the caller) to REPLACE the 
/       first (and only the first) selected response.  If more than
/       one response is selected, the remainder will be treated as
/       though no file redirection was supplied.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  chgresp(), resploop()
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  chg/chgrloop.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:34 Source code master starting point */
/*: CR 11/09/88 13:05 Handle <CANCEL> while changing another's response. */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR  9/06/89 10:35 Add 5th arg to show_resp. */
/*: CR  3/04/90 22:35 Add context argument to enterresp() call. */
/*: CR  3/21/90 22:31 Write NEWPAGE before show_response() call. */
/*: CR  3/24/90 13:48 Iseditreq() needs full list of edit synonymns. */
/*: CR  7/13/90 14:24 Use thisuser->is_organizer. */
/*: CR  4/08/91 18:48 Add new unit_lock argument. */
/*: CR  4/09/91 13:51 Add new confnum argument to updatetext() call. */
/*: CR  6/19/91 16:05 Use equal_id() function. */
/*: CR  6/27/91 16:12 Use chg_dresp() to delete responses. */
/*: CR  7/23/91 15:00 Add 3rd argument to equal_id() call. */
/*: CR  7/27/91 13:58 Replace NULL with null.str. */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: CX 11/10/91 18:55 Chixify. */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR 10/29/91 12:50 Add confnum arg to getoresp() call. */
/*: JV 10/31/91 14:28 Add dest_chn arg to chgxresp() call. */
/*: CR 12/21/91 15:58 Skip deleted responses. */
/*: JX  5/19/92 13:08 Fix Chix. */
/*: CX  6/05/92 16:24 XKEY --> u_keybd. */
/*: JX  6/09/92 17:19 chxtoken returns -1 on fail. */
/*: JX  6/29/92 16:31 More chix fix. */
/*: JX  8/03/92 09:57 CHN's are Chix. */
/*: CR  9/09/92 14:01 Add SHOWN arg to show_resp() call.  */
/*: CI 10/05/92 12:00 Chix Integration. */
/*: CR 10/09/92 16:59 Remove unused dumint4. */
/*: CR 10/11/92 21:43 Remove CQ from equal_id() call. */
/*: TP 10/19/92 14:05 Replace u_keybd with input_control(). */
/*: CR 10/27/92 16:06 Test return value from getline2(). */
/*: CR 12/11/92 15:17 chxint4 int4 arg. */
/*: JV  3/03/93 14:54 Add attach arg to chg_dresp(). */
/*: CP  6/27/93 17:31 Rework actions, arguments, header. */
/*: CP  7/16/93 13:53 Remove unused 'found'. */
/*: CP  8/24/93 15:37 Add attach,confnum args to show_resp() call. */
/*: CP  9/15/93 15:42 Use new get_resp_text()/enterresp() calls. */
/*: CK  9/16/93 22:05 Remove unused u_keybd. */
/*: CK  9/19/93  0:07 Add 'onlyattach' arg to show_resp() call. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"
#include "xaction.h"
#include "derr.h"

extern struct master_template master;
extern Userreg                thisuser;
extern Textbuf                tbuf;
extern union  null_t          null;
extern short                  confnum;

FUNCTION  chg_rloop (item, first, last, justone, in)
   int    item, first, last, justone, in;
{
   static Chix cdrtable = nullchix;
   int    r, choice, error1, error2, oitem, oresp, shown, success;
   int4   lpos;
   short  am_author;
   Chix   answer, myid, ownerid, itemchn, respchn, editor, editword;
   Chix   response;
   Chix   getline2();

   ENTRY ("chgrloop", "");

   mdtable ("chg_Achgr", &cdrtable);
   myid     = chxalloc (L(30), THIN, "chg_rloop myid");
   ownerid  = chxalloc (L(30), THIN, "chg_rloop ownerid");
   editor   = chxalloc (L(60), THIN, "chg_rloop editor");
   editword = chxalloc (L(60), THIN, "chg_rloop editword");
   answer   = chxalloc (L(10), THIN, "chg_rloop answer");
   respchn  = chxalloc (L(30), THIN, "chg_rloop respchn");
   itemchn  = chxalloc (L(30), THIN, "chg_rloop itemchn");

   sysuserid (myid);

   /*** Examine the responses in the (FIRST,LAST) range in LIFO order. 
   /    If user can change it (is owner or is organizer), display it. */
   if (last  < 0  ||  last > master.responses[item])
      last  = master.responses[item];
   if (first < 1)    first = 1;
   for (r=last;  r>=first;  --r) {
      if (sysbrktest())  DONE(1);

      /*** Get the response header.  Skip response if deleted.  Is this
      /    user the author of the response? */
      response = getline2 (XCRD, XCRF, confnum, nullchix, item, r, 0, tbuf);
      if (response == nullchix)                             continue;
      if (chxtoken (ownerid, nullchix, 2, response) == -1)  continue;
      am_author = equal_id (ownerid, myid, nullchix);

      /*** Response must be owned by user, or user must be organizer. */
      if (NOT am_author  &&  NOT thisuser->is_organizer)    continue;

      /*** If replacement text coming in from a "<file", just do the
      /    replacement and go on. */
      if (in != XKEY) {
         enterresp (item, r, in);
         in = XKEY;
         if (justone)  DONE(1);
         continue;
      }

      unit_write     (XWTX, NEWCPAGE);
      show_responses (XWTX, confnum, item, r, r, 0, 0, 0, &shown, 0);

      /*** If organizer, ask for confirmation to change. */
      if (NOT am_author  &&  NOT org_confirm())             continue;

      /*** Ask the user what to do with this response. */
      choice = mdprompter ("chg_PedrR", cdrtable, 1, "chg_HedrR",
                                       60, answer, nullchix);
      switch (choice) {
         case (-4):
         case (-3): DONE(1);   /* user hit break */

         case (-2): break;     /* <RETURN>.  Go on to previous response */

         case (-1):            /* user entered number. */
            lpos = L(0);
            r = chxint4 (answer, &lpos);
            if (lpos == L(0))   continue;
            first = r = BOUND (r, 1, master.responses[item]);
            ++r;
            break;

         case ( 0): DONE(1);   /* user wants to STOP */

         case ( 1):            /* EDIT */
            mdwrite   (XWTX, "chg_TeditR", null.md);
            chxtoken  (editword, nullchix, 2, cdrtable);
            iseditreq (answer, editword, thisuser->editor, editor);
            editresp  (item, r, editor);
            if (justone)  DONE(1);
            break;

         case ( 2):            /* DELETE */
            if (is_distributed (confnum)) {
               getoitem (&oitem, itemchn, item, &master);
               getoresp (&oresp, respchn, confnum, item, r);
               chgxresp (XT_DELETE, confnum, oitem, itemchn, oresp, respchn,
                           nullchix, nullchix, nullchix, nullchix, nullchix,
                           nullchix, 0, &error2);
            }
            chg_dresp (XT_DELETE, confnum, item, -1, &r, nullchix, nullchix,
                       nullchix, nullchix, nullchix, nullchix, 
                       (Attachment) NULL, 0, nullchix, &error1);
            if (justone)  DONE(1);
            break;

         case ( 3):            /* REPLACE */
            if (get_resp_text(0)  && 
                    unit_lock (XITX, WRITE, L(0), L(0), L(0), L(0), CQ(""))) {
               enterresp (item, r, XITX);
               unit_unlk (XITX);
            }
            if (justone)  DONE(1);
            break;
      }
   }

done:
   chxfree (itemchn);
   chxfree (respchn);
   chxfree (answer);
   chxfree (editword);
   chxfree (editor);
   chxfree (ownerid);
   chxfree (myid);
   RETURN  (success);
}
