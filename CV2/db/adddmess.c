/*** ADD_DMESS.  Send a message to a user on this host.
/
/    ok = add_dmess (author, authorid, authorchn, subject,
/                    messtype, date, time, prevtext, prevparts,
/                    retrecsize, recipid, newtext, attach, error);
/
/    Parameters:
/       int    ok;          (Success?)
/       Chix   author;      (full name of message author)
/       Chix   authorid;    (userid of message author)
/       Chix   authorchn;   (author lives on this host)  (may be nullchix!)
/       Chix   subject;     (one-line subject of message)
/       int    messtype;    (type of message, from mess.h)
/       Chix   date;        (date message was created)
/       Chix   time;        (time message was created)
/       Chix   prevtext;    (text of "previous" parts of message)
/       int    prevparts;   (number of parts in prevtext)
/       int    retrecsize;  (size of desired return receipt)
/       Chix   recipid;     (userid of recipient)
/       Chix   newtext;     (text of new part of message)
/       Attachment attach;  (info about message attachment)
/       int   *error;       (return error code)
/
/    Purpose:
/       Add_dmess() is the one true way of actually sending a message to
/       someone on the current host.  All Caucus and CaucusLink code that
/       sends messages must call this function.
/
/    How it works:
/       Retrecsize of -1 means no return receipt was requested.  Otherwise,
/       it is the number of lines in the original message that should
/       accompany the return receipt.
/
/       PREVTEXT and NEWTEXT must be allocated Chixen.
/       ATTACH may be NULL, which means "no attachment".
/
/       Messtype must be MSEND, MFORWARD, or MREPLY, from mess.h.
/
/    Returns: 1 on success, sets ERROR to 0.
/             0 on any error condition, see below.
/
/    Error Conditions: sets value of ERROR accordingly:
/       DNOMEM       ran out of dynamic memory
/       DNOUSER      userid recipid does not exist
/       DBOXFULL     this user's mailbox is full
/ 
/    Side effects: writes in text buffer structures block/line/text.
/                  writes in partfile directory pdir.
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/adddmess.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/27/91 17:47 New function. */
/*: CR  7/08/91 16:38 Misc. fixes. */
/*: CR  7/16/91 17:08 Add HOST argument to person_format() calls. */
/*: CR  7/26/91 18:19 Correct call to person_format(). */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR 10/10/91 21:32 Add host to signature. */
/*: CR 10/11/91 11:25 Display debugging info only if debug is on. */
/*: CR 11/19/91 18:31 Add PREFACE at bottom only if NEWTEXT not empty. */
/*: CR 11/21/91 17:25 Fix previous fix. */
/*: CR 11/25/91 13:51 Don't add 1 to PARTS if MFORWARD. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JV 12/12/91 16:39 Add to header. */
/*: WC 12/18/91 22:09 Extract preface assembly code into build_preface(). */
/*: CR  1/10/92 22:44 Hard-code SUBJECT field. */
/*: JX  5/13/92 15:45 Fix mesdir flag array, remove BLOCK. */
/*: CX  5/27/92 17:33 Fix minor debug sprintf mismatch. */
/*: CX  5/28/92 18:02 Add tbuf arg to updatepart() call. */
/*: JX  5/29/92 14:33 Chixify 8th arg to updatepart() call. */
/*: CR 10/11/92 22:36 Allow for AUTHORCHN == nullchix. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: CL 12/10/92 17:06 Long ascofchx args. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: JV  4/05/93 17:46 Add attachment arg. */
/*: CN  5/15/93 21:43 Make mesdir short. */
/*: CP  7/14/93 15:38 Integrate 2.5 changes. */
/*: CT  8/16/93 13:22 ATTACH arg may be NULL. */
/*: CR  5/06/95 16:43 Use limit_10k to limit # lines of text. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"
#include "derr.h"

extern Textbuf               tbuf;
extern union  null_t         null;
extern int                   debug;

FUNCTION  add_dmess (author, authorid, authorchn, subject,
                     messtype, date, time, prevtext, prevparts,
                     retrecsize, recipid, newtext, attach, error)
   Chix   author, authorid, authorchn, recipid;
   Chix   subject, date, time;
   Chix   prevtext, newtext;
   int    messtype, prevparts, retrecsize, *error;
   Attachment attach;
{
   int    highest, cmi, parts, success;
   Chix   header, preface, cmiesc, chn;
   Chix   ctext;
   short *mesdir;
   Chix   a;
   char  *sysmem();

   char  tempchar1[80], tempchar2[80], tempchar3[80];

   ENTRY ("add_dmess", "");

   header    = chxalloc (L(160), THIN, "add_dmess header");
   preface   = chxalloc (L(160), THIN, "add_dmess preface");
   cmiesc    = chxalloc (L(40),  THIN, "add_dmess cmiesc");
   chn       = chxalloc (L(40),  THIN, "add_dmess chn");
   a         = chxalloc (L(100), THIN, "add_dmess a");
   ctext     = chxalloc (L(200), THIN, "add_dmess ctext");
   if (authorchn != nullchix)  chxcpy (chn, authorchn);
   success   = 0;

   if (debug) {
      printf ("-------Add_dmess()---------\n");
      ascofchx(tempchar1,author,0L,80L);
      ascofchx(tempchar2,authorid,0L,80L);
      ascofchx(tempchar3,chn,0L,80L);
      printf ("  author='%s', authorid='%s', chn='%s'\n", tempchar1,
                 tempchar2, tempchar3);
      ascofchx (tempchar1, subject, L(0), L(80));
      printf ("  subject='%s'\n", tempchar1);
      ascofchx(tempchar1,date,0L,80L);
      ascofchx(tempchar2,time,0L,80L);
      printf ("  messtype=%d, date='%s', time='%s'\n", messtype, tempchar1,
                 tempchar2);
      if (attach != (Attachment) NULL) {
         ascofchx (tempchar1,attach->db_name,0L,80L);
         ascofchx (tempchar2,attach->name,0L,80L);
         ascofchx (tempchar3,attach->format->format,0L,80L);
         printf ("  attach: db_name='%s', name='%s', format='%s'\n", tempchar1,
                 tempchar2, tempchar3);
      }
   }
   chxcatsub (a, prevtext, L(0), L(50));
   if (debug) {
      ascofchx (tempchar1, prevtext, L(0), L(80));
      printf ("  prevtext starts '%s'\n", tempchar1);
      ascofchx (tempchar1, recipid, L(0), L(80));
      printf ("  prevparts=%d, retrecsize=%d, recipid='%s'\n",
                 prevparts, retrecsize, tempchar1);
   }
   chxclear  (a);
   chxcatsub (a, newtext,  L(0), L(50));
   if (debug) {
      ascofchx (tempchar1, a, L(0), L(80));
      printf   ("  newtext starts '%s'\n", tempchar1);
      printf   ("\n");
   }

   /*** Lock and load the recipient's message directory into messdir. */
   if ( (mesdir = (short *) sysmem (sizeof(short) * L(MAXMESS+1), "adddmess")) == NULL)
      { *error = DNOMEM;                         FAIL; }
   if (NOT unit_lock (XUMD, WRITE, L(0), L(0), L(0), L(0), recipid))
      { *error = DNOUSER;   sysfree ( (char *) mesdir);    FAIL; }

   highest = loadmdir (0, mesdir);

   /*** HIGHEST is the number of the new message. */
   if (++highest >= MAXMESS) {
      unit_unlk (XUMD);
      sysfree ( (char *) mesdir);
     *error = DBOXFULL;
      FAIL;
   }
   mesdir [highest] = MSG_NEW | (retrecsize >= 0 ? MSG_RRR : 0);

   /*** Each message begins with a HEADER and a PREFACE.  Assemble the
   /    header. */
   mdstr (cmiesc, "ss_cmiesc", null.md);
   cmi   = (chxindex (authorid, L(0), cmiesc) == 0);
   parts = prevparts + (messtype==MFORWARD ? 0 : 1);
   if (cmi) {
      chxformat (header, CQ("#%d %s %d"), L(highest), L(parts), authorid,
                   null.chx);
      chxformat (header, CQ(" %d"), L(retrecsize), L(0), null.chx, null.chx);
   }
   else {
      chxformat (header, CQ("#%d %s@%s %d"), L(highest), L(parts), authorid,
                   chn);
      chxformat (header, CQ(" %d"), L(retrecsize), L(0), null.chx, null.chx );
   }

   /*** Assemble the message preface. */
   build_preface (preface, highest, messtype, author, authorid, chn,
                           date, time, cmi);

   /*** Assemble the complete text of the message. */
   chxcat (ctext, preface);
   chxcat (ctext, chxquick ("     SUBJECT: " , 0));
   chxcat (ctext, subject);
   chxcat (ctext, chxquick ("\n", 0));
   chxcat (ctext, prevtext);
   if (NOT EMPTYCHX(newtext)) {
      if (prevparts > 0)   chxcat (ctext, chxquick ("---\n", 0));
      chxcat (ctext, preface);
      chxcat (ctext, newtext);
   }
   limit_10k (ctext);

   tbuf->type = TB_IS_EMPTY;
   updatepart (XUMF, 0, 0, highest, recipid, ctext, header, nullchix, 0,
                null.chx, attach, tbuf, 0, nullchix);
   tbuf->type = TB_IS_EMPTY;

   /*** rewrite the recipient's message directory. */
   storemdir (highest, mesdir);
   sysfree   ( (char *) mesdir);
   unit_unlk (XUMD);

   SUCCEED;

done:
   chxfree (header);
   chxfree (preface);
   chxfree (cmiesc);
   chxfree (a);
   chxfree (ctext);
   chxfree (chn);

   RETURN  (success);
}
