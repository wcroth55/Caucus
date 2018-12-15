/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** Include file CAUCUS.H for CAUCUS source code.  Defines
/    structure templates, macros, and maximum parameters (as
/    defined constants.)
/
/    Copyright (C) 1984 Camber-Roth.  All rights reserved.  */

/*: AA  7/29/88 17:46 Source code master starting point */
/*: ZZ  7/01/91 23:31 Split things out to separate files, reduce carvmarks! */
/*: CR  7/10/91 16:41 Include address.h. */
/*: CR  7/27/91 16:22 Move (& rename) customize attributes to xaction.h. */
/*: JV  8/13/91 08:14 Include debug.h */
/*: CX 10/15/91 18:07 Add NEWLINE, CQ. */
/*: CX 10/16/91 21:35 Chixify userreg_t. */
/*: CX 11/12/91 13:48 Add FAIL/SUCCEED macros. */
/*: JV 10/28/91 14:59 Add conf.h */
/*: CR 10/30/91 16:35 Add CQ, NEWLINE def'ns for chix use. */
/*: CR 12/26/91 12:24 Include "done.h". */
/*: CR  1/07/91 14:46 Add new members to userreg_t. */
/*: CR  1/09/92 12:35 Remove userreg_t.sysopts, add sysi1. */
/*: JV  1/14/92 14:36 Move NEWPAGE to handicap.h. */
/*: CR  1/20/92 15:45 Add this_template.cnum. */
/*: CX  3/17/92 15:45 Remove non-existent afm.h. */
/*: DE  3/18/92 15:51 Removed redundant SUCCEED/FAIL*/
/*: DE  3/24/92 11:56 Created 'Flag' type for char flags */
/*: CX  5/13/92 10:59 Remove unused japan stuff from flag_template. */
/*: JX  5/26/92 17:32 Move CQ and NEWLINE to chixuse.h. */
/*: CX  6/02/92 13:18 Add termstate_t.lineedit, UREAD... macros. */
/*: CX  6/05/92 14:58 Remove unused flag.infile. */
/*: JX  6/05/92 15:39 Remove unused flag.nolower. */
/*: JX  7/13/92 10:44 Put flag_template.japan back in. */
/*: CR  4/18/92 16:33 Add flag.restrict_user. */
/*: CR  9/04/92 14:13 Add flag.logfeature. */
/*: CI 10/04/92 23:44 Chix Integration. */
/*: JV 10/14/92 18:25 Add TEN_K_OFFSET. */
/*: CR 10/30/92 11:50 Add IS_AFILE for systurn(); eightbit/is_afile to flag_t.*/
/*: CR 11/12/92 17:50 #ifndef CQ and NEWLINE. */
/*: JV  1/06/92 15:29 Add SET_ defines for SETOPTS field. */
/*: JV  5/03/93 15:14 Move termstate_t to sys.h, add sys.h. */
/*: JD  7/26/93 10:20 Add DISKFULL option. */
/*: CN  5/15/93 17:30 Change mess.num to short; remove modbit. */
/*: JJ  7/21/93 10:42 Add this_template.rnum. */
/*: CP  8/17/93 14:08 Add Userreg showattach,terminal members. */
/*: JV  9/07/93 10:53 Add Userreg transfer. */
/*: CK 10/06/93 14:27 Add 'This' typedef. */
/*: CP 12/04/93 13:55 Add flag.underscore */
/*: CR  7/31/95 16:00 Add HANGOUT option. */
/*: CR 10/07/01 15:00 Raise userid length. */

