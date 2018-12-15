/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** PUTNEXTXACT.  Writes a xaction onto the end of a xaction file.
/
/    int n = putnextxact (unit, xptr, cdata[])
/
/    Parameters:
/       int unit;
/       XACTHEADPTR xptr;
/       Chix cdata[];
/
/    Purpose:
/       This function writes a xaction onto the end of a xaction file.
/       It gets the data for the header from the XACTION structure.
/       It gets the text of the xaction from the CDATA structures.
/
/    How it works:
/       Putnextxact first validates the xaction, in an attempt to further
/       ensure that there aren't bad xactions floating out on the net.
/       Then it writes out appropriate header fields and data fields.
/
/       The caller does not have to fill in the text of the xaction fields
/       which are also represented by numbers, e.g. {TYPE, TYPENUM},
/       and {ACTION, ACTNUM}.  Putnextxact() will translate the numeric
/       fields into the appropriate text.
/       All text fields which are not used *MUST* be set to empty strings.
/
/       There are two fields which require a bit more processing.
/       This are the DATA fields, each of which must be 'announced'
/       by a DATALENgth field.  The DATALEN fields give us an upper
/       bound on the size of the DATA field.  We malloc enough memory
/       for this and then read the text of DATA into it.
/
/       putnextxact() uses DATASIZE[n] to determine if there is any text
/       in either of the DATATEXT fields.  If DATASIZE is >0, putnextxact
/       writes out the text stored in CDATA[n].
/
/ ***   WARNING: the caller had *better* declare enough empty Chixes,
/       if necessary, to complete the array of them in CDATA.  "Complete"
/ ***   means that there *must* be TEXTFIELDS Chixes in the array.
/
/       putnextxact() assumes that UNIT has been unit_lock'd and
/       unit_append'd.  It also assumes that the caller will unit_close
/       and unit_unlk the unit.  Finally it assumes that the CDATA chixes
/       will be freed by the caller.
/    
/   CHIX warning: putnextxact uses the characters '=' & ';' directly.
/       It also compares strings against '\0' directly.
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/      Can't write to UNIT.
/      Bad format for a xaction.
/
/   Related functions: validxact(), getnextxact()
/
/   Called by: 
/
/   Operating System dependencies: none
/
/   Home: xaction/putnextx.c
/ */

