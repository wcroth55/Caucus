/*** API.H.    Definition file for Caucus DB API. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/06/92 12:14 Initial version. */
/*: CR 11/12/92 17:59 Rearrange to compile correctly. */
/*: CR 11/27/92 19:19 Add A_BADARG. */
/*: CR 11/30/92 17:17 Change object Text to Atext. */
/*: CR 11/30/92 23:31 Add decl of a_mak_cstat(). */
/*: CR 12/01/92 15:04 Add decl' of a_mak_pstat(). */
/*: JV 12/03/92 10:51 Add laston, sitevar[] to Person; P_LASTON. */
/*: JV 12/04/92 16:28 Add print to Person; P_PRINT. */
/*: CR 12/03/92 15:28 Add V_GET, V_CLR. */
/*: CR 12/04/92 16:05 Add decl' for a_mak_resp(). */
/*: CR 12/09/92 23:08 Declare a_mak_person(). */
/*: JV 12/14/92 16:38 Declare a_mak_people(). */
/*: CR  1/19/93 13:04 Add license member to Initapi. */
/*: CR  2/12/93 08:22 Add Subj object. */
/*: CR  2/28/93 11:46 Add new V_ verbs; new Initapi 'tbufs', 'masters'. */
/*: JV  2/17/93 17:42 Add support for attachments. */
/*: JV  3/08/93 09:55 Add A_BADATTACH, P_ATTACH, P_RESP. */
/*: JV  6/03/93 10:28 Add T_MESSAGE. */
/*: JV  6/08/93 17:52 Add lines, chars to Atext, P_TITLE to P_RESP. */
/*: JV  6/09/93 11:55 Add terminal member to Initapi. */
/*: CP  5/17/93 21:31 Add P_ISINCE, P_RSINCE, P_AND. */
/*: CP  7/31/93 14:46 Add internal api debugging stuff. */
/*: JV  9/28/93 14:59 Add LIBRARY flag to Conf, P_FLIB property mask. */
/*: JZ 10/18/93 10:45 Declare strncpy. */
/*: CR  3/28/95  7:46 Add Initapi.userid. */
/*: CR  6/16/95 16:12 Add text properties. */
/*: CR  9/06/95 12:25 Add Var def. */
/*: CR 11/02/95 19:25 Change P_ALL -> P_EVERY. */
/*: CR  4/13/00 14:00 Add exists member to Resp. */
/*: CR 10/07/01 15:00 Raise userid length. */

#ifndef API_H
#define API_H

#include "attach.h"
#include "maxparms.h"

/*** This file contains four sections:
/      (A)  Commonly used macros.
/      (B)  Bit-mask definitions of properties or "small" objects.
/      (C)  Return values from "a_...()" functions.
/      (D)  Definitions of properties.
/      (E)  Definitions of larger objects (responses, etc.)
/      (F)  Declarations of a_mak...() functions.
/ */

/*** SECTION A: COMMONLY USED MACROS. ---------------------------*/
#ifndef FUNCTION
#define FUNCTION
#endif
#ifndef NOT
#define NOT       !
#endif

/*** Macros for internal api debugging. */
#define A_SET_NAME(x,y)  strncpy (x, (y==NULL ? "?" : y), 15);  x[15] = '\0';

#define A_ALLOC       100
#define A_FREE        101
#define A_CLOSE       102

#define V_GET   2    /* a_cache/a_cch_tbuf "get"   verb. */
#define V_CLR   3    /* a_cache/a_cch_tbuf "clear" verb. */
#define V_SIZE  4    /*         a_cch_tbuf "set size" verb. */
#define V_FREE  5    /*         a_cch_tbuf "free cache" verb. */


/*** SECTION B: BIT MASK PROPERTY DEFINITIONS. ------------------*/
#define  P_EVERY    0x0FFFFFF
#define  P_TEXT           0x1
#define  P_TITLE          0x2
#define  P_FROZEN         0x4
#define  P_NAME           0x8
#define  P_LASTNM        0x10
#define  P_PHONE         0x20
#define  P_OWNER         0x40
#define  P_GREET         0x80
#define  P_FCHANGE      0x100
#define  P_FADD         0x200
#define  P_FVIS         0x400
#define  P_FCHGSUB      0x800
#define  P_USERLIST    0x1000
#define  P_SETTING     0x2000
#define  P_LASTON      0x4000
#define  P_ATTACH      0x8000
#define  P_PRINT      0x10000
#define  P_SUBJECT    0x20000
#define  P_RESP       P_TEXT | P_NAME | P_OWNER | P_TITLE
#define  P_ISINCE     0x40000
#define  P_RSINCE     0x80000
#define  P_AND       0x100000
#define  P_FLIB      0x200000
#define  P_MEMBER    0x400000
#define  P_BITS      0x800000
#define  P_OVERRIDE 0x1000000


