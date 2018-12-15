/*** UNITCODE.H.   Caucus unit i/o code numbers. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:46 Source code master starting point */
/*: CR  1/31/89 14:34 Add XCLG for by-conference user logging. */
/*: CR  8/09/89 15:15 Add XSMK for sysmkdir() locking. */
/*: CW  5/05/89 16:13 Add XSPV for VM. */
/*: CW  5/17/89 15:35 Add new window unit code numbers. */
/*: CR 10/20/89 14:14 Add XWTY for output w/o <CANCEL>ing! */
/*: CR 11/14/89 17:17 Add XSMO. */
/*: CR  2/22/90 15:55 Add XWPT. */
/*: CR  3/22/90 16:11 Add XWTZ. */
/*: CR  7/16/90  7:56 Add XSUG unit. */
/*: CR  2/28/91 21:36 Add bit bucket XIBB. */
/*: JV  4/11/91 14:34 Add CaucusNet units XH**, XP**, XR**. */
/*: JV  4/11/91 14:41 Add CaucusNet units XSPB XSHD/N XSSM XSRM XSPL XSPU. */
/*: JV  4/11/91 14:43 Add CaucusNet units XUXF, XUXT, XCXT, XCNB. */
/*: JV  4/11/91 14:45 Warning: the previous 3 MOVED some unit #'s! */
/*: JV  4/11/91 15:01 Add XPXH, XPXT. */
/*: JV  5/23/91 17:35 Add XCTC, chg XHST->XHI1, XHI2. */
/*: JV  6/14/91 15:01 Fix Net Numbers. */
/*: CR  7/01/91 20:21 Add #ifndef test. */
/*: JV  7/15/91 18:55 Add XSRP, remove XRRP */
/*: CR  7/31/91 17:48 Add XHSV. */
/*: CR  8/02/91 17:35 Add XHSL. */
/*: CR  8/08/91 22:12 Split XPNO -> XPNH, XPNT. */
/*: CR  8/19/91 22:20 Add XRCC, XRCH, XRRL. */
/*: CR  9/05/91 15:43 Remove XPXF, XPXC, XPXR, XRXF. */
/*: JV 10/10/91 10:41 Add XSRD, XSRN. */
/*: JV 10/14/91 12:07 Add XSR2, the 2nd XSRP file. */
/*: JV 10/17/91 12:45 Add Audit units (XAxxx). */
/*: JV 12/10/91 18:10 Add XHDD. */
/*: JV  1/20/92 11:26 Add XCUX. */
/*: CX  6/05/92 11:43 Add XISF. */
/*: JX  8/05/92 09:52 Add XSUP. Renumbered everything after 89. */
/*: CR  6/09/92 14:11 Add XIMF. */
/*: CR  9/04/92 14:08 Add XULG. */
/*: CI 10/04/92 23:34 Chix Integration. */
/*: TP 10/14/92 17:28 Add XIAL, renumber XURG -> XULG. */
/*: CR 10/30/92 14:43 Add XIOF. */
/*: JV 10/26/92 14:35 Add XHRV. */
/*: JV  2/26/93 22:42 Add File Attachment Codes XACD, XAUD, XARF, XAMF. */
/*: JV  3/03/93 22:42 Add XSAM. */
/*: JV  3/26/93 13:42 Add XUDD. */
/*: JV  5/24/93 14:32 Add XART, XAMT. */
/*: CP  4/14/92 15:14 Add MAXLNUM. */
/*: CP  5/06/92 10:46 Add XCED, XCEN, XCFD, XCFN. */
/*: CR  6/28/95 18:15 Add variables files. */
/*: CC  2/09/01 21:00 Add XSAY. */
/*: CR  8/20/04 Add XCUP. */

#ifndef UNITCODE_H
#define UNITCODE_H

/*** I/O related unit numbers (specific to each user). */
#define  XKEY -100  /* Keyboard! */
#define  XWTZ -11   /* Same as XWTX, but does NOT go to XITT! */
#define  XWPT -10   /* Real prompt line for VM. */
#define  XWTY  -9   /* Same as XWTX, but <CANCEL> does not interrupt! */
#define  XWAN  -8   /* Window for announcements. */
#define  XWDB  -7   /* Window for debugging. */
#define  XWER  -6   /* Window for errors/announcements. */
#define  XWPM  -5   /* Window for prompt. */
#define  XWPF  -4   /* Window for pfKeys. */
#define  XWHL  -3   /* Window for help. */
#define  XWTX  -2   /* Window for text. */

