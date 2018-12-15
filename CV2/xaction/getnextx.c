/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** GETNEXTXACT.  Gets the next xaction from a user's xaction file.
/
/    int n = getnextxact (unit, xaction)
/
/    Parameters:
/       int unit;
/       XACTHEADPTR xaction;
/
/    Purpose:
/       This function gets the next xaction from a xaction file
/       and stores the data in a XACTION structure.  It also
/       stores the text of the entire xaction and returns the address
/       of it in XACTTEXT.  (Some functions need the *entire* header.)
/
/       Each time getnextxact() is called, it mallocs storage for
/       XACTTEXT after it parses the header of the xaction.
/       This storage isn't deallocated until getnextxact is called
/       again.  If the caller doesn't call getnextxact() again then
/       the memory doesn't get deallocated.
/       No memory will be allocated if getnextxact fails.
/
/    How it works:
/       Getnextxact reads from the present place in the xaction file.
/       It parses the field name for each line and stores the value
/       associated with it into the XACTION structure.
/
/       There is one field which requires a bit more processing.
/       This is the DATA field, which must be 'announced'
/       by a DATALENgth field.  The DATALEN field gives us an upper
/       bound on the size of the DATA field.  We malloc enough memory
/       for this and then read the text of DATA into it.
/
/       After we have read the entire xaction, we make sure it makes
/       sense with validxact() (which is in this file).
/       If it does, we also create a completely text version of this
/       and store it at XACTTEXT.  Memory for this is handled the same
/       way we handle memory for DATA.
/
/       When there are no more transactions, unit_read fails but
/       FIRST_LINE is true.  If unit_read fails reasons other than EOF,
/       this part of the logic will fail.
/
/       Finally, note that for xactions that have a DATA field, the
/       line that's just "DATA=" is counted as part of the header,
/       not as part of the data.
/
/       getnextxact() assumes that UNIT has been unit_lock'd and
/       unit_view'd.  It also assumes that the caller will unit_close
/       and unit_unlk the unit.
/
/   WARNINGS:
/   1) getnextxact() assumes that the caller has allocated and initialized
/      PXACTION.  The best way to do this is with make_xaction().
/
/   2) There are a few bugs concerning XACTTEXT.  It won't actually
/   correctly contain the text of the xaction.  But since there's now
/   putnextxact(), there is no use for XACTTEXT.  It's just sitting there
/   on the chopping block...  (As of 7/30/91, most of its remains are gone...)
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/      Can't read from UNIT.
/      Bad format for a xaction.
/
/   Related functions: validxact()
/
/   Called by: moveuxact(), xfu (Main)
/
/   Operating System dependencies: none
/
/   Home: xaction/getnextx.c
/ */