/*** SECTION C: RETURN VALUE FROM A_...() FUNCTIONS. ------------*/
#define  A_OK          0
#define  A_BADPATH     2
#define  A_NOCONF      3
#define  A_NOITEM      4
#define  A_NORESP      5
#define  A_NOREAD      6
#define  A_NOWRITE     7
#define  A_NOTOWNER    8
#define  A_NOTHAW      9
#define  A_NOPERSON   10
#define  A_NOTNEW     11
#define  A_NOTMEMBR   12
#define  A_BADARG     13    /* Null object, or bad tag. */
#define  A_NONE       14    /* No namex's found.  (Fold with A_NOPERSON?) */
#define  A_NOTUNIQ    15
#define  A_NOATTACH   16
#define  A_BADATTACH  17
#define  A_TOOMANY    18    /* Too many users. */
#define  A_DBERR   10000    /* ...plus unitcode number. */

#define  A_WAIT     1000
#define  A_NOWAIT   1001




/*** SECTION D: PROPERTY/SMALL OBJECT DEFINITIONS. --------------*/

typedef  struct Atext_t *  Atext;
struct          Atext_t {
   Chix     data;         /* Actual data of text. */
   int      lines;        /* # lines of text. */
   int      chars;        /* Characters of text. */
                          /* May want a PTF/RTF data type someday. */
};

/*** For "Namelist" object definition. */
#include "namelist.h"

typedef  struct Rlist_t *  Rlist;
struct          Rlist_t {
   short    tag;        /* Object type tag for future use. */
   char     dname[16];  /* object name for debugging. */
   short    i0;         /* 1st  item in range. */
   short    i1;         /* last item in range. */
   short    r0;         /* 1st  resp in range. */
   short    r1;         /* last resp in range. */
   Rlist    next;       /* next range in linked list. */
};

/*** Variable tables object definition. */
typedef  struct  Vartab_t * Vartab;
struct           Vartab_t {
   short    tag;        /* Object type tag for future use. */
   char     dname[16];  /* object name for debugging. */
   int      id1;        /* object id # 1 */
   int      id2;        /* object id # 2 */
   Chix     owner;      /* object id */
   int      max;        /* current max size of arrays. */
   int      used;       /* current # slots used in arrays. */
   Chix    *name;       /* ptr to array of chix names  of variables. */
   Chix    *value;      /* ptr to array of chix values of variables. */
   short   *code;       /* numeric code for each variable. */
};
 
   

/*** SECTION E: DEFINITIONS OF LARGER OBJECTS. ------------------*/
#define T_RESP      100
#define T_CLIST     101
#define T_PERSON    102
#define T_CONF      103
#define T_INIT      104
#define T_CSTAT     105
#define T_PSTAT     106
#define T_PEOPLE    107
#define T_SUBJ      108
#define T_NAMEX     109
#define T_ATTACH    110
#define T_MESSAGE   111
#define T_RLIST     112
#define T_VAR       113
#define T_VARTAB    114

/*** Generic API object, used when we need to map headers of all possible
/    API objects into one type. */
typedef  struct Ageneric_t *  Ageneric;
struct          Ageneric_t {
   short    tag;          /* Object type tag for future use? */
   char     dname[16];    /* object name for debugging. */
};


typedef  struct Resp_t *  Resp;
struct          Resp_t {
   short    tag;          /* Object type tag for future use. */
   char     dname[16];    /* object name for debugging. */

                     /* Identifiers: */
   short    cnum;         /* conference number */
   short    inum;         /* item number */
   short    rnum;         /* response number */

                     /* Selectable properties: */
   Atext    text;         /* text of response */
   char     frozen;       /* 0=>thawed, 1=>frozen, 2=>frozen solid */
   Chix     title;        /* title of item == resp 0 */
   Attachment attach;     /* File Attachment */

                     /* These properties filled-in/used at function's whim. */
   Chix     author;       /* name of author */
   Chix     date;         /* date/time string */
   Chix     owner;        /* owner userid [@host?] */
   Chix     copied;       /* where response was copied from */
   int      textprop;     /* text property */
   int      bits;         /* other miscellaneous mask bits. */
   int      lastcall;     /* time of last a_get_resp */
   char     exists;       /* does the response exist? */

   char     ready;        /* asynchronous event complete? */
};
#define MA_TEXT  0x00F    /* mask for bits in textprop */
#define MA_BITS  0xFF0    /* mask for bits in 'bits'.  */


typedef struct Person_t *  Person;
struct         Person_t {
   short    tag;          /* object type tag for future use. */
   char     dname[16];    /* object name for debugging. */

                     /* Identifiers: */
   Chix     owner;       /* userid of person. */

                     /* Selectable properties: */
   Chix     name;         /* Person's full name. */
   Chix     lastname;     /* "last" name, for alpha'ing in membr files. */
   Chix     phone;        /* telephone. */
   Atext    intro;        /* Brief introduction. */
   Chix     setopts;      /* User settings. */
   Chix     laston;       /* Last on Caucus. */
   Atext    print;        /* Print instructions. */
   Chix     sitevar[10];  /* User Variables. */
   char     ready;        /* asynchronous event complete? */
};


typedef  struct Conf_t *  Conf;
struct          Conf_t {
   short    tag;     /* object type tag for future use. */
   char     dname[16];    /* object name for debugging. */