/*** Sub-include other caucus include files. */
#include "handicap.h"    /* Easy-to-read macros. */
#include "maxparms.h"    /* Maximum sizes of things. */
#include "unitcode.h"    /* I/O unit code numbers. */
#include "common.h"      /* Stuff common to server/client model. */
#include "textbuf.h"     /* Templates for globals. */
#include "namelist.h"    /* Namelist template, NLNULL. */
#include "partic.h"      /* Participant record template. */
#include "mess.h"        /* Message type macros. */
#include "goodlint.h"    /* Make Lint happy. */
#include "entryex.h"     /* ENTRY/EXIT macros. */
#include "address.h"     /* Address data type. */
#include "debug.h"
#include "conf.h"
#include "done.h"        /* FAIL and SUCCEED macros. */
#include "sys.h"
#include "logger.h"

/*** Eye-catchers */
#define BOUND(x,a,b) (x<a ? a : (x>b ? b : x))
#define CRLF         "\n"
#ifndef CQ
#define CQ(x)        chxquick (x, 0)
#endif
#ifndef NEWLINE
#define NEWLINE      chxquick ("\n", 9)
#endif

/*** Include code needed only for CaucusLink?  (Which is now
/    presumably obsolete... */
#define C_LINK    0

/*** Function codes for buildline(). */
#define BLINIT   -1
#define BLADD    -2
#define BLFLUSH  -3

/*** Debugging switch codes. */
#define  LOCKREPORT     1

/*** Pointers to UNCATEGORIZED ITEMS category. */
#define  UNCAT       1
#define  UNCATHEAD   2

/*** Values of overint4 line formatting options. */
#define  TRUNCATE    0
#define  DO_WRAP     1
#define  REFORMAT    2

/*** Values of userreg_t.mytext setting. */
#define  LATER       0   /* My text appears NEW, later. */
#define  NOW         1   /* My text appears NEW immediately. */
#define  NEVER       2   /* My text is marked seen immediately. */

/*** Values for var_substitute(). */
#define  COUNT       0   /* Count up size of variable substitutions. */
#define  SUBSTITUTE  1   /* Actually do substitutions. */

/*** Offset to get response partfiles to count 998, 999, A00, A01. */
#define TEN_K_OFFSET 11960

/*** User registration information. */
#define   REG_MAX1     40
#define   REG_MAX2     80
#define   REG_MAX4    160

/* Position of SETtings options within the SETOPTS field: */
#define SET_DEBUG      1
#define SET_SCRS       2
#define SET_SCRW       3
#define SET_FORMAT     4
#define SET_EDIT       5
#define SET_TAB        6
#define SET_PAGE       7
#define SET_PRINT      8
#define SET_VERBOSE    9
#define SET_DICT      10
#define SET_EOT       11
#define SET_EXPWARN   12
#define SET_TEXTENT   13
#define SET_RECEIPT   14
#define SET_MYTEXT    15
#define SET_STARTMEN  16
#define SET_IMPORT    17
#define SET_EXPORT    18
struct userreg_T {
                            /* The first group is stored in the URG file. */
   Chix   name;                   /* User's name. */
   Chix   lastname;               /* Part of name used for alphabetization. */
   Chix   id;                     /* userid */
   Chix   setopts;                /* SET options */
   Chix   sysopts;                /* system dependent SET options */
   Chix   phone;                  /* telephone */
   Chix   laston;                 /* date/time last on. */
   struct namlist_t *briefs;      /* brief introduction. */
   struct namlist_t *print;       /* user's Print code instructions. */

