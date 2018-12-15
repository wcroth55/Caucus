/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** UNITGLOBALS.  Definition of data structures global to the UNIT
/    i/o routines. */

/*: AA  7/29/88 17:54 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CC  2/09/01 21:00 Add XSAY. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"
#include "unitcode.h"
#include "unitwipe.h"

int4   time_lock = L(0);     /* Time spent waiting for locks. */

#if UNIX | NUT40 | WNT40
struct locktype_t locktype[LZZZ+1] =
  { { -1, L(0), "", ""},  { -1, L(0), "", ""},  { -1, L(0), "", ""},
    { -1, L(0), "", ""},  { -1, L(0), "", ""},  { -1, L(0), "", ""} };
#endif

struct unit_template units[XLAST+1];

char shouldlock [XLAST+1] = {
   0,   /* XIRE  0   redirection */
   0,   /* XIPT  1   print */
   0,   /* XITX  2   misc text 1 */
   0,   /* XITY  3   misc text 2 */
   0,   /* XICE  4   caucus ed file */
   0,   /* XILM  5   last message sent */
   0,   /* XICF  6   Input Command File */
   0,   /* XISF  7   Input Session File */
   0,   /* XIMF  8   Input Menu File. */
   0,   /* XIBB  9   Output Bit Bucket. */
   0,   /* XIAL 10   "all" input. */
   0,   /* XIOF 11   FILEOUTPUT file. */
   0,   /* 12 */

   1,   /* XURG 13   user registration */
   1,   /* XUMD 14   user mess dir */
   1,   /* XUMF 15   user mess file  */
   1,   /* XUND 16   user sub name dir   */
   1,   /* XUNN 17   user sub names  */
   1,   /* XUSD 18   user subject dir  */
   1,   /* XUSF 19   user subjects  */
   1,   /* XUOD 20   user ordered subs dir  */
   1,   /* XUON 21   user ordered subs  */
   1,   /* XUPA 22   user participation, per conference  */
   1,   /* XUOU 23   only one user  */
   1,   /* XUXF 24   user xaction file */
   0,   /* XUXT 25   user xaction tempfile */
   1,   /* XULG 26   user logfeature file */
   0,   /* XUDD 27   user directory */
   1,   /* XUVA 28   user variables file */
   0,   /* 29 */

   1,   /* XCRD 30   conference response directory  */
   1,   /* XCRF 31   conference response files  */
   1,   /* XCMD 32   membership directory  */
   1,   /* XCMN 33   membership names  */
   1,   /* XCTD 34   item title dir  */
   1,   /* XCTN 35   item title names  */
   1,   /* XCAD 36   author dir  */
   1,   /* XCAN 37   author names  */
   1,   /* XCND 38   sub names dir  */
   1,   /* XCNN 39   sub names  */
   1,   /* XCSD 40   subject dir  */
   1,   /* XCSF 41   subjects  */
   1,   /* XCOD 42   ordered sub names dir  */
   1,   /* XCON 43   ordered sub names  */
   1,   /* XCMA 44   master resp records  */
   1,   /* XCIN 45   introduction to conf  */
   1,   /* XCGR 46   greeting to conf  */
   1,   /* XCUS 47   userlist  */
   0,   /* XCDD 48   conference name */
   1,   /* XCLG 49   by-conference user log file */
   0,   /* XCXT 50   conference xaction file (temp) */
   1,   /* XCNB 51   conference neighbors */
   1,   /* XCTC 52   conference trueconf name */
   0,   /* XCUX 53   conference temp user xactions */
   1,   /* XCED 54   conference item-entry directory. */
   1,   /* XCEN 55   conference item-entry namelist. */
   1,   /* XCFD 56   conference resp-entry directory. */
   1,   /* XCFN 57   conference resp-entry namelist. */
   1,   /* XCVA 58   conference variables file. */
   1,   /* XCUP 59   conference hidden items file. */

   0,   /* XSMD 60   master dictionary */
   1,   /* XSND 61   system user names dir  */
   1,   /* XSNN 62   system user names  */
   1,   /* XSPC 63   system print codes  */
   1,   /* XSMU 64   max users enforcement  */
   1,   /* XSPD 65   password dir  */
   1,   /* XSPN 66   passwords  */
   1,   /* XSBG 67   bug list  */
   1,   /* XSLG 68   log file  */
   1,   /* XSCD 69   conference names dir  */
   1,   /* XSCN 70   conference names  */
   1,   /* XSCK 71   clock check  */
   1,   /* XSMK 72   make-directory locking. */
   1,   /* XSPV 73   server maintenance PriViledges */
   0,   /* XSMO 74   master.opt options file */
   0,   /* XSUG 75   user groups. */
   1,   /* XSPB 76   parsebay files */
   1,   /* XSHD 77   host names dir */
   1,   /* XSHN 78   host names partfile */
   1,   /* XSNF 79   system info */
   1,   /* XSSM 80   shipping methods */
   1,   /* XSSS 81   shipping scripts */
   1,   /* XSRM 82   receiving methods */
   1,   /* XSRS 83   receiving scripts */
   1,   /* XSPL 84   XFP is running */
   1,   /* XSPU 85   XPU is running */
   1,   /* XSRP 86   ReParseBay files */
   1,   /* XSRD 87   Routing Partfile Dir */
   1,   /* XSRN 88   Routing Partfiles */
   1,   /* XSR2 89   2nd ReParseBay file */
   1,   /* XSUP 90   User Pre-Registration file */
   0,   /* XSAM 91   File Attachment Map File */
   1,   /* XSMG 92   Caucus managers */
   1,   /* XSVA 93   site variables */
   1,   /* XSOB 94   object files */
   1,   /* XSOB 95   array files */
   0, 0, 0, 0, /* 96-99 */
   1,   /* XHI1 100   host info */
   1,   /* XHML 101   outbound mail */
   1,   /* XHXA 102   xactions passing thru */
   1,   /* XHI2 103   2nd Host Info */
   1,   /* XHSV 104   host's shipping method variables */
   1,   /* XHSL 105   shipper lock -- only one shipper at a time! */
   0,   /* XHDD 106   host directory */
   1,   /* XHRV 107   host's receiving method variables */
   0,   /* 108 */
   1,   /* XNET 109   "The XFP is running" */
   0, 0, 0,  /* 110, 111, 112, */
   1,   /* XPAK 120  xport files of acks */
   1,   /* XPXH 121  xport header file */
   1,   /* XPXT 122  xport text-file */
   0,   /* XPXJ 123  xport joined-file: only used by one program. */
   1,   /* XPRH 124  xport reship header-file */
   1,   /* XPRT 125  xport text-file */
   1,   /* XPNH 126  cantship header */
   1,   /* XPNT 127  cantship text */
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 128-140 */
   1,   /* XRXC 141  xport file chunks (receive) */
   1,   /* XRRX 142  rereceive (seq# too high) */
   1,   /* XRCC 143  chunk counter */
   1,   /* XRCH 144  chunk history record */
   1,   /* XRRL 145  only one receiver per method at a time! */
   0, 0, 0, 0, 0, /* 146-149 */
   1,  /* XAHMI 150  Audit: Incoming Mail, by Host. */
   1,  /* XAHMO 151  Audit: Outgoing Mail, by Host. */
   1,  /* XAHCI 152  Audit: Incoming Conference material, by Host. */
   1,  /* XAHCO 153  Audit: Outgoing Conference material, by Host. */
   1,  /* XAHPI 154  Audit: Incoming People info, by Host. */
   1,  /* XAHPO 155  Audit: Outgoing People info, by Host. */
   1,  /* XAUCO 156  Audit: Outgoing Conference material, by Userid. */
   1,  /* XAUMI 157  Audit: Incoming Mail, by recipient Userid.. */
   1,  /* XAUMO 158  Audit: Outgoing Mail, by sending Userid. */
   1,  /* XACCI 159  Audit: Incoming Conference material, by Conference. */
   1,  /* XACCO 160  Audit: Outgoing Conferecne material, by Conference. */
   0, 0, 0, 0, 0, 0, 0, 0, 0, /* 161-169 */
   1,  /* XACD  170  Conf directory of File Attachments. */
   1,  /* XAUD  171  User directory of F-A's. */
   1,  /* XARF  172  Response-Type F-A's. */
   1,  /* XAMF  173  Message-Type F-A's. */
};


