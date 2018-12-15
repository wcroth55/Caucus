/*** MAKE_TEXTBUF.   Allocate and initialize a textbuffer.
/
/    t = make_textbuf (resps, lines, name);
/
/    Parameters:
/       Textbuf t;       ( Resulting text buffer.)
/       int     resps;   ( Max # of responses or messages per text buffer.)
/       int     lines;   ( Max # of lines, total, in text buffer.)
/       char   *name;    ( name, for debugging)
/
/    Purpose:
/       Create a new, empty text buffer.
/
/    How it works:
/      Calls to make_textbuf usually use the TB_RESPS and TB_LINES constants
/      defined in textbuf.h as the args to make_textbuf().
/
/    Returns: a new Textbuf on success
/             NULL on error
/
/    Error Conditions:
/       Insufficient memory, or the heap is screwed up.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  resp/maketext.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/91 14:17 New function. */
/*: CR 12/11/92 15:56 Long sysmem arg. */
/*: CR  2/28/93 21:32 Initialize new 'lastused' member. */
/*: JV  3/09/93 11:38 Add File Attachment. */
/*: JV  6/06/93 11:52 Add text length info. */
/*: CP  8/01/93 14:22 Add 'name' argument. */
/*: CP  8/12/93 13:08 Make t->attach alloc. array of Attachments, make NULL. */
/*: CK  9/22/93 20:30 Raise initial textbuf line alloc to 92. */
/*: JZ 10/18/93 11:03 Declare strncpy(). */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"

extern int debug;

FUNCTION   Textbuf make_textbuf (resps, lines, name)
   int     resps, lines;
   char   *name;
{
   Textbuf t;
   int     i;
   char   *sysmem(), *strncpy();

   ENTRY ("make_textbuf", "");

   if (resps <  20)  resps =  20;
   if (lines < 100)  lines = 100;

   t = (Textbuf) sysmem (L(sizeof (struct textbuf_t)), "make_textbuf");
   if (t == nulltext)  RETURN (t);

   t->type     = TB_IS_EMPTY;
   strncpy (t->dname, name, 15);  t->dname[15] = '\0';
   t->l_max    = lines;
   t->r_max    = resps;
   t->r_used   = 0;
   t->r_start  = (short *) sysmem (L(sizeof(short)) * L(resps + 1), "r_start");
   t->r_stop   = (short *) sysmem (L(sizeof(short)) * L(resps + 1), "r_stop");
   t->textprop = (short *) sysmem (L(sizeof(short)) * L(resps + 1), "textprop");
#if C_LINK
   t->origresp = (short *) sysmem (L(sizeof(short)) * L(resps + 1), "origresp");
   t->orighost = (Chix *)  sysmem (L(sizeof(Chix))  * L(resps + 1), "orighost");
#endif
   t->line     = (Chix *)  sysmem (L(sizeof(Chix))  * L(lines + 1), "line");
   t->lines    = (int4 *)  sysmem (L(sizeof(int4))  * L(resps + 1), "lines");
   t->bytes    = (int4 *)  sysmem (L(sizeof(int4))  * L(resps + 1), "bytes");
   t->lastused = 0;
   t->copied   = (Chix *)  sysmem (L(sizeof(Chix))  * L(resps + 1), "copied" );
   t->attach   = (Attachment *) sysmem (L(sizeof(Attachment)) *
                                        L(resps+1), "attach");

#if C_LINK
   if (t->orighost == (Chix *)       NULL)   RETURN (nulltext);
#endif
   if (t->attach   == (Attachment *) NULL)   RETURN (nulltext);

   for (i=0;   i<resps;   ++i) {
#if C_LINK
      t->orighost[i] = nullchix;
#endif
      t->attach  [i] = (Attachment) NULL;
      t->copied  [i] = chxalloc (L(0), THIN, "maketext copied");
   }

   if (t->line     == (Chix *) NULL)   RETURN (nulltext);
   for (i=0;   i<lines;   ++i)
      t->line[i] = chxalloc (L(92), THIN, "make_text line");

   if (t->r_start  == (short *) NULL  ||
       t->r_stop   == (short *) NULL  ||
#if C_LINK
       t->origresp == (short *) NULL  ||
       t->orighost == (Chix *)  NULL  ||
#endif
       t->lines    == (int4 *)  NULL  ||
       t->bytes    == (int4 *)  NULL  ||
       t->attach   == (Attachment *) NULL ||
       t->line     == (Chix *)  NULL)      t = nulltext;

   if (debug  &&  t!=nulltext)  mem_debug (MEM_ALLOC, t->dname, "tbuf", L(999));

   RETURN (t);
}
