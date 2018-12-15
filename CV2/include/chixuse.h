
/*** CHIXUSE.H.  Include file for applications that use chix functions. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:31 New file. */
/*: CR  4/30/91 23:40 Split off from old chix.h. */
/*: CR  6/19/91 13:10 Move nullchix def from chixdef.h to chixuse.h */
/*: CR  7/01/91 18:07 Add #ifndef... */
/*: CR  7/10/91 13:09 Make chxresize() a public function. */
/*: CR  7/11/91 12:05 Define EMPTYCHX. */
/*: CR  1/03/92 13:19 Raise ALLCHARS to 32760. */
/*: CR  3/20/92 15:17 Add CHXSUB macro. */
/*: CX  5/21/92 14:23 Add ascquick() decl. */
/*: JX  5/26/92 17:31 Move CQ, NEWLINE here from caucus.h, add NEWCPAGE. */
/*: JX  6/09/92 09:16 Add nullchix to EMPTYCHX. */
/*: JX  7/01/92 15:28 Raise ALLCHARS to 65530. */
/*: CR 11/15/92 23:09 Lower ALLCHARS back to 32760, damn it. */
/*: CR 12/10/92 14:58 Make maxlen member, some return values, long. */
/*: CR 12/11/92 16:22 ALLCHARS is now -1L, has special meaning. */
/*: CR 12/15/92 15:12 ALLCHARS is now -1000L, since -1 may be meaningful. */
/*: CR 12/21/92 14:11 Rearrange so ischix is contiguous with name. */
/*: CR  1/13/93 15:01 Move SIZE from chixdef.h. */
/*: CP  8/01/93 16:02 Add MEM_DEBUG macros. */

#include "int4.h"

#ifndef CHIXUSE
#define CHIXUSE

/*** Currently, we only allow for two sizes of Chix: 1 and 2 bytes. */
#define THIN      1   /* Value must be equal to # of bytes. */
#define WIDE      2   /* Value must be equal to # of bytes. */

/*** A chix is actually a pointer to a structure.  The structure
/    contains a pointer to a dynamically-allocated block of memory,
/    which contains the "string".  The structure also contains other
/    control information about the string. */
typedef unsigned char uchar;
struct chix_t {
   int4    ischix;    /* Magic number to help confirm this is a chix! */
   char    name[16];  /* Name of this string, for debugging. */
   int4    maxlen;    /* Current max length of string data, in characters. */
   uchar  *p;         /* Pointer to string data. */
   int4    actlen;    /* Actual length, used only in chxcat(). */
   short   dic;       /* Dictionary number, if relevant. */
   short   resize;    /* Count of number of resizes of this string. */
   char    type;      /* # of bytes per character in this string. */
   char    expand;    /* Reserved for expansion. */
};

typedef struct chix_t *   Chix;

#define  nullchix     ((Chix) NULL)
#ifndef  L
#define  L(x)         ((int4) x )
#endif

/*** Collating stuff. */
#define  CLEAR_CT      0
#define  ADDRULE_CT    1
#define  APPLY_CT      2

/*** Special character stuff. */
#define  SC_GET        0
#define  SC_SET        1
#define  SC_BLANK      2

/*** Practically infinite number of characters; used when calling
/    chxcatsub(). */
#define  ALLCHARS     ((int4) -1000)

/*** Shorter form of chxalsub() when we always want a substring from
/    position I through the end. */
#define  CHXSUB(a,i)   chxalsub(a,i,ALLCHARS)

/*** "Quickies" */
#define CQ(x)        chxquick (x,      0)
#define NEWLINE      chxquick ("\n",   9)
#define NEWCPAGE     chxquick ("\001", 8)

/*** MEM_DEBUG macros. */
#define MEM_ALLOC    1000
#define MEM_FREED    1001

/*** Is a chix empty, i.e. cleared? */
#define  EMPTYCHX(x)   (x == nullchix || chxvalue (x, (int4) 0) == 0)

/*** Long version of sizeof(). */
#define  SIZE(x)      ( (int4) sizeof(x) )

/*** Prototypes of all "public" (i.e., meant to be used
/    by an application) chix functions. */

int4  ascofchx    (char *s, Chix a, int4 i, int4 n);
char *ascquick    (Chix x);
int   chxabr      (Chix a, Chix b);
Chix  chxalloc    (int4 maxlen, int type, char *name);
Chix  chxalsub    (Chix a, int4 i, int4 n);
int   chxalter    (Chix a, int4 pos, Chix b, Chix c);
int   chxbreak    (Chix a, Chix fore, Chix aft, Chix pat);
int   chxcat      (Chix a, Chix b);
int   chxcatsub   (Chix a, Chix b, int4 i, int4 n);
int   chxcatval   (Chix a, int width, int4 v);
int   chxclear    (Chix x);
int4  chxcollate  (int verb, int4 a, int4 b, int4 c, int4 i);
int   chxcompare  (Chix x, Chix y, int4 len);
int   chxconc     (Chix a, Chix b, Chix c);
int   chxcpy      (Chix a, Chix b);
int   chxeq       (Chix x, Chix y);
int   chxformat   (Chix targ, Chix format, int4 l1, int4 l2, Chix c1, Chix c2);
int   chxfree     (Chix x);
int4  chxindex    (Chix x, int4 pos, Chix y);
int   chx_init    (int  type);
int4  chxlen      (Chix x);
int4  chxint4     (Chix a, int4 *i);
int   chxnextline (Chix a, Chix b, int4 *pos);
int4  chxnextword (Chix word, Chix text, int4 *pos);
int   chxnum      (Chix a, int4 *v);
int   chxofascii  (Chix a, char *str);
int   chxofraw    (Chix a, char *raw, int width, int4 len);
Chix  chxquick    (char *str, int slot);
int   chxreplace  (Chix a, int4 pos, Chix b, Chix c);
int4  chxrevdex   (Chix x, int4 pos, Chix y);
int   chxsetval   (Chix x, int4 pos, int width, int4 val);
int   chxsimplify (Chix x);
int   chxspecial  (int  op, int type, Chix a);
int   chxtoken    (Chix word, Chix rest, int n, Chix line);
int   chxtrim     (Chix x);
int   chxtrunc    (Chix x, int4 pos);
int   chxtype     (Chix x);
int4  chxvalue    (Chix x, int4 n);
int   eucofjix    (char *s, Chix a, int4 i, int4 n);
int   jixofeuc    (Chix a, char *b, int init, char *outseq);
int   jixofsjis   (Chix a, char *b, int init, char *outseq);
int   jixread     (Chix a, int rep);
int   jixreduce   (Chix a);
int   jixscrsize  (Chix a, int maxlen, int *maxchar, int *maxscr);
int   jixwidth    (int4 value);
int   sjisofjix   (char *s, Chix a, int4 i, int4 n);

#endif