#define  XITT  -1   /* Terminal. */
#define  XIRE   0   /* Redirection (> >> <) file */
#define  XIPT   1   /* accumulated PRINT file */
#define  XITX   2   /* temporary TeXt file #1 */
#define  XITY   3   /* temporary Text file #2 */
#define  XICE   4   /* Caucus Editor file. */
#define  XILM   5   /* Last Message sent */
#define  XICF   6   /* Input Command File: AND NOW? <comfile */
#define  XISF   7   /* Input Session File: caucus_x -i file  */
#define  XIMF   8   /* Input Menu File. */
#define  XIBB   9   /* Bit Bucket.  Output gets thrown away. */
#define  XIAL  10   /* "All" input. */
#define  XIOF  11   /* FILEOUTPUT option file. */

/*** User file unit numbers. */
#define  XURG  13   /* Registration file. */
#define  XUMD  14   /* User's mail directory. */
#define  XUMF  15   /* User's mail partfile. */
#define  XUND  16   /* subject Names directory. */
#define  XUNN  17   /* subject Names. */
#define  XUSD  18   /* Subject definitions directory. */
#define  XUSF  19   /* Subject definitions. */
#define  XUOD  20   /* Ordered (alphabetized) subject namelist directory. */
#define  XUON  21   /* Ordered subject namelist. */
#define  XUPA  22   /* PArticipation file for each conference for this user. */
#define  XUOU  23   /* One user per userid per conference. */
#define  XUXF  24   /* User Xaction File. */
#define  XUXT  25   /* User Xaction file (Temporary). */
#define  XULG  26   /* User feature log file. */
#define  XUDD  27   /* User Directory. */
#define  XUVA  28   /* User variables file. */

/*** Conference file unit numbers. */
#define  XCRD  30   /* Response Directory. */
#define  XCRF  31   /* Response partFile. */
#define  XCMD  32   /* Membership partfile directory. */
#define  XCMN  33   /* Membership partfile. */
#define  XCTD  34   /* Title partfile directory. */
#define  XCTN  35   /* Title partfile. */
#define  XCAD  36   /* Author of items partfile directory. */
#define  XCAN  37   /* Author of items partfiles. */
#define  XCND  38   /* subject Names directory. */
#define  XCNN  39   /* subject Names. */
#define  XCSD  40   /* Subject directory. */
#define  XCSF  41   /* Subject textfile(s). */
#define  XCOD  42   /* Ordered (alphabetized) subject names directory. */
#define  XCON  43   /* Ordered subject names. */
#define  XCMA  44   /* MAster list of responses to items. */
#define  XCIN  45   /* Introduction */
#define  XCGR  46   /* Greeting. */
#define  XCUS  47   /* Userlist */
#define  XCDD  48   /* Conference directory. */
#define  XCLG  49   /* Conference log file. */
#define  XCXT  50   /* Conference XacTion (Temporary) file. */
#define  XCNB  51   /* Conference NeighBors. */
#define  XCTC  52   /* Conference TrueConf name. */
#define  XCUX  53   /* Temporary User Xactions for a Conference. */
#define  XCED  54   /* item time Entered: names directory. */
#define  XCEN  55   /* item time Entered: names file. */
#define  XCFD  56   /* resp time entered: names directory. */
#define  XCFN  57   /* resp time entered: names file. */
#define  XCVA  58   /* conference variables file. */
#define  XCUP  59   /* conference "hidden items" file, like XUPA */