                            /* The rest of these are derived... */
   char   truncate;               /* truncate (1) or reformat text (0) ? */
   Chix   editor;                 /* Default text editor. */
   short  tablength;              /* # spaces tab is converted to */
   char   newpage;                /* What char starts new printer page? */
   char   verbose;                /* Verbose or terse caucus prompts. */
   short  dict;                   /* Which dictionary. */
   Chix   eot;
   char   expwarn;                /* Warn about expiration date? */
   char   texten;              /* TEXT_ENTRY 0=>terminal, 1=>editor, 2=>wwrap */
   short  receiptsize;            /* # lines from orig. message in receipt. */
   char   mytext;                 /* When does my text show up as new? */
   char   read_only;              /* Am I a read-only member of this conf? */
   Chix   startmenu;              /* Name of start-up menu (or OFF). */
   char   import_mail;            /* 0=off, 1=import, 2=copy. */
   char   export_mail;            /* 0=off, 1=export, 2=copy. */
   char   is_organizer;           /* 0=no, 1=has 2ndary organizer perms. */
   short  printcode;              /* 0=default, 1=edit */
   int    debug;                  /* debug value for this user. */
   int    lines;                  /* screensize  */
   int    width;                  /* screenwidth */
   int    sysi1;                  /* System-dependent option: int, #1 */
   int    showattach;             /* 0=no/off, 1=yes/on. */
   Chix   terminal;               /* terminal type name. */
   Chix   transfer;               /* file transfer protocol. */
};
typedef struct userreg_T * Userreg;
#define  nulluser  ( (Userreg) NULL)

/*** User's message list record. */
struct message_t {
   short  highest;             /* Highest message number. */
   short  high_seen;           /* Highest message seen by user. */
   short  num[MAXMESS];        /* Flags for each message. */
};

/*** Screen height & width template. */
struct screen_template {
   short lines;
   short width;
};

/*** Structure for command-line flags. */
struct flag_template {
   int    baud;
   int    print;
   int    loctimeout;
   int    remtimeout;
   short  vcmethod;
   char   login;
   char   escape;
   char   redirect;
   Chix   helpfile;
   char   in_macro;
   char   newuser;
   Chix   pwfile;
   char   userid[MAX_USERID];
   Chix   logfile;
   char   pctype;
   Chix   exitfile;
   Chix   entrfile;
   int    port;         /* For IBM PC: 0=>COM1, 1=>COM2. */
   char   verify;
/* Chix   infile; */
   char   echo;
   char   no1prompt;
   char   editlist;
   char   loop;
   char   expwarn;
   char   logbyconf;
   char   japan;
   Chix   optfile; /* name of options file */
   Chix   autologin;  /* Automatic login from command line. */
   char   oneuser;
   char   fullscreen;  /* fullscreen on or off for VM/CMS. */
   char   editcom;     /* Allow use of EDIT command at AND NOW? */
   char   restrict_user;  /* Restrict commands as if user typed them. */
   char   logfeature;     /* Do by-user feature logging. */
   char   eightbit;       /* user requested full 8-bit term i/o processing. */
   char   iocontrol;      /* I/O is to a file or pipe, not terminal. */
   char   diskfull;       /* Bother to check whether disk is full? */
   Chix   outfile;        /* FILEOUTPUT name. */
   char   underscore;     /* Add underscore to userid? */
   char   hangout;        /* Display output & prompts after hangup signal? */
   Chix   webpasswd;      /* Use web passwd file for captive mode? */
   Chix   sweb;           /* swebd.conf configuration file. */
};

/*** Structure for attribute bits about the current item and response. */
struct this_template {
   short frozen;
   short number;
   short rnum;
   int   cnum;
};
typedef struct this_template * This;

/**********************  CV2 new definitions start here ************/

#define  MSG_NEW   1
#define  MSG_SEE   2
#define  MSG_DEL   4
#define  MSG_MLN   8     /* (Make Like New!) */
#define  MSG_RRR  16     /* Return Receipt Requested. */

#define  OS_ECHO       0x01
#define  CAUCUS_ECHO   0x02
#define  COOK_INPUT    0x04
#define  EIGHT_BIT     0x08
#define  LINE_EDIT     0x10
#define  GET_TERM      0x20
#define  SET_TERM      0x40
#define  IS_AFILE      0x80

/*** Options values for unit_read() 3rd argument. */
#define  UREAD_MAX    0x10000L    /* Read up to N chars. */
#define  UREAD_PFK    0x20000L    /* Turn on PF Key mapping. */

typedef  char   Flag;