   short    cnum;    /* Identifier. */

                     /* Selectable properties: */
   Chix     lname;       /* local name of conference .*/
   Chix     owner;       /* userid of primary organizer. */
   Atext    intro;       /* introduction to purpose of conference. */
   Atext    greet;       /* greeting text for conference. */
   Atext    userlist;    /* userlist from CUS USER.  ??? */
   char     change;      /* can users change their responses? */
   char     add;         /* can users add new items? */
   char     visible;     /* is conference visible or invisible? */
   char     library;     /* 0=conference, 1=library. */
   char     subject;     /* can users change OSUBJECTS? */
   char     attach;      /* can users add file attachmets? */
   char     ready;    /* asynchronous event complete? */
};


typedef  struct Initapi_t *  Initapi;
struct          Initapi_t {
   short    tag;      /* object type tag for future use. */
   char     dname[16];    /* object name for debugging. */

   Chix     confid;      /* caller must  fill in 'confid' pathname. */
   int      tbufs;       /* caller *may* fill in # of tbufs to cache. */
   int      masters;     /* caller *may* fill in # of Master's to cache. */
   char     terminal;    /* caller must set to 1 if a text terminal. */
   char     userid[MAX_USERID+2]; /* caller may identify effective Caucus user*/
   int      diskform;    /* caller may set language (character set). */

   struct   license_t *license;  /* a_init() fills this in. */
};
#define  LICE_OK        0    /* License is OK. */
#define  LICE_EXPIRED   1    /* License has expired. */
#define  LICE_BADCLOCK  2    /* Somebody set the clock back! */
#define  LICE_BADSYSID  3    /* Bad system id. */


typedef  struct Cstat_t *  Cstat;
struct          Cstat_t {
   short    tag;     /* object type tag for future use. */
   char     dname[16];    /* object name for debugging. */
   short    cnum;    /* Identifier. */
   Chix     owner;        /* primary organizer. */
   short    items;        /* Highest numbered item in conf. */
   short   *resps;        /* # resps in each item; -1 => deleted. */
   char     ready;   /* asynchronous event complete? */
   long     time;    /* age of this data */
};


typedef  struct Pstat_t *  Pstat;
struct          Pstat_t {
   short    tag;     /* object type tag for future use. */
   char     dname[16];    /* object name for debugging. */

   short    cnum;    /* Identifier. */
   Chix     owner;

   short    items;      /* Highest item seen. */
   short   *resps;      /* # responses seen on each; -1 none; -2 forgot. */
   short   *back;       /* # resps to go back for SET MYTEXT LATER. */
   Chix     lastin;     /* last time in this conf. */
   char     ready;   /* asynchronous event complete? */
};


typedef  struct People_t *  People;
struct          People_t {
   short    tag;     /* object type tag for future use. */
   char     dname[16];    /* object name for debugging. */

                     /* Identifiers. */
   Chix     namepart;    /* Partial name. */
   short    cnum;        /* Member of this conf, or 0 for any. */

   Namelist ids;         /* Resulting list of userids. */
   char     ready;   /* asynchronous event complete? */
};

typedef  struct Subj_t *  Subj;
struct          Subj_t {
   short    tag;     /* object type tag for future use. */
   char     dname[16];    /* object name for debugging. */

                     /* Identifiers. */
   Chix     namepart;    /* Partial name. */
   short    cnum;        /* Conference number. */
   Chix     owner;       /* userid of owner, if psubject. */

   Namelist names;       /* Resulting list of userids. */
   char     ready;   /* asynchronous event complete? */
};

typedef  struct Namex_t *  Namex;
struct          Namex_t {
   short    tag;     /* object type tag for future use. */
   char     dname[16];    /* object name for debugging. */

                     /* Identifiers. */
   Chix     namepart;    /* Partial name. */
   short    cnum;        /* Conference number. */
   Chix     owner;       /* userid of owner, if psubject. */

   Rlist    rlist;       /* Resulting item/response list. */
   char     ready;   /* asynchronous event complete? */
};


typedef  struct Var_t *  Var;
struct          Var_t {
   short    tag;     /* object type tag for future use. */
   char     dname[16];    /* object name for debugging. */

                     /* Identifiers. */
   short    cnum;        /* Conference number, if conf var. */
   Chix     owner;       /* userid of owner, if user var. */
   Chix     vname;       /* variable name. */

   Chix     value;   /* value of variable. */
   char     ready;   /* asynchronous event complete? */
};


/*** SECTION F: DECLARATIONS OF A_MAK...() FUNCTIONS. -----------*/
Initapi  a_mak_init();
Conf     a_mak_conf();
Atext    a_mak_text();
Cstat    a_mak_cstat();
Pstat    a_mak_pstat();
Person   a_mak_person();
People   a_mak_people();
Resp     a_mak_resp();
Subj     a_mak_subj();
Namex    a_mak_namex();
Rlist    a_mak_rlist(), and_rlist(), rl_of_select();
Var      a_mak_var();
Vartab   a_mak_vartab();

char    *sysmem();

#endif