/*: JV  7/08/91 14:22 Create function. */
/*: JV  7/12/91 15:08 Add MESSTABLE[], MTYPE. */
/*: JV  7/15/91 18:22 Add multiple text fields. */
/*: JV  7/17/91 11:46 Fix previous. */
/*: CR  7/27/91 13:39 Declare strtoken(). */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: JV  9/30/91 15:00 Change end-of-transaction marker from '.' to ';'. */
/*: JV 11/06/91 10:29 Change common.h to null.h, add chixuse.h. */
/*: JV 11/12/91 10:14 Add confochn to CONF */
/*: JV 11/22/91 17:28 Fix mtypenum. */
/*: JV 12/07/91 19:55 Declare chxalloc. */
/*: DE  6/05/92 11:23 Chixified */
/*: JX  9/29/92 15:43 Fixify. */
/*: CR 10/21/92 15:22 Remove nested comments. */
/*: CL 12/11/92 11:18 Long Chxcatsub args. */
/*: CL 12/11/92 13:48 Long Chxvalue N. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"
#include "handicap.h"
#include "goodlint.h"
#include "null.h"
#include "xaction.h"
#include "chixuse.h"

#define MAXSTR 160

extern union null_t null;
extern Chix fieldtable,
            typetable,
            actiontable,
            subtable,
            messtable,
            mctrltable;
extern int  debug;

FUNCTION putnextxact (unit, xptr, cdata)
   int  unit;
   XACTIONPTR xptr;
   Chix cdata[];
{
   Chix   tmpbuf, templine, templine2, token1, token2, dataname;
   int    i;
   Chix   cline;
   int4   pos;
   char  *strtoken();

   ENTRY ("putnextxact", "");

   if (NOT validxact (xptr)) {
      bug ("PUTNEXTXACT: invalid xaction.", 0);  RETURN (0); }

   tmpbuf     = chxalloc (L(160), THIN, "putnextx tempbuf");
   templine   = chxalloc (L(MAXSTR), THIN, "putnextx templine");
   templine2  = chxalloc (L(MAXSTR), THIN, "putnextx templine2");
   token1     = chxalloc (L(40),  THIN, "putnextx token1");
   token2     = chxalloc (L(40),  THIN, "putnextx token2");
   dataname   = chxalloc (L(20),  THIN, "putnextx dataname");

   chxtoken (token1, null.chx, xptr->typenum, typetable);
   chxtoken (token2, null.chx, xptr->actnum,  actiontable);
   chxclear  (tmpbuf);
   chxformat (tmpbuf, CQ("TYPE=%s %s "), L(0), L(0), token1, token2);
   chxformat (tmpbuf, CQ("%s\n"), L(0), L(0), 
              xptr->transit ? chxquick("TRANSIT",1) : null.chx, null.chx);
   unit_write (unit, tmpbuf);

   if (xptr->subnum > 0) {
      chxtoken (token1, null.chx, xptr->subnum, subtable);
      chxclear (tmpbuf);
      chxformat (tmpbuf, CQ("SUBTYPE=%s\n"), L(0), L(0), token1, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->conftname,0L) != L('\0')) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("CONF=%s@%s\n"), L(0), L(0), xptr->conftname, 
                 xptr->confochn);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->uid,0L) != L('\0')) {
      chxclear   (tmpbuf);
      chxformat  (tmpbuf, CQ("UID=%s\n"), L(0), L(0), xptr->uid, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->uname,0L) != L('\0')) {
      chxclear   (tmpbuf);
      chxformat  (tmpbuf, CQ("NAME=%s\n"), L(0), L(0), xptr->uname, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->unewname, L(0)) != L('\0')) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("NEWNAME=%s\n"), L(0), L(0), xptr->unewname, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->phone, L(0)) != L('\0')) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("PHONE=%s\n"), L(0), L(0), xptr->phone, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (xptr->itemid > 0) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("IID=%d@%s\n"), xptr->itemid, L(0), xptr->itemchn,
                 null.chx);
      unit_write (unit, tmpbuf);
   }
   if (xptr->respid > 0) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("RID=%d@%s\n"), xptr->respid, L(0), xptr->respchn,
                 null.chx);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->rechost, L(0)) != L('\0')) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("RECHOST=%s\n"), L(0), L(0), xptr->rechost, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->recid, L(0)) != L('\0')) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("RECID=%s\n"), L(0), L(0), xptr->recid, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->recname, L(0)) != L('\0')) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("RECNAME=%s\n"), L(0), L(0), xptr->recname, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->title, L(0)) != L('\0')) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("TITLE=%s\n"), L(0), L(0), xptr->title, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->date, L(0)) != L('\0')) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("XACTDATE=%s\n"), L(0), L(0), xptr->date, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (xptr->mtypenum >= 0) {
      chxtoken  (token1, null.chx, xptr->mtypenum+1, messtable);
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("MESSTYPE=%s\n"), L(0), L(0), token1, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (xptr->m_oparts > 0) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("O_PARTS=%d\n"), L(xptr->m_oparts), L(0), null.chx,
                 null.chx);
      unit_write (unit, tmpbuf);
   }
   if (xptr->rrlength > -1) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("RRLEN=%d\n"), L(xptr->rrlength), L(0), null.chx, 
                 null.chx);
      unit_write (unit, tmpbuf);
   }
   if (xptr->mailctrl != 0) {
      chxtoken (token1, null.chx, xptr->mailctrl, mctrltable);
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("MAILCTRL=%s\n"), L(0), L(0), token1, null.chx);
      unit_write (unit, tmpbuf);
   }
   if (chxvalue (xptr->daterecvd, L(0)) != L('\0')) {
      chxclear  (tmpbuf);
      chxformat (tmpbuf, CQ("RECVDATE=%s\n"), L(0), L(0), xptr->daterecvd, 
                 null.chx);
      unit_write (unit, tmpbuf);
   }
   for (i=0; i<TEXTFIELDS; i++)
      if (xptr->datasize[i] > 0) {
         chxclear  (tmpbuf);
         if (xptr->textprop > 0)
            chxformat (tmpbuf, CQ("TEXTPROP=%d\n"), L(xptr->textprop), L(0),
                               null.chx, null.chx);
         chxformat (tmpbuf, CQ("DATALEN%d=%d\n"), L(i+1), 
                    L(xptr->datasize[i]), null.chx, null.chx);
         unit_write (unit, tmpbuf);

         chxclear  (dataname);
         chxformat (dataname, CQ("DATATEXT%d=\n"), L(i+1), L(0), null.chx, 
                    null.chx);
         unit_write (unit, dataname);
         cline = chxalloc ((int4) MAXSTR, THIN, "putnextx cline");
         for (pos=0;   chxnextline (cline, cdata[i], &pos);  ) {
            chxclear  (templine);
            chxcatsub (templine, cline, L(0), L(MAXSTR));
            chxclear  (templine2);
            chxformat (templine2, CQ(" %s\n"), L(0), L(0), templine, null.chx);
            unit_write (unit, templine2);
         }
         unit_write (unit, CQ(";\n"));           /* terminate data field */
         chxfree (cline);
      }
   unit_write (unit, CQ("=\n"));

   chxfree ( tmpbuf );
   chxfree ( templine );
   chxfree ( templine2 );
   chxfree ( token1 );
   chxfree ( token2 );
   chxfree ( dataname );

   RETURN (1);
}
