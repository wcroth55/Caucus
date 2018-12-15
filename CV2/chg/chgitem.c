
/*** CHG_ITEM.   Change an item.
/
/    CHG_ITEM prompts the user
/    for the kind of change to be made.  (edit, delete, replace, just
/    like it is for a response.) */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:33 Source code master starting point */
/*: JV  8/22/89 18:49 Tell user that there is no item */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR  9/15/89 17:13 Check thisuser->read_only. */
/*: CR 10/11/89 16:11 Revert to "no items selected" if parserange fails. */
/*: CR  3/04/90 22:43 Add context argument to textenter() call. */
/*: CR  3/22/90 12:18 Unlock XITX before all RETURN's! */
/*: CR  7/13/90 14:22 Use thisuser->is_organizer. */
/*: CR  8/12/90 20:21 Cast textenter/textedit arg to make Lint happy. */
/*: CR  4/08/91 18:48 Add new unit_lock argument. */
/*: CR  4/09/91 13:51 Add new confnum argument to updatetext() call. */
/*: CR  6/19/91 16:15 Use new equal_id() function. */
/*: CR  6/25/91 14:10 Use chg_ditem() to modify item partfile. */
/*: CR  7/08/91 12:16 Add 3rd arg to chixfile(). */
/*: CR  7/23/91 15:00 Add 3rd argument to equal_id() call. */
/*: CR  7/27/91 13:56 Replace NULL with null.str. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: CX 11/10/91 17:40 Chixify .*/
/*: JV 10/31/91 14:25 Add dest_chn to chgxitem() call. */
/*: UL 12/11/91 18:41 Make unit_lock args 3-6 int4.*/
/*: JV 12/16/91 16:29 Remove pdir. */
/*: CR  1/03/92 14:53 Check return from chixfile(). */
/*: JV  5/13/92 11:25 Define return variable for chixfile(). */
/*: JX  5/19/92 12:26 Add cnum to getline2(), Fix Chix. */
/*: JX  7/31/92 17:23 Change dest_chn to chix in chgxitem(). */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR  8/07/92 14:07 Allow CHANGE ITEM <file. */
/*: CI 10/05/92 11:25 Chix Integration. */
/*: CR 10/11/92 21:42 Remove CQ from equal_id() call. */
/*: CR  1/13/93 12:45 Add chxwrap() after chixfile(). */
/*: JV  4/05/93 17:10 Add attach arg to chg_ditem(). */
/*: JV  6/08/93 16:14 Don't need to get title here. */
/*: CP  6/27/93 20:45 Trim out asking user for item. */
/*: CP  7/14/93 15:12 Integrate 2.5 changes. */
/*: J1  8/03/93 10:28 Unlock XITX even if changes cancelled. */
/*: CK  8/06/93 16:31 Yet another integration. */
/*: CK 10/14/93 14:32 Display header & title. */
/*: CP 11/17/93 17:59 Comment out redundant show_header() call. */
/*: CR  5/06/95 16:43 Use limit_10k to limit # lines of text. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"
#include "derr.h"
#include "xaction.h"

extern Userreg                thisuser;
extern struct master_template master;
extern Textbuf                tbuf;
extern union  null_t          null;
extern Chix                   yesnotable;
extern short                  confnum;

FUNCTION  chg_item (in, item)
   int    in, item;
{
   static Chix  itemtab = nullchix;
   int    i, select, error1, error2, oitem, got_it;
   int    success;
   short  am_author, ok;
   Chix   ctext, line, def, hdrstr, format, output, ownerid,
          answer, editor, ohost;
   Chix   title;
   Chix   getline2();

   ENTRY ("chgitem", "");

   mdtable ("chg_Aitem", &itemtab);

   ohost      = chxalloc (L(90), THIN, "chgitem ohost");
   def        = chxalloc (L(10), THIN, "chgitem def");
   hdrstr     = chxalloc (L(90), THIN, "chgitem hdrstr");
   answer     = chxalloc (L(10), THIN, "chgitem answer");
   editor     = chxalloc (L(60), THIN, "chgitem editor");
   format     = chxalloc (L(80), THIN, "chgitem format");
   output     = chxalloc (L(80), THIN, "chgitem output");
   ownerid    = chxalloc (L(80), THIN, "chgitem ownerid");
   success    = 0;

   /*** Display the header for this item. */
   title = getline2 (XCRD, XCRF, confnum, nullchix, item, 0, 1, tbuf);
   if (title == nullchix)  FAIL;
   chxcpy (hdrstr, getline2 (XCRD, XCRF, confnum, nullchix, item, 0, 0, tbuf));
   show_header (XWTX, item, hdrstr, title, 1);

   /*** Make sure this item belongs to the user. */
   chxtoken (ownerid, nullchix, 2, hdrstr);
   am_author = equal_id (ownerid, thisuser->id, nullchix);
   if (NOT am_author  &&  NOT thisuser->is_organizer) {
      mdwrite (XWER, "chg_Tnotyours", null.md);
      goto done;
   }

   if (in == XKEY) {
      unit_write  (XWTX, NEWCPAGE);
/*    show_header (XWTX, item, nullchix, nullchix, 0); */
  
      /*** If the Organizer is changing someone else's item, get confirmation
      /    that pern really wants to do it. */
      if (NOT am_author  &&  NOT org_confirm())  goto done;
  
      /*** Find out what the user wants to do to this item. */
      chxtoken (def, nullchix, 1, itemtab);
      select = mdprompter ("chg_Pedr", itemtab, 2, "chg_Hedr", 60, answer, def);
  
      /*** Failed, <cancel>, or STOP goes home. */
      if (select<0  ||  select==3)  goto done;
  
  
      /*** Handle item DELETION. */
      if (select == 2) {
         /*** First, get confirmation from user. */
         mdwrite  (XWTX,   "chg_TdeleteI", null.md);
         chxtoken (def, nullchix, 1, yesnotable);
         if (mdprompter ("chg_PdeleteI", yesnotable, 2, "chg_HdeleteI", 10,
                           answer, def) < 1)  goto done;
  
         /*** If distributed, write item deletion transaction.  Then
         /    actually delete the item. */
         if (is_distributed (confnum)) {
            getoitem (&oitem, ohost, item, &master);
            chgxitem (XT_DELETE, confnum, oitem, ohost, nullchix, nullchix,
                      nullchix, nullchix, nullchix, nullchix, nullchix, 0,
                     &error2);
         }
         chg_ditem (XT_DELETE, confnum, -1, &item,
                 nullchix, nullchix, nullchix, nullchix, nullchix,
                 nullchix, nullchix, (Attachment) NULL, 0, nullchix, &error1);
  
         /*** Tell the user it is deleted. */
         mdstr   (format, "chg_FdeleteI", null.md);
         chxformat (output, format, (int4) item, L(0), nullchix, nullchix);
         unit_write (XWTX, output);
         success = 1;
         goto done;
      }
  
      /*** The rest of the code handles EDIT and REPLACE.  In either case,
      /    we end up with (edited or new) text in XITX.  If edit, find out
      /    which editor, copy the item text into XITX, and edit it. */
      unit_lock (XITX, WRITE, L(0), L(0), L(0), L(0), nullchix);
      if (iseditreq (answer, def, thisuser->editor, editor)) {
  
         /*** Copy the text of the item into XITX. */
         if (NOT unit_make (XITX) )  { unit_unlk (XITX);    goto done;  }
  
         for (i=2;   (line = getline2 (XCRD, XCRF, confnum,
                          nullchix, item, 0, i, tbuf)) != nullchix;
                     ++i) {
            unit_write (XITX, line);
            unit_write (XITX, NEWLINE);
         }
         unit_close (XITX);
  
         /*** Allow the user to edit XITX. */
         mdwrite (XWTX, "chg_TnoweditI", null.md);
         if (NOT textedit (XITX, editor, (int) thisuser->truncate)) {
            unit_unlk (XITX);
            goto done;
         }
      }
  
      /*** If REPLACE, just ENTER the text again. */
      else {
         mdwrite (XWTX, "chg_Treplace", null.md);
         unit_kill  (XITX);
         if (NOT textenter (XITX, (int) thisuser->truncate, 0, 0, 0)) {
            unit_unlk (XITX);
            goto done;
         }
      }
  
      /*** Confirm that the user really wants to make these changes. */
      chxtoken (def, nullchix, 2, yesnotable);
      ok = mdprompter ("chg_Pchgok", yesnotable, 2, "chg_Hchgok",
                       40, answer, def);
      in = XITX;
      if (ok <= 0) {
         mdwrite (XWTX, "chg_Tcancelled", null.md);
         unit_unlk (XITX);
         goto done;
      }
   }

   /*** Copy XITX back into partfiles. */
   ctext = chxalloc (L(500), THIN, "chgitem ctext");
   got_it = chixfile  (in, ctext, "");
   if (got_it == 0) {
      mdwrite (XWTX, "chg_Tcancelled", null.md);
      chxfree (ctext);
      goto done;
   }
   if (got_it == -1)  mdwrite (XWTX, "ent_Tenpartial", null.md);
   chxwrap (ctext, 77);
   if (limit_10k (ctext))  mdwrite (XWER, "ced_Elines", null.md);
  
   chg_ditem (XT_CHANGE, confnum, -1, &item,
              nullchix, nullchix, nullchix, ctext, nullchix,
              nullchix, nullchix, (Attachment) NULL, 0, nullchix, &error1);
   if (is_distributed (confnum)) {
      getoitem (&oitem, ohost, item, &master);
      chgxitem (XT_CHANGE, confnum, oitem, ohost, nullchix, nullchix,
                nullchix, ctext, nullchix, nullchix, nullchix, 0, &error2);
   }
   chxfree   (ctext);

   mdstr   (format, "chg_Fupdated", null.md);
   chxformat (output, format, (int4) item, L(0), nullchix, nullchix);
   unit_write (XWTX, output);
   if (in == XITX)  unit_unlk (XITX);
   success = 1;

done:
   chxfree (def);
   chxfree (ohost);
   chxfree (hdrstr);
   chxfree (answer);
   chxfree (editor);
   chxfree (format);
   chxfree (output);
   chxfree (ownerid);
   RETURN  (success);
}