/*: JV  4/03/91 15:05 Create function. */
/*: JV  6/26/91 13:19 Add mailctrl field, etc. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 15:12 Change FieldTable macros. */
/*: JV  7/04/91 17:54 Removed lint. */
/*: JV  7/09/91 09:46 Moved empty_xact to xfpgloba.c */
/*: JV  7/12/91 15:08 Add MTYPE and MESSTABLE, O_PARTS. */
/*: JV  7/15/91 16:32 Add code for DATATEXT2. */
/*: JV  7/17/91 11:46 Minor mods. */
/*: JV  7/18/91 10:51 Increase debug granularity, fix sysmem(). */
/*: JV  7/22/91 16:36 Add ENTRY. */
/*: JV  7/26/91 21:52 Fix bugs. */
/*: CR  7/28/91 15:08 Carve mark all changes! */
/*: JV  7/29/91 12:31 Read in data of xaction as a Chix. */
/*: JV  8/05/91 13:14 Fix Transit bug. */
/*: JV  9/30/91 15:00 Change end-of-transaction marker from '.' to ';'. */
/*: JV 10/08/91 13:30 Change validxact to allow mail. */
/*: JV 10/10/91 19:22 Increase textbuffer size. */
/*: JV 10/11/91 10:00 Strip off one-space pad in text lines. */
/*: JV 10/28/91 14:50 Added 2nd arg to cnumber().*/
/*: JV 10/29/91 15:24 Expand validxact(). */
/*: JV 10/31/01 13:49 Remove final vestiges of xacttext_data. */
/*: JV 11/06/91 10:29 Change common.h to null.h, add chixuse.h, add TCONF@CHN. */
/*: JV 11/21/91 18:07 Fix MESSTYPE bug. */
/*: WC 11/29/91 21:14 Add 3rd arg to cnumber(). */
/*: JV 12/07/91 19:44 Declare chxalloc */
/*: JV 12/12/91 18:45 Allow TT_HOST xactions. */
/*: CR 12/23/91 22:51 Protect printxact() against NULLs and &*%$ int4s!. */
/*: JV  1/02/92 16:17 Allow TT_SUBDEL, TT_SUBACK. */
/*: JV  1/08/92 16:04 Allow TT_ROUTES. */
/*: CR  4/16/92 17:05 Expanded size of LINE and other char buffers. */
/*: DE  6/04/92 13:03 Chixified */
/*: DE  6/08/92 15:14 Fix Chix */
/*: JX  9/01/92 13:15 Remove empty_xact. */
/*: JX  9/23/92 14:24 Don't need to realloacate data buffers. */
/*: CL 12/10/92 17:07 Long ascofchx args. */
/*: CL 12/11/92 11:18 Long Chxcatsub args. */
/*: CL 12/11/92 13:48 Long Chxvalue N. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: CR 12/15/92 14:01 Replace ALLCHARS in ascofchx() with char array size. */
/*: CN  4/30/93 12:12 Use ascquick in sprintf(...name); increase bugbuf. */
/*: JV  4/30/93 14:57 Ascquick name before using it in sprintf (2nd one). */
/*: JV  5/28/93 10:01 Clear_xaction() before using the pointer passed to us. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"
#include "handicap.h"
#include "goodlint.h"
#include "null.h"
#include "xaction.h"
#include "conf.h"
#include "chixuse.h"

extern union null_t null;
extern Chix fieldtable;
extern Chix typetable,
            actiontable,
            subtable,
            messtable,
            mctrltable;
extern int  debug;

FUNCTION getnextxact (unit, pxaction)
   int  unit;
   XACTIONPTR pxaction;
{
   static Flag done1=0;
   int  namenum, which, v, success;
   Chix line, value, header, name, tmpstr1, tmpstr2, cptr;
   Flag  first_line;
   char bugbuf[200], *strtoken();
   int4 pos;

   ENTRY ("getnextxact", "");

   line     = chxalloc (L(300),  THIN, "getnextx line");
   value    = chxalloc (L(300),  THIN, "getnextx value");
   header   = chxalloc (L(1024), THIN, "getnextx header");
   name     = chxalloc (L(100),  THIN, "getnextx name");
   tmpstr1  = chxalloc (L(100),  THIN, "getnextx tmpstr1");
   tmpstr2  = chxalloc (L(100),  THIN, "getnextx tmpstr2");
   success  = 0;

   clear_xaction (pxaction);

   first_line  = 1;

   /*** Begin reading from UNIT. */
   while ( unit_read (unit, line, L(0)) ) {

      pos = 0;                                  /* Initialize for this line. */
      if (chxvalue(line, L(0)) == L('='))
         { done1=1; break; }                               /* End of Xaction */
      if (chxvalue(line, L(0)) == L('\0'))
         continue;                           /* Ignore blank lines in header */
      else first_line=0;

      chxcat (header, line);                        /* Copy for text version */
      chxcat (header, CQ("\n"));

      chxbreak (line, name, value, CQ("="));
      namenum = tablematch (fieldtable, name) + 1;         /* Get field name */
      if (namenum < 1) {                                      /* Bad xaction */
         sprintf (bugbuf, "GETNEXTXACT: bad field, type=%s, typenum=",
                  ascquick(name));
         bug (bugbuf, namenum);
         break; }           /* Bad xaction. */

      switch (namenum) {
      case FT_TYPE:
         strtoken (pxaction->type, 1, ascquick(value));
         pxaction->typenum = tablematch (typetable, CQ(pxaction->type)) + 1;
         strtoken (pxaction->action, 2, ascquick(value));
         pxaction->actnum = tablematch (actiontable, CQ(pxaction->action)) + 1;
         chxtoken (tmpstr1, null.chx, 3, value);
         pxaction->transit=(tablematch (actiontable, tmpstr1)+1 == XT_TRANSIT);
         break;
      case FT_SUBTYPE:
         strtoken (pxaction->subtype, 1, ascquick(value));
         pxaction->subnum  = tablematch (subtable, CQ(pxaction->subtype)) + 1;
         break;
      case FT_CONF:
         chxbreak (value, pxaction->conftname, pxaction->confochn, CQ("@"));
         pxaction->conflocnum = cnumber (value, TRUECONF, &v);  break;
      case FT_UID:
         chxcpy (pxaction->uid, value);          break;
      case FT_NAME:
         chxcpy (pxaction->uname, value);        break;
      case FT_NEWNAME:
         chxcpy (pxaction->unewname, value);     break;
      case FT_PHONE:
         chxcpy (pxaction->phone, value);        break;
      case FT_IID:
         chxbreak (value, tmpstr1, tmpstr2, CQ("@"));
         cptr = tmpstr1;
         pxaction->itemid = chxint4 (cptr, &pos);
         chxcpy (pxaction->itemchn, tmpstr2);   break;
      case FT_RID:
         chxbreak (value, tmpstr1, tmpstr2, CQ("@"));
         cptr = tmpstr1;
         pxaction->respid = chxint4 (cptr, &pos);
         chxcpy (pxaction->respchn, tmpstr2);   break;
      case FT_RECHOST:
         chxcpy (pxaction->rechost, value);      break;
      case FT_RECID:
         chxcpy (pxaction->recid, value);        break;
      case FT_RECNAME:
         chxcpy (pxaction->recname, value);      break;
      case FT_TITLE:
         chxcpy (pxaction->title, value);        break;
      case FT_DATE:
         chxcpy (pxaction->date, value);         break;
      case FT_MTYPE:
         strtoken (pxaction->mtype, 1, ascquick(value));
         pxaction->mtypenum  = tablematch (messtable, CQ(pxaction->mtype));
         break;
      case FT_M_OPARTS:
         cptr = value; pxaction->m_oparts = (short) chxint4 (cptr, &pos);
         break;
      case FT_RRLEN:
         cptr = value; pxaction->rrlength = (short) chxint4 (cptr, &pos);
         break;
      case FT_MAILCTRL:
         chxtoken (tmpstr1, null.chx, 1, value);
         pxaction->mailctrl  = tablematch (mctrltable, tmpstr1) + 1;
         break;
      case FT_DATERECVD:
         chxcpy (pxaction->daterecvd, value);    break;
      case FT_DATALEN1:
      case FT_DATALEN2:
         which = namenum==FT_DATALEN1 ? 0 : 1;
         cptr = value;
         pxaction->datasize[which] = chxint4 (cptr, &pos);
         break;
      case FT_DATA1:
      case FT_DATA2:
         which = (namenum==FT_DATA1) ? 0 : 1;

         /* Prepare places to put text. */
         if (pxaction->datasize[which] == 0) {
            bug ("GETNEXTXACT: Data without a DataLength.", 0); FAIL; }
         while (unit_read (unit, line, L(0))) {
            if (chxvalue(line, L(0)) == L(';')) break;
            chxcatsub (pxaction->data[which], line, L(1), ALLCHARS);
            chxcat    (pxaction->data[which], CQ("\n"));
         }
         break;
      /* There shouldn't be any blank lines in the header. */

      case FT_TEXTPROP:
         cptr = value; pxaction->textprop = (int) chxint4 (cptr, &pos);
         break;

      default:
       if (first_line) continue;   /* Be forgiving about blank lines between*/
       sprintf (bugbuf, "GETNEXTXACT: got bad field, type=%s, typenum=", 
                         ascquick(name));
       bug (bugbuf, namenum);   break;
      }
   }
   if (first_line || namenum < 1) FAIL;  /* Unit_read failed on EOF. */
   if (NOT done1 || validxact (pxaction)) {
      if (debug>1) printxact (pxaction);
      SUCCEED;
   }
   else {
      bug ("GETNEXTXACT: invalid xaction format.", 0);
      if (debug) printxact (pxaction);
      SUCCEED;   /* Used to FAIL, but caused problems for upward
                 /  compatibility... */
   }