/*** System-wide file unit numbers. */
#define  XSMD  60   /* master Dictionary. */
#define  XSND  61   /* Name Directory */
#define  XSNN  62   /* Name File. */
#define  XSPC  63   /* Printcodes. */
#define  XSMU  64   /* Maximum Users control */
#define  XSPD  65   /* Password Directory */
#define  XSPN  66   /* Password partFiles */
#define  XSBG  67   /* Buglist file. */
#define  XSLG  68   /* LoG     file. */
#define  XSCD  69   /* Conference names directory. */
#define  XSCN  70   /* Conference names partfile. */
#define  XSCK  71   /* Clock Check */
#define  XSMK  72   /* Make-directory lock. */
#define  XSPV  73   /* VM Privilege file */
#define  XSMO  74   /* Master.opt options file. */
#define  XSUG  75   /* User groups--for '<'ing in XCUS. */
#define  XSPB  76   /* ParseBay files. */
#define  XSHD  77   /* Host names Directory. */
#define  XSHN  78   /* Host Names parfile. */
#define  XSNF  79   /* System iNFo. */
#define  XSSM  80   /* Shipping Method list. */
#define  XSSS  81   /* Shipping Scripts. */
#define  XSRM  82   /* Receiving Methods list. */
#define  XSRS  83   /* Receiving Scripts. */
#define  XSPL  84   /* XFP is running (Lockfile) */
#define  XSUL  85   /* XFU is running (Lockfile) */
#define  XSRP  86   /* ReParseBay */
#define  XSRD  87   /* Routing table names directory */
#define  XSRN  88   /* Routing table names partfile */
#define  XSR2  89   /* 2nd ReParseBay file */
#define  XSUP  90   /* User Pre-registration file. */
#define  XSAM  91   /* File Attachment Map file */
#define  XSMG  92   /* Caucus managers */
#define  XSVA  93   /* Site variables  */
#define  XSOB  94   /* Object files. */
#define  XSAY  95   /* Array files. */

#define  XHI1  100  /* general Host Info. */
#define  XHML  101  /* outbound MaiL for this Host. */
#define  XHXA  102  /* XActions from a Host and/or passing thru. */
#define  XHI2  103  /* 2nd HostInfo. */
#define  XHSV  104  /* shipping method variables for this host. */
#define  XHSL  105  /* only one shipper at a time shipping to this host! */
#define  XHDD  106  /* Host Directory. */
#define  XHRV  107  /* receiving method variables for this host */

#define  XPAK  120  /* xport file of AcKs and naks in a shiPping bay. */
#define  XPXH  121  /* Xport Header-file. */
#define  XPXT  122  /* Xport Text-file. */
#define  XPXJ  123  /* Joined XPXH & XPXF. */
#define  XPRH  124  /* Reship Header-file. */
#define  XPRT  125  /* Reship Text-file. */
#define  XPNH  126  /* CantShip header. */
#define  XPNT  127  /* CantShip text file. */

#define  XRXC  141  /* Xport file Chunk   (Receiving bay). */
#define  XRRX  142  /* ReReceive (seq# too high). */
#define  XRCC  143  /* Receive Chunk Counter. */
#define  XRCH  144  /* Received chunk history record. */
#define  XRRL  145  /* Only one receiver per ship method at a time. */

#define  XAHMI 150  /* Audit: Incoming Mail, by Host. */
#define  XAHMO 151  /* Audit: Outgoing Mail, by Host. */
#define  XAHCI 152  /* Audit: Incoming Conference material, by Host. */
#define  XAHCO 153  /* Audit: Outgoing Conference material, by Host. */
#define  XAHPI 154  /* Audit: Incoming People info, by Host. */
#define  XAHPO 155  /* Audit: Outgoing People info, by Host. */
#define  XAUCO 156  /* Audit: Outgoing Conference material, by Userid. */
#define  XAUMI 157  /* Audit: Incoming Mail, by recipient Userid.. */
#define  XAUMO 158  /* Audit: Outgoing Mail, by sending Userid. */
#define  XACCI 159  /* Audit: Incoming Conference material, by Conference. */
#define  XACCO 160  /* Audit: Outgoing Conferecne material, by Conference. */

#define  XACD  170  /* Conf directory of File Attachments. */
#define  XAUD  171  /* User directory of F-A's. */
#define  XARF  172  /* Response-Type F-A's. */
#define  XAMF  173  /* Message-Type  F-A's. */
#define  XART  174  /* Response-Type F-A's, temporary file. */
#define  XAMT  175  /* Message-Type  F-A's, temporary file. */

#define  XLAST 175   /* Always the last Unit number. */

/*** Access and lock type codes, used in unit_lock calls. */
#define UNLKED   0
#define READ     1
#define WRITE    2
#define TLOCK    3     /* Test, lock if possible, don't wait. */
#define LOCKED   4

#endif
