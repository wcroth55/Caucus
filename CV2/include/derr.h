/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** DERR.H:  The Include file for all the error numbers for
/             Distributed Caucus, as returned by the chg_ditem(),
/             chg_dresp()..., chgxitem(), chgxresp(), etc. functions. */

/*: JV  6/21/91 17:24 Created file. */
/*: CR  6/22/91 15:49 Add DBADDB, DINORESP. */
/*: CR  7/01/91 20:35 Add comments, #ifndef stuff. */
/*: CR  7/05/91 16:48 Add DNOMEM. */
/*: JV  7/24/91 08:04 Add HUMOR. */
/*: CR  7/17/91 17:59 Add DSUBSET definition, change DNOOSUB -> DNOSUBJ. */
/*: CR  8/01/91 16:35 Add DS_... shipper program series. */
/*: CR  8/08/91 13:13 Add DS_NOTIMP, DS_NOTDSK, DS_DSKLOW. */
/*: CR  8/14/91 19:02 Add DR_... series. */
/*: JV 11/07/91 09:39 Add DMKDIR. */
/*: CR 11/15/91 12:46 Add DREXISTS. */
/*: JV 12/23/91 11:58 Add DNOPERM. */
/*: JV 10/26/92 13:22 Add DR_BADUU, DR_BADCPT. */
/*: CR  7/05/94 16:32 Add DSUBPUNC. */

#ifndef DERR_H
#define DERR_H

#define NOERR      0

/* These are for the Xaction Library. */
#define DBADXACT   1      /* The xaction is not a sane one. */
#define DLOCK      2      /* Couldn't write the xact: couldn't lock file. */
#define DFILE      3      /*   "       "         "  : couldn't open file. */
#define DNOCHN     4      /* That CHN is not in the database. */
#define DMISC      5      /* Miscellaneous error: usually a bug. */

/* These are for the DB Library. */
#define DNOCONF   11      /* No conf of that true name exists on this host. */
#define DCFULL    12      /* The Conf has MAXITEMS items in it already. */
#define DNOMEM    13      /* Can't allocate any more dynamic memory. */
#define DMKDIR    14      /* Can't create that/a directory. */
#define DNOPERM   15      /* The requestor doesn't have permission. */

#define DINOITEM  21      /* There is no item of that ITEMNUM@CHN pair. */
#define DIFULL    22      /* The item has MAXRESP responses in it already. */
#define DIEXISTS  23      /* That item already exists. */
#define DINORESP  24      /* There is no response of that rnum@chn pair. */
#define DREXISTS  25      /* That response already exists. */

#define DNOSUBJ   31      /* That subject doesn't exist. */
#define DOEXIST   32      /* That subject already exists. */
#define DSUBSET   33      /* Subject name is a proper subset of another. */
#define D2SUBJ    34      /* Name matches multiple subjects. */
#define DSUBPUNC  35      /* Subject name is all punctuation! */

#define DNOPERS   41      /* There isn't anyone by that name or id. */
#define DPEXIST   42      /* That person already exists. */
#define DNOALIEN  43      /* This host doesn't store remote user names. */
#define DLANG     44      /* This host doesn't support the character set
                          /  that this user's name is in. */

#define DNOUSER   51      /* No username/id matched the recipient. */
#define DMANYUSER 52      /* Many users matched the recipient. */
#define DBOXFULL  53      /* The recipient's mailbox is full. */

#define DBADDB    60      /* The Caucus database is confused! */
#define DBADACT   61      /* Bad action code! *Bad* action code! SIT!*/

/*** The DS_... series refers to the shipper program. */
#define  DS_NOXPXH  70    /* Can't get at transport header file! */
#define  DS_NOXPXT  71    /* Can't get at transport text file! */
#define  DS_NOXPXJ  72    /* Can't create a joint chunk shipment file. */
#define  DS_NOXPRH  73    /* Can't put header file in RESHIP bay. */
#define  DS_XTMOVE  74    /* Can't move transport text file to RESHIP bay. */
#define  DS_NOTIMP  75    /* sysfsize() or sysbytes() not defined ! */
#define  DS_NOTDSK  76    /* Can't get info about free disk space. */
#define  DS_DSKLOW  77    /* Insufficient disk space. */

/*** The DR_... series refers to the receiver program. */
#define  DR_NOXRXC  80    /* Can't read a chunk that syspdir said exists. */
#define  DR_BADCHK  81    /* Chunk file doesn't have a chunk header. */
#define  DR_GETNEI  82    /* Can't load hostinfo for transport file. */
#define  DR_WRINEI  83    /* Can't rewrite hostinfo after doing transport. */
#define  DR_BADUU   84    /* Bad uuencoded chunk. */
#define  DR_BADCPT  85    /* Bad encrypted chunk. */

#endif