/*** Unittag: an array of names of the units, only for debugging porpoises. */
char *unittag [XLAST+1] = {
   "XIRE",     /*   redirection */
   "XIPT",     /*   print */
   "XITX",     /*   misc text 1 */
   "XITY",     /*   misc text 2 */
   "XICE",     /*   caucus ed file */
   "XILM",     /*   last message sent */
   "XICF",     /*   Input Command File */
   "XISF",     /*   Input Session File */
   "XIMF",     /*   Input Menu File */
   "XIBB",     /*   Output bit bucket. */
   "XIAL",     /*   "all" input. */
   "XIOF",     /*   FILEOUTPUT file. */
   NULL, /* 12 */

   "XURG",     /*   user registration */
   "XUMD",     /*   user mess dir */
   "XUMF",     /*   user mess file  */
   "XUND",     /*   user sub name dir   */
   "XUNN",     /*   user sub names  */
   "XUSD",     /*   user subject dir  */
   "XUSF",     /*   user subjects  */
   "XUOD",     /*   user ordered subs dir  */
   "XUON",     /*   user ordered subs  */
   "XUPA",     /*   user participation, per conference  */
   "XUOU",     /*   only one user  */
   "XUXF",     /*   user xaction file */
   "XUXT",     /*   user xaction file (temp) */
   "XULG",     /*   user logfeature file */
   "XUDD",     /*   user directory */
   "XUVA",     /*   user variables file. */
    NULL,            /* 29 */

   "XCRD",     /*   conference response directory  */
   "XCRF",     /*   conference response files  */
   "XCMD",     /*   membership directory  */
   "XCMN",     /*   membership names  */
   "XCTD",     /*   item title dir  */
   "XCTN",     /*   item title names  */
   "XCAD",     /*   author dir  */
   "XCAN",     /*   author names  */
   "XCND",     /*   sub names dir  */
   "XCNN",     /*   sub names  */
   "XCSD",     /*   subject dir  */
   "XCSF",     /*   subjects  */
   "XCOD",     /*   ordered sub names dir  */
   "XCON",     /*   ordered sub names  */
   "XCMA",     /*   master resp records  */
   "XCIN",     /*   introduction to conf  */
   "XCGR",     /*   greeting to conf  */
   "XCUS",     /*   userlist  */
   "XCDD",     /*   conference name */
   "XCLG",     /*   49 by-conference user log file. */
   "XCXT",     /*   conf xaction (temp) file */
   "XCNB",     /*   51 conf neighbors */
   "XCTC",     /*   52 trueconf name */
   "XCUX",     /*   53 conference temp user xactions */
   "XCED",     /*   54 conference item-entry directory. */
   "XCEN",     /*   55 conference item-entry namelist. */
   "XCFD",     /*   56 conference resp-entry directory. */
   "XCFN",     /*   57 conference resp-entry namelist. */
   "XCVA",     /*   58 conference variables file. */
   NULL,       /* 59 */

   "XSMD",     /*   60 master dictionary */
   "XSND",     /*   system user names dir  */
   "XSNN",     /*   system user names  */
   "XSPC",     /*   system print codes  */
   "XSMU",     /*   max users enforcement  */
   "XSPD",     /*   password dir  */
   "XSPN",     /*   passwords  */
   "XSBG",     /*   bug list  */
   "XSLG",     /*   log file  */
   "XSCD",     /*   conference names dir  */
   "XSCN",     /*   conference names  */
   "XSCK",     /*   clock check  */
   "XSMK",     /*   make-directory locking. */
   "XSPV",     /*   server maintenance PriViledges */
   "XSMO",     /*   master.opt options file */
   "XSUG",     /*   user groups. */
   "XSPB",     /*   parsebay */
   "XSHD",     /*   host names dir */
   "XSHN",     /*   host name pfiles */
   "XSNF",     /*   system info */
   "XSSM",     /*   shipping methods list */
   "XSSS",     /*   shipping scripts */
   "XSRM",     /*   receiving methods */
   "XSRS",     /*   receiving scripts */
   "XSPL",     /*   XFP is running */
   "XSUL",     /*   XFU is running */
   "XSRP",     /*   ReParseBay files */
   "XSRD",     /*   routing table dir */
   "XSRN",     /*   routing table pfiles */
   "XSR2",     /*   2nd ReParseBay file */
   "XSUP",     /*   User Pre-registration file. */
   "XSAM",     /*   File Attachment Map File. */
   "XSMG",     /*   Caucus Managers */
   "XSVA",     /*   site variables */
   "XSOB",     /*   object files */
   NULL,  /* 95 */
   NULL, NULL, NULL, NULL,        /* 96-99 */
   "XHI1",     /*   host info */
   "XHML",     /*   outbound mail */
   "XHXA",     /*   xactions passing thru */
   "XHI2",     /*   2nd host info */
   "XHSV",     /*   Shipping variables */
   "XHSL",     /*   Shipper Lock */
   "XHDD",     /*   Host Directory */
   "XHRV",     /*   Receiving variables */
   NULL,       /* 108 */
   "XNET",     /*   XFP is running */
   NULL, NULL, NULL, NULL, NULL, /* 110-114 */
   NULL, NULL, NULL, NULL, NULL, /* 115-119 */
   "XPAK",     /*   xport ack */
   "XPXH",     /*   121 xport header-file */
   "XPXT",     /*   122 xport text-file */
   "XPXJ",     /*   xport joined-file */
   "XPRH",     /*   reship header-file */
   "XPRT",     /*   reship text-file */
   "XPNH",     /*   cantship header */
   "XPNT",     /*   127 cantship text */
   NULL, NULL, NULL, NULL, NULL, NULL,       /* 128-133 */
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 134-140 */
   "XRXC",     /*   receive chunk */
   "XRRX",     /*   ReReceive (seq# too high) */
   "XRCC",     /*   Receive Chunk Counter. */
   "XRCH",     /*   Received Chunk History record. */
   "XRRL",     /*   only one receiver per ship method at a time. */
   NULL, NULL, NULL, NULL,    /* 146-149 */
   "XAHMI",    /* Audit: Incoming Mail, by Host. */
   "XAHMO",    /* Audit: Outgoing Mail, by Host. */
   "XAHCI",    /* Audit: Incoming Conference material, by Host. */
   "XAHCO",    /* Audit: Outgoing Conference material, by Host. */
   "XAHPI",    /* Audit: Incoming People info, by Host. */
   "XAHPO",    /* Audit: Outgoing People info, by Host. */
   "XAUCO",    /* Audit: Outgoing Conference material, by Userid. */
   "XAUMI",    /* Audit: Incoming Mail, by recipient Userid.. */
   "XAUMO",    /* Audit: Outgoing Mail, by sending Userid. */
   "XACCI",    /* Audit: Incoming Conference material, by Conference. */
   "XACCO",    /* Audit: Outgoing Conferecne material, by Conference. */
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 161-169 */
   "XACD",     /* Conf directory of File Attachments. */
   "XAUD",     /* User directory of F-A's. */
   "XARF",     /* Response-Type F-A's. */
   "XAMF",     /* Message-Type  F-A's. */
   "XART",     /* Response-Type F-A's, temporary file. */
   "XAMT",     /* Message-Type  F-A's, temporary file. */
};

FUNCTION unitglobals()
{
   ENTRY  ("unitglobals", "");
   RETURN (1);
}
