/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** TEXTBUF.H.   Definitions of structures used to define the global
/    "text buffer" structures -- the ones that contain Caucus text
/    partfiles. */

/*: JV  4/01/91 15:48 Move text_t, line_t, block_t from caucus.h. */
/*: JV  5/13/91 17:47 Add global redundancy checking. */
/*: JV  5/15/91 16:06 Create PartFileBuffer struct. */
/*: JV  5/16/91 12:27 Add SYSxxxx defines. */
/*: CR  6/20/91 20:52 block_template: new fields origresp[], orighost[]. */
/*: CR  7/01/91 21:03 Rename from globals.h. */
/*: JV  7/02/91 17:14 Add chixuse.h. */
/*: CX 10/15/91 14:10 Major redefinition of textbuf, for chixification. */
/*: JX  3/09/92 10:10 Note: PFILEBUF was turned into Textbuf in 10/91. */
/*: CR 12/07/92 13:29 Add cnum member. */
/*: CR  2/27/93 16:57 Add 'lastused' member. */
/*: JV  3/03/93 11:47 Add attachment. */
/*: JV  6/05/93 22:54 Add length fields: lines & bytes. */
/*: CP  8/01/93 14:16 Add dname member to textbuf. */
/*: CR  6/16/95 16:10 Add tbuf->textprop array. */

#ifndef TEXTBUF_H
#define TEXTBUF_H

#include "chixuse.h"
#include "attach.h"

/*** Types of text contained by BLOCK.TYPE. */
#define TB_IS_UNALLOC  -2
#define TB_IS_EMPTY    -1

#define TB_CHARS  8000     /* Recommended # of chars in a textbuf. */
#define TB_LINES   500     /* Recommended # of lines in a textbuf. */
#define TB_RESPS   200     /* Recommended # of resps/mess in a textbuf. */

struct textbuf_t {
   short  type;       /* Unitcode of contained partfile, or TB_IS_... code. */
   short  item;       /* If an item, what item number. */
   short  r_max;      /* Max # of responses/messages this textbuf can hold. */
   short  r_used;     /* # of above currently in use. */
   short  r_first;    /* Number of first response/message in textbuf. */
   short  r_last;     /* Number of last  response/message in textbuf. */
   short *r_start;    /* Index to LINEs of first line of response. */
   short *r_stop;     /* Index to LINEs of last  line of response. */
   short  firstline;  /* # of first line in first response/mess. */
   short  lastline;   /* # of last  line in last  response/mess. */
#if C_LINK
   short *origresp;   /* For each resp, resp # on original host. */
   Chix  *orighost;   /* For each resp, name  of  original host. */
#endif
   short *textprop;   /* Bit mask of text properties. */
   Attachment *attach;/* For each resp, info about attachment. */
   short  l_max;      /* Max # of lines textbuf can hold. */
   short  l_used;     /* # of above currently in use. */
   Chix  *line;       /* Array of lines in text buffer. */
   short  cnum;       /* If conference text, what conference number? */
   int4   lastused;   /* Time last used, for caching info. */
   int4  *lines;      /* Array of number of lines in a resp/mess. */
   int4  *bytes;      /* Array or number of bytes in a resp/mess. */
   Chix  *copied;     /* Where this response was copied from, by whom */
   char   dname [16]; /* Name for debugging. */
};
   
typedef struct textbuf_t * Textbuf;

#define nulltext  ((Textbuf) NULL)

#endif
