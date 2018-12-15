
/*** UNIT_LOCK.   Name and lock a specific file, by unit number.
/
/    UNIT_LOCK locks the file specified by unit number N.  The file
/    is further specified, if necessary, by conference, item, response,
/    and line-numbers CNUM, ITEM, RESP, and LNUM; and by the string WORD.
/
/    The parameter READWRIT indicates the kind of lock requested.  There
/    are three possibilities:
/       WRITE   lock for writing, only one locker allowed
/       READ    lock for reading, many simultaneous read locks allowed
/       TLOCK   request a WRITE lock, but return immediately if already
/               locked by someone else.
/    Caucus.h defines the macros WRITE, READ, and TLOCK that should be
/    used for this parameter.
/
/    Once a file is locked, it stays locked, until UNIT_UNLK or
/    UNIT_WIPE is called.  On some systems, the lock may also be
/    removed if the process exits.
/
/    The "Arguments" table below describes the arguments that are
/    required for each file unit code.  The meanings of the entries
/    in the table are as follows:
/       Cnum      conference number.
/       Item      item number.
/       Resp      response number.
/       Mess      message number.
/       Lnum      starting line number of a particular partfile.
/       Pfile     part-file number (as in names001)
/       x         number (as in number of a names file).
/       File      name of a (usually external to Caucus) file.
/       Uid       userid.
/       ""        empty string.
/       Uid/""    userid, or uses current userid if empty string.
/       host      host number, 1 to MAXHOST-1
/       ship      shipping method, 1 to 999
/       recv      receiving method, 1 to 999
/       seq       sequence number for CaucusLink, 0 to MAXSEQ-1
/       chunk     chunk number for CaucusLink, 0 to MAXSEQ-1.
/
/    If an argument appears in parentheses (e.g., "(cnum)"), it means
/    that the argument may be used in a call to unit_lock, but is
/    actually ignored.
/
/=======Arguments===================================================
/  N    cnum   item    resp    lnum   word       Limits, Comments
/===================================================================
/ XIRE    0      0       0       0    file
/ XIPT    0      0       0       0    file
/ XITX    0      0       0       0      ""
/ XITY    0      0       0       0      ""
/ XICE    0      0       0       0      ""
/ XILM    0      0       0       0      ""
/ XICF    0      0       0       0    file
/ XISF    0      0       0       0    file
/ XIBB    0      0       0       0      ""
/ XURG    0      0       0       0     uid/""
/ XUMD    0      0       0       0     uid/""
/ XUMF    0      0     mess    lnum    uid
/ XUND (cnum)    0       0       0     uid/""  See [1]: psub's in one file
/ XUNN (cnum)  pfile     0       0     uid/""  See [1]
/ XUSD (cnum)    0       0       0     uid/""  See [1];
/ XUSF (cnum)    0    pfile    lnum    uid     See [1], [2].
/ XUOD    0      0       0       0     uid/""
/ XUON (cnum)  pfile     0       0     uid/""  See [1].
/ XUPA  cnum     0       0       0     uid
/ XUOU    0      0       0       0     uid/""
/ XUXF    0      0       0       0     uid
/ XUXT    0      0       0       0     uid
/ XUDD    0      0       0       0     uid
/ XUVA    0      0       0       0     uid
/ XCRD  cnum   item      0       0      ""
/ XCRF  cnum   item    resp    lnum      ""
/ XCMD  cnum     0       0       0      ""
/ XCMN  cnum   pfile     0       0      ""
/ XCTD  cnum     0       0       0      ""
/ XCTN  cnum   pfile     0       0      ""
/ XCAD  cnum     0       0       0      ""
/ XCAN  cnum   pfile     0       0      ""
/ XCND  cnum     0       0       0      ""
/ XCNN  cnum   pfile     0       0      ""
/ XCSD  cnum     0       0       0      ""
/ XCSF  cnum     0     pfile   lnum     ""     See [2].
/ XCOD  cnum     0       0       0      ""
/ XCON  cnum   pfile     0       0      ""
/ XCMA  cnum     0       0       0      ""
/ XCIN  cnum     0       0       0      ""
/ XCGR  cnum     0       0       0      ""
/ XCUS  cnum     0       0       0      ""
/ XCDD  cnum     0       0       0      ""
/ XCLG  cnum     0       0       0      ""
/ XCXT  cnum     0       0       0      ""
/ XCNB  cnum     0       0       0      ""
/ XCTC  cnum     0       0       0      ""
/ XCED  cnum     0       0       0      ""
/ XCEN  cnum   pfile     0       0      ""
/ XCFD  cnum   item      0       0      ""
/ XCFN  cnum   pfile   item      0      ""     See [3].
/ XCVA  cnum   item
/ XSMD    0      x       0       0      ""
/ XSND    0      0       0       0      ""
/ XSNN    0      x       0       0      ""
/ XSPC    0      0       0       0      ""
/ XSMU    0      x       0       0      ""
/ XSPD    0      0       0       0      ""
/ XSPN    0      x       0       0      ""
/ XSBG    0      0       0       0      ""
/ XSLG    0      0       0       0      ""
/ XSCD    0      0       0       0      ""
/ XSCN    0      x       0       0      ""
/ XSCK    0      0       0       0      ""
/ XSMK    0      0       0       0      ""
/ XSPV    0      0       0       0      ""
/ XSMO    0      0       0       0    file
/ XSUG    0      0       0       0    file
/ XSPB   seq     0       0       0      ""
/ XSHD    0      0       0       0      ""
/ XSHN    0      x       0       0      ""
/ XSNF    0      0       0       0      ""
/ XSSM    0      0       0       0      ""
/ XSSS   ship    0       0       0      ""
/ XSRM    0      0       0       0      ""
/ XSRS   recv    0       0       0      ""
/ XSPL    0      0       0       0      ""
/ XSUL    0      0       0       0      ""
/ XSRP   seq     0       0       0      ""
/ XSR2   seq     0       0       0      ""
/ XSRD    0      0       0       0      ""
/ XSRN    0      x       0       0      ""
/ XSUP    0      0       0       0    file
/ XSAM    0      0       0       0    file
/ XSOB    0      0       0       0    file
/ XSAY    0      0       0       0    file
/ XHI1   host    0       0       0      ""
/ XHML   host    0       0       0      ""
/ XHXA   host   seq      0       0      ""
/ XHI2   host    0       0       0      ""
/ XHSV   host   ship     0       0      ""
/ XHRV   host   recv     0       0      ""
/ XHSL   host    0       0       0      ""
/ XHDD   host    0       0       0      ""
/ XPAK   ship   host    seq      0      ""
/ XPXH   ship   host    seq      0      ""
/ XPXT   ship   host    seq      0      ""
/ XPXJ   ship   host    seq    chunk    ""
/ XPRH   ship   host    seq      0      ""
/ XPRT   ship   host    seq      0      ""
/ XPNH   ship   host    seq      0      ""
/ XPNT   ship   host    seq      0      ""
/ XRXC   recv   seq      0       0      ""
/ XRRX   recv   host    seq      0      ""
/ XRCC   recv     0      0       0      ""
/ XRCH   recv     0      0       0      ""
/ XRRL   recv     0      0       0      ""
/ XAHMI  lhnum    0      0       0      ""
/ XAHMO  lhnum    0      0       0      ""
/ XAHCI  lhnum    0      0       0      ""
/ XAHCO  lhnum    0      0       0      ""
/ XAHPI  lhnum    0      0       0      ""
/ XAHPO  lhnum    0      0       0      ""
/ XAUCO    0      0      0       0     userid
/ XAUMI    0      0      0       0     userid
/ XAUMO    0      0      0       0     userid
/ XACCI   cnum    0      0       0      ""
/ XACCO   cnum    0      0       0      ""
/ XADD     0      0      0       0      ""
/ XACD    cnum    0      0       0      ""
/ XAUD     0      0      0       0     userid
/ XARF    cnum  item    resp     0      ""
/ XAMF     0      0     mess     0     userid
/
/ Notes:
/ [1]. All of the PSUBJECTs for a single user are lumped together into
/      one set of files, regardless of which conference they came from.
/      (The conference name is embedded as part of the psubject name.)
/      This means that unit_lock can cheerfully ignore the CNUM argument.
/
/ [2]. The use of pfile in XUSF may appear inconsistent with pfile in
/      XUNN.  There are historical reasons for this, because names partfiles
/      always use the partfile # as the item number, whereas text partfiles
/      use the partfile # as the response number.
/
/ [3]. XCFN uses the partfile number as the "item" number to be consistent
/      with other names partfiles.  Unfortunately, this means it has to
/      use the item number as the "response" number!  *Sigh*.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:54 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CC  2/09/01 21:00 Add XSAY. */
/*: CR  4/11/03 Use <errno.h> */
/*: CR  8/20/04 Add XCUP. */