done:
   chxfree (line);
   chxfree (value);
   chxfree (header);
   chxfree (name);
   chxfree (tmpstr1);
   chxfree (tmpstr2);

   RETURN (success);
}

validxact (pxact)
   XACTIONPTR pxact;
{
   if (pxact->typenum < TT_ITEM || pxact->typenum > TT_ROUTES) return (0);
   if (pxact->conflocnum < 1 && pxact->typenum != TT_MESSAGE &&
                                pxact->typenum != TT_SUBREQ  &&
                                pxact->typenum != TT_SUBACK  &&
                                pxact->typenum != TT_SUBNAK  &&
                                pxact->typenum != TT_SUBDEL  &&
                                pxact->typenum != TT_ROUTES  &&
                                pxact->typenum != TT_HOST) return (0);
   return (1);
}

printxact (px)
   XACTIONPTR px;
{
   char temp1[200];
   char temp2[200];

#define NN(x)  (x==NULL ? "" : x)
   printf ("Transaction:\n");
   printf ("   TYPE     = %-8s  (%d) %s\n", NN(px->type), px->typenum,
                                          px->transit ? "TRANSIT" : "");
   printf ("   ACTION   = %-8s  (%d)\n", NN(px->action), px->actnum);
   printf ("   SUBTYPE  = %-8s  (%d)\n", NN(px->subtype), px->subnum);
   printf ("   CONF     = %-20s  (%d)\n", NN(ascquick(px->conftname)),
                                           px->conflocnum);
   printf ("   UID      = %s\n",       NN(ascquick(px->uid)));
   ascofchx (temp1, px->uname, L(0), L(200));
   ascofchx (temp2, px->phone, L(0), L(200));
   printf ("   NAME     = %s   PHONE=%s\n",  NN(temp1), NN(temp2));
   printf ("   IID      = %d@%s\n",    px->itemid, NN(ascquick(px->itemchn)));
   printf ("   RID      = %d@%s\n",    px->respid, NN(ascquick(px->respchn)));
   ascofchx (temp1, px->recid,   L(0), L(200));
   ascofchx (temp2, px->rechost, L(0), L(200));
   printf ("   RECID    = %s@%s\n",    temp1, NN(temp2));
   printf ("   RECNAME  = %s\n",       NN(ascquick(px->recname)));
   printf ("   TITLE    = %s\n",       NN(ascquick(px->title)));
   printf ("   DATE     = %s\n",       NN(ascquick(px->date)));
   printf ("   MTYPE    = %s\n",       NN(px->mtype));
   printf ("   M_OPARTS = %d\n",       px->m_oparts);
   printf ("   RRLEN    = %d\n",       px->rrlength);
   printf ("   MAILCTRL = %d\n",       px->mailctrl);
   printf ("   DATALEN1 = %d\n",       px->datasize[0]);
   printf ("   DATALEN2 = %d\n",       px->datasize[1]);
}