#include <stdio.h>
#include <errno.h>
#include "caucus.h"
#include "handicap.h"
#include "systype.h"
#include "units.h"
#include "unitwipe.h"

#include <fcntl.h>
#include <sys/types.h>

#if UNIX | NUT40
#include <unistd.h>
#define  LOCKER(lt,al)  fcntl (lt.fd, F_SETLK, al)
#endif

#if WNT40
   struct flock { int l_type;   int x;  int l_start;   int z; };
#define F_RDLCK  0
#define F_WRLCK  1
#define  LOCKER(lt,al)  winlock (lt.handle, al)
#endif

#define  MAXCANCEL  12

extern struct unit_template  units[];
extern Chix                  confid;
extern Flag                  shouldlock[];
extern int                   debug;
extern int4                  time_lock;

FUNCTION  unit_lock (n, readwrit, cnum, item, resp, lnum, word)
   int   n, readwrit;
   int4  cnum, item, resp, lnum;
   Chix  word;
{
   char *bugtell();
   Chix  newstr = nullchix;
   int   success;

/*------------------------------Most UNIXes------------------------------*/
#if UNIX | NUT40 | WNT40

/*** Maximum range of host and sequence numbers used in locking CaucusLink
/    shipper and receiver files. */
#define MAXHOST    L(1000)
#define MAXSEQ  L(1679616)   /* 36 ** 4 */

/*** Under UNIX, there are six possible kinds of locking done:
/      LSYS:   lock files common to the Caucus system.
/      LUSR:   lock files common to a single user.
/      LCNF:   lock files common to a single conference.
/      LHST:   lock files common to a single host.
/      LSHP:   lock files common to a shipping bay (method).
/      LRCV:   lock files common to a receiving bay (method).
/
/    For each of these types, we maintain an open "lockmap" file.  Each
/    byte in the relevant lockmap file acts as a one-bit semaphore that
/    we can lock or unlock.  The semaphore is used as a logical "lock"
/    for the file that is mapped onto that semaphore.  The mappings
/    are coded below.
/
/    This version ignores the READWRIT parameter, and treats all locks
/    as exclusive. */

   extern struct locktype_t locktype[];
   short  type, i;
   int4   lock;
   char  *bugtell();

   static struct { int4 first;   int4 range; } lmap[XLAST+1];
   static int lmap_initialized = 0;

   struct flock alock = { 0, SEEK_SET, 0, 1 };
   int    lockflag;

   ENTRY ("unitlock", "");

/*** The DEF macro is used to define the lock map byte ranges for various
/    unit codes.  For a given user or a given conference, each unit code
/    number gets a certain block or range of byte #s.  The DEF macro
/    defines a struct of int4s for each unit that contains the starting
/    lock map byte number, and the size of the range.  */
#define DEF(xt, xp, xr) \
   { lmap[xt].first = lmap[xp].first + lmap[xp].range;   lmap[xt].range = xr; }

   if (NOT lmap_initialized) {
      lmap_initialized = 1;
      lmap[0].first = L(0);   lmap[0].range = L(1000);   /* 1000 unit codes. */

      /*** Define the lock map byte number ranges, by lower-case unit code. */
      DEF (XUNN,    0, L(1000));   /* 1000 psubj names files. */
      DEF (XUON, XUNN, L(1000));   /* 1000 psubj alpha files. */
      DEF (XUPA, XUON, L(1000));   /* 1000 conf partic. records. */
      DEF (XUSF, XUPA, L(1000));   /* 1000 psubjects. */
      DEF (XUMF, XUSF, L(1000));   /* 1000 messages.  Ignore lnums. */
      DEF (XAMF, XUMF, L(1000));   /* 1000 message attachments. */
  
      DEF (XCMN,    0, L(1000));     /* 1000 membership partfiles. */
      DEF (XCTN, XCMN, L(1000));     /* 1000 title partfiles. */
      DEF (XCAN, XCTN, L(1000));     /* 1000 author partfiles. */
      DEF (XCNN, XCAN, L(1000));     /* 1000 subject name partfiles. */
      DEF (XCON, XCNN, L(1000));     /* 1000 alpha subject name partfiles. */
      DEF (XCSF, XCON, L(1000));     /* 1000 subject definition partfiles. */
      DEF (XCEN, XCSF, L(1000));     /* 1000 item-entry-time name partfiles. */
      DEF (XCFD, XCEN, MAXITEMS);  /* maxitems resp-entry-time directories. */
      DEF (XCVA, XCFD, MAXITEMS);  /* maxitems variables files */
      DEF (XCFN, XCVA, MAXITEMS * L(1000)); /* resp-entry-time name partfiles. */
      DEF (XCRD, XCFN, MAXITEMS);         /* response 'directories'. */
      DEF (XCRF, XCRD, MAXITEMS * MAXRESP + MAXLNUM); /* individ. resp's. */
      DEF (XARF, XCRF, MAXITEMS * MAXRESP);           /* resp attachments. */
   }


   newstr    = chxalloc (L(160), THIN, "unitlock newstr");
   success   = 0;

   /*** Save the true name of the file in the UNITS structure.  */
   unit_name (units[n].nameis, n, cnum, item, resp, lnum, word);
   if (units[n].access != UNLKED)
      buglist (bugtell ("Unitlock:", n, (int4) units[n].access,
                        "Locking file not unlocked!\n"));

   /*** Many file types do not *actually* get locked. */
   if (NOT shouldlock[n]) {
      if (debug & DEB_LOCK)  bugtell ("pLok", n, L(0), units[n].nameis);
      units[n].access = LOCKED;
      SUCCEED;
   }

   /*** Figure the lock TYPE and semaphore mapping number (LOCK) of the
   /    files that do get locked. */
   switch (n) {
      case (XUDD):
      case (XURG):
      case (XUMD):
      case (XUND):
      case (XUSD):
      case (XUOD):
      case (XUXF):
      case (XUXT):
      case (XAUCO):
      case (XAUMO):
      case (XAUMI):
      case (XULG):
      case (XUVA):
      case (XUOU):   type = LUSR;    lock =  n;                          break;

      case (XUNN):   type = LUSR;    lock = lmap[XUNN].first + item;     break;
      case (XUON):   type = LUSR;    lock = lmap[XUON].first + item;     break;
      case (XUPA):   type = LUSR;    lock = lmap[XUPA].first + cnum;     break;
      case (XUSF):   type = LUSR;    lock = lmap[XUSF].first + resp;     break;
      case (XUMF):   type = LUSR;    lock = lmap[XUMF].first + resp;     break;
      case (XAMF):   type = LUSR;    lock = lmap[XAMF].first + resp;     break;

      case (XCMA):
      case (XCTD):
      case (XCAD):
      case (XCMD):
      case (XCND):
      case (XCSD):
      case (XCOD):
      case (XCIN):
      case (XCGR):
      case (XCLG):
      case (XCXT):
      case (XCNB):
      case (XCTC):
      case (XACCI):
      case (XACCO):
      case (XCUX):
      case (XCED):
/*    case (XCVA): */
      case (XCUP):
      case (XCUS):   type = LCNF;    lock =  n;                        break;

      case (XCMN):   type = LCNF;    lock = lmap[XCMN].first + item;   break;
      case (XCTN):   type = LCNF;    lock = lmap[XCTN].first + item;   break;
      case (XCAN):   type = LCNF;    lock = lmap[XCAN].first + item;   break;
      case (XCNN):   type = LCNF;    lock = lmap[XCNN].first + item;   break;
      case (XCON):   type = LCNF;    lock = lmap[XCON].first + item;   break;
      case (XCSF):   type = LCNF;    lock = lmap[XCSF].first + resp;   break;
      case (XCEN):   type = LCNF;    lock = lmap[XCEN].first + item;   break;
      case (XCFD):   type = LCNF;    lock = lmap[XCFD].first + item;   break;
      case (XCVA):   type = LCNF;    lock = lmap[XCVA].first + item;   break;

      case (XCFN):   type = LCNF;
                     lock = lmap[XCFN].first + (resp * L(1000)) + item;
                     break;

      case (XCRD):   type = LCNF;    lock = lmap[XCRD].first + item;   break;

      case (XCRF):   type = LCNF;
                     lock = lmap[XCRF].first + (item * MAXRESP) + resp + lnum;
                     break;

      case (XARF):   type = LCNF;
                     lock = lmap[XARF].first + (item * MAXRESP) + resp;
                     break;

      case (XSND):
      case (XSPC):
      case (XSPD):
      case (XSBG):
      case (XSLG):
      case (XSCK):
      case (XSMK):
      case (XSPV):
      case (XSMO):
      case (XSSM):
      case (XSRM):
      case (XSNF):
      case (XSPL):
      case (XSUL):
      case (XSHD):
      case (XSRD):
      case (XSUP):
      case (XSAM):
      case (XSMG):
      case (XSVA):
      case (XSOB):
      case (XSAY):
      case (XSCD):   type = LSYS;    lock = n;                         break;

      case (XSNN):   type = LSYS;    lock = L( 1000) + item;           break;
      case (XSMU):   type = LSYS;    lock = L( 2000) + item;           break;
      case (XSPN):   type = LSYS;    lock = L( 3000) + item;           break;
      case (XSCN):   type = LSYS;    lock = L( 4000) + item;           break;
      case (XSHN):   type = LSYS;    lock = L( 5000) + item;           break;
      case (XSSS):   type = LSYS;    lock = L( 6000) + cnum;           break;
      case (XSRS):   type = LSYS;    lock = L( 7000) + cnum;           break;
      case (XSRN):   type = LSYS;    lock = L( 8000) + item;           break;

      case (XSPB):   type = LSYS;    lock =     MAXSEQ + cnum;   break;
      case (XSRP):   type = LSYS;    lock = 2 * MAXSEQ + cnum;   break;
      case (XSR2):   type = LSYS;    lock = 3 * MAXSEQ + cnum;   break;

      case (XHML):
      case (XHSL):
      case (XHDD):
      case (XHI2):
      case (XAHMI):
      case (XAHMO):
      case (XAHCI):
      case (XAHCO):
      case (XAHPI):
      case (XAHPO):
      case (XHI1):   type = LHST;    lock = n;              break;

      case (XHSV):   type = LHST;    lock = L(1000) + item;   break;
      case (XHRV):   type = LHST;    lock = L(2000) + item;   break;
      case (XHXA):   type = LHST;    lock = L(3000) + item;   break;

      /*** LSHP (shipment) files can ignore shipping method #, as there
      /    is only one shipment method per host.   XRRX is included with
      /    the LSHP files, even though it is a receive file. */
      case (XPXH): type = LSHP;  cnum=item;  lock = resp +     MAXSEQ;  break;
      case (XPXT): type = LSHP;  cnum=item;  lock = resp + 2 * MAXSEQ;  break;
      case (XPRH): type = LSHP;  cnum=item;  lock = resp + 3 * MAXSEQ;  break;
      case (XPRT): type = LSHP;  cnum=item;  lock = resp + 4 * MAXSEQ;  break;
      case (XPNH): type = LSHP;  cnum=item;  lock = resp + 5 * MAXSEQ;  break;
      case (XPNT): type = LSHP;  cnum=item;  lock = resp + 6 * MAXSEQ;  break;
      case (XPAK): type = LSHP;  cnum=item;  lock = resp + 7 * MAXSEQ;  break;
      case (XRRX): type = LSHP;  cnum=item;  lock = resp + 8 * MAXSEQ;  break;

      case (XRCC):
      case (XRCH):
      case (XRRL): type = LRCV;              lock = n;                  break;

      case (XRXC): type = LRCV;              lock = item + MAXSEQ;      break;

      default:
         buglist (bugtell ("Bad LOCK case!", n, L(0), units[n].nameis));
         sysexit (1);
   }

   units[n].locknum  = lock;
   units[n].locktype = type;

   /*** If debugging, tell user about lock attempt. */
   if (debug & DEB_LOCK)  bugtell ("LOCK", n, lock, units[n].nameis);

   /*** Make sure the locking file for this TYPE is open. */
   if (NOT unit_lkfx (type, word, cnum)) {
      syssleep (2);
      if (NOT unit_lkfx (type, word, cnum)) {
         char  temp[300];
         sprintf (temp, "unit_lock: can't open lockmap %d %d %d %s\n",
                  type, cnum, n, (word==nullchix ? " " : ascquick(word)));
         buglist (temp);
         unitstack();
         FAIL;
      }
   }

   alock.l_type   = (readwrit == READ ? F_RDLCK : F_WRLCK);
   alock.l_start  = lock;
   success   = 0;

   for (i=0;   (lockflag = LOCKER (locktype[type], &alock)) < 0; 
        i++) {
      if (readwrit == TLOCK)  { units[n].locknum = -1;   FAIL; }
/*    fprintf (stderr, "errno=%d\n", errno);   */
      if (sysbrktest()  &&  i>MAXCANCEL)  sysexit(1);
      if (debug & DEB_LOCK) {
         chxclear  (newstr);
         chxformat (newstr, CQ("Please wait for lock on %d: %d\n"), 
                            L(n), L(lock), nullchix, nullchix);
         unit_write (XWAN, newstr);
      }
      syssleep (2);
      time_lock += 2;
   }

   units[n].access = LOCKED;
   SUCCEED;
#endif

 done:

   if (chxtype(newstr)) chxfree ( newstr );

   RETURN ( success );
}

#if WNT40

FUNCTION  winlock (HANDLE hd, struct flock *alock)
{
   OVERLAPPED overlap;
   DWORD      flags;
   BOOL       success;

   overlap.Offset     = alock->l_start;
   overlap.OffsetHigh = 0;
   flags = LOCKFILE_FAIL_IMMEDIATELY | 
           (alock->l_type == F_WRLCK ? LOCKFILE_EXCLUSIVE_LOCK : 0);
   success = LockFileEx (hd, flags, 0, 1, 0, &overlap);

   return (success ? 0 : -1);
}
#endif
