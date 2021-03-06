
/*** UNIT_NAME.   Figure the name of a conference file.
/
/    1 = unitname (name, n, cnum, item, resp, lnum, word)
/
/    Parameters:
/       int  n;       // Unit number
/       int4 cnum,    // Conference number
/            item,    // Item number
/            resp,    // Resp number (or message number)
/            lnum;    // Line number
/       char name[];  // Place to store name
/       Chix word;    // Textual input info
/
/    Purpose:
/       UNIT_NAME takes a unit number N, and returns the system-dependent
/       filename in NAME.  In many cases, the precise name of the file is
/       further specified by the numbers CNUM, ITEM, RESP, and LNUM, and the
/       string WORD.
/
/    How it works:
/       For a detailed list of the arguments required for each file unit
/       number, see the header documentation for unit_lock.
/
/    Returns: 1
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies
/
/    Known bugs:
/       XSAM and file attachments have not been implemented for Primos.
/
/       AOS and Client sections are considered obsolete code and are
/       not up-to-date in many areas.
/
/    Home: unit/unitname.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:55 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CC  2/09/01 21:00 Add XSAY. */
/*: CR  5/02/03 Use /tmp for XITX, XITY. */
/*: CR  6/18/03 Correct calculation of cauid to allow for "/caucus"! */
/*: CR  8/20/04 Add XCUP. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

extern struct unit_template units[];
extern struct flag_template flag;
extern Chix   confid;
extern int    debug;

FUNCTION  unit_name (name, n, cnum, item, resp, lnum, word)
   int    n;
   int4   cnum, item, resp, lnum;
   char   name[];
   Chix   word;
{
   static char first_xulg = 1;
   static char xulgname[160];
   char   uidstr[200], std[16], std2[16], *strito36();
   char   cidstr[200], cauid[200], wordstr[200], item_str[8], resp_str[8];
   Chix   uid;
   int    year, month, day, hour, minute, sec, wday, i;
   char  *bugtell();
   int4   systime();

   ENTRY ("unitname", "");

   uid = chxalloc (L(16), THIN, "unitname uid");

   /*** Get the regular ascii form of WORD. */
   wordstr[0] = '\0';
   if (word!=nullchix) ascofchx (wordstr, word, L(0), L(200));

   /*** Get the regular ascii form of CONFID and CAUID (latter just userid). */
   ascofchx (cidstr, confid, L(0), L(200));
   for (i=strlen(cidstr)-1;   i>=0;   --i)  if (cidstr[i] == '/')  break;
   if (i >= 0)  strcpy (cauid, cidstr+i+1);
   else         strcpy (cauid, cidstr);

   /*** If WORD is supplied, use it for the uid as well.  Otherwise,
   /    ask the operating system for the current uid. */
   if (NOT NULLSTR(wordstr)) {
      strcpy (uidstr, wordstr);
      chxcpy (uid, CQ(wordstr));
   } else {
      sysuserid (uid);
      ascofchx  (uidstr, uid, L(0), L(40));
   }

/*============================Unixes=====================================*/
#if UNIX | NUT40 | WNT40

#define USTUFF        cidstr, udnum, uidstr
#define USERNUM(q,n)  sprintf (name, "%s/USER%03d/%s/%s%03d",  USTUFF, q, n)
#define USER(q)       sprintf (name, "%s/USER%03d/%s/%s",      USTUFF, q)
/*#define UTEXT(q)      sprintf (name, "%s/TEXT%03d/%s/%s",      USTUFF, q)*/
#define UTEXT(q)      sprintf (name, "/tmp/caucus.%s.%s.%s",   cauid, uidstr, q)
#define CONFNUM(q,n)  sprintf (name, "%s/C%04d/%s%03d",   cidstr, cnum,q, n)
#define CONF(q)       sprintf (name, "%s/C%04d/%s",       cidstr, cnum,q)
#define SYSTNUM(q,n)  sprintf (name, "%s/MISC/%s%03d",    cidstr,      q, n)
#define SYST(q)       sprintf (name, "%s/MISC/%s",        cidstr,      q)
#define SOBJ(q)       sprintf (name, "%s/OBJECTS/%s",     cidstr,      q)
#define HOST(q)       sprintf (name, "%s/H%03d/%s",       cidstr, cnum,q)

  {

   int  udnum;

   /*** Get user directory number for those files that need it. */
   if (n==XITX  ||  n==XITY  ||  n==XICE  ||  n==XILM  ||
                (n>=XURG  &&  n< XCRD) || (n>=XAUCO && n<=XAUMO))
      udnum = sysudnum (uid, (debug ? 0 : -1), n+2000);

   switch (n) {

   /*-----------I/O files--------------------------------------------*/
      case (XIRE):
      case (XICF):
      case (XIMF):
      case (XISF):
      case (XIAL):
      case (XIOF):
      case (XIPT):  strcpy  (name, wordstr);    break;
      case (XIBB):  strcpy  (name, "xxx");      break;

      case (XITX):  UTEXT   ("textmsc1");       break;
      case (XITY):  UTEXT   ("textmsc2");       break;
      case (XICE):  UTEXT   ("caucused");       break;  /* Used to be USER */
      case (XILM):  UTEXT   ("lastmess");       break;

   /*-----------User files-------------------------------------------*/
      case (XURG):  USER    ("register");       break;
      case (XUOU):  USER    ("onlyuser");       break;
      case (XUND):  USER    ("nmsubddd");       break;
      case (XUNN):  USERNUM ("nmsub", item);    break;
      case (XUOD):  USER    ("alsubddd");       break;
      case (XUON):  USERNUM ("alsub", item);    break;
      case (XUXF):  USER    ("xactions");       break;
      case (XUXT):  USER    ("tmpxact");        break;
      case (XUVA):  USER    ("variable");       break;
      case (XAUCO): USER    ("confnet.au");     break;
      case (XAUMO): USER    ("mailout.au");     break;
      case (XAUMI): USER    ("mailin.au");      break;

      case (XUDD):
         sprintf (name, "%s/USER%03d/%s", cidstr, udnum, wordstr);
         break;

      /*** Note that changing XUPA's format may break api/alispsta.c */
      case (XUPA):
         sprintf (name, "%s/USER%03d/%s/p%04d000",
                        cidstr, udnum, wordstr, cnum);
         break;

      case (XUSD):  USER    ("dirsubjs");       break;
      case (XUSF):
         sprintf (name, "%s/USER%03d/%s/s%03d%04d",
                        cidstr, udnum, wordstr, resp, lnum);
         break;

      case (XUMD):  USER    ("dirmessg");       break;
      case (XUMF):
         sprintf (name, "%s/USER%03d/%s/m%03d%04d",
                        cidstr, udnum, wordstr, resp, lnum);
         break;

      case (XULG):
         if (first_xulg) {
            sysymd  (systime(), &year, &month, &day, &hour, &minute, &sec, 
                                                                     &wday);
            sprintf (xulgname, "%s/USER%03d/%s/lg%02d%02d%02d",
                               cidstr, udnum, wordstr, year, month, day);
         }
         strcpy (name, xulgname);
         first_xulg = 0;
         break;

   /*-----------Conference files-------------------------------------*/
      case (XCMD):  CONF    ("membrddd");       break;
      case (XCMN):  CONFNUM ("membr", item);    break;
      case (XCTD):  CONF    ("titleddd");       break;
      case (XCTN):  CONFNUM ("title", item);    break;
      case (XCAD):  CONF    ("authrddd");       break;
      case (XCAN):  CONFNUM ("authr", item);    break;
      case (XCND):  CONF    ("nmsubddd");       break;
      case (XCNN):  CONFNUM ("nmsub", item);    break;
      case (XCOD):  CONF    ("alsubddd");       break;
      case (XCON):  CONFNUM ("alsub", item);    break;

      case (XCMA):  CONF    ("masteres");       break;
      case (XCIN):  CONF    ("introduc");       break;
      case (XCGR):  CONF    ("greeting");       break;
      case (XCUS):  CONF    ("userlist");       break;
      case (XCXT):  CONF    ("xaction");        break;
      case (XCUX):  CONF    ("userxact");       break;
      case (XCNB):  CONF    ("neighbor");       break;
      case (XCTC):  CONF    ("trueconf");       break;
      case (XCUP):  CONF    ("hidden");         break;
/*    case (XCVA):  CONF    ("variable");       break; */
      case (XACCI): CONF    ("netin.au");       break;
      case (XACCO): CONF    ("netout.au");      break;

      case (XCED):  CONF    ("itimeddd");       break;
      case (XCEN):  CONFNUM ("itime", item);    break;

      case (XCVA):
         if (item==0)  sprintf (name, "%s/C%04d/variable", cidstr, cnum);
         else          sprintf (name, "%s/C%04d/var%04d", cidstr, cnum, item);
         break;

      case (XCFD):
         if (item > 999) strito36 (item_str, item + TEN_K_OFFSET, 3);
         else            sprintf  (item_str, "%03d", item);
         sprintf (name, "%s/C%04d/rt%sddd", cidstr, cnum, item_str);
         break;

      case (XCFN):
         if (resp > 999) strito36 (item_str, resp + TEN_K_OFFSET, 3);
         else            sprintf  (item_str, "%03d", resp);
         sprintf (name, "%s/C%04d/rt%s%03d", cidstr, cnum, item_str, item);
         break;

      case (XCRD):  CONFNUM ("direc", item);    break;
      case (XCRF):
         /*** Handle 10K Items and responses in the following way (each):
         /    Name is in pre-2.4 format (all decimal digits, iiirrrllll)
         /    Item & Response parts are in STD (base36), from
         /    A00 (12960) to GXZ (21959). */
         if (item > 999) strito36 (item_str, item + TEN_K_OFFSET, 3);
         else            sprintf  (item_str, "%03d", item);
 
         if (resp > 999) strito36 (resp_str, resp + TEN_K_OFFSET, 3);
         else            sprintf  (resp_str, "%03d", resp);
 
         sprintf (name, "%s/C%04d/%s%s%04d", cidstr, cnum,
                  item_str, resp_str, lnum);    break;

      case (XCSD):  CONF    ("dirsubjs");       break;
      case (XCSF):
         sprintf (name, "%s/C%04d/s%03d%04d", cidstr, cnum, resp, lnum);
         break;

      case (XCDD): sprintf (name, "%s/C%04d",   cidstr, cnum);   break;
      case (XCLG): sprintf (name, "%s/log%03d", cidstr, cnum);   break;

   /*-----------System files-----------------------------------------*/
      case (XSMD):  SYSTNUM ("dicti", item);    break;
      case (XSMU):  SYSTNUM ("mxusr", item);    break;
      case (XSPC):  SYST    ("printcod");       break;
      case (XSBG):  SYST    ("bugslist");       break;
      case (XSLG):  SYST    (ascquick(flag.logfile));   break;
      case (XSCK):  SYST    ("confsmsk");       break;
      case (XSMK):  SYST    ("mkdirlk");        break;

      case (XSMG):  SYST    ("managers");       break;
      case (XSVA):  SYST    ("variable");       break;
      case (XSND):  SYST    ("namesddd");       break;
      case (XSNN):  SYSTNUM ("names", item);    break;
      case (XSPD):  SYST    ("passwddd");       break;
      case (XSPN):  SYSTNUM ("passw", item);    break;
      case (XSCD):  SYST    ("confsddd");       break;
      case (XSCN):  SYSTNUM ("confs", item);    break;
      case (XSPV):  SYST    ("privlist");       break;
      case (XSOB): sprintf (name, "%s/OBJECTS/%s", cidstr, wordstr);   break;
      case (XSAY): sprintf (name, "%s/MISC/ARRAYS/%s", cidstr, wordstr);  break;
      case (XSMO): sprintf (name, "%s/%s",         cidstr, wordstr);   break;
      case (XSUP): sprintf (name, "%s/%s",         cidstr, wordstr);   break;
      case (XSUG): sprintf (name, "%s/GROUPS/%s",  cidstr, wordstr);   break;
      case (XSPB): sprintf (name, "%s/PARSEBAY/x%s", cidstr,
                                   strito36 (std, cnum, 7));       break;
      case (XSRP): sprintf (name, "%s/REPARSE/x%s",  cidstr,
                                   strito36 (std, cnum, 7));       break;
      case (XSR2): sprintf (name, "%s/REPARSE/x%s",  cidstr,
                                   strito36 (std, cnum, 7));       break;
      case (XSSS): sprintf (name, "%s/SCRIPTS/shipm%03d", cidstr, cnum);
                   break;
      case (XSRS): sprintf (name, "%s/SCRIPTS/recvm%03d", cidstr, cnum);
                   break;
      case (XSHD):  SYST    ("hostddd");        break;
      case (XSHN):  SYSTNUM ("host", item);     break;
      case (XSNF):  SYST    ("sysinfo");        break;
      case (XSSM):  SYST    ("shipmeth");       break;
      case (XSRM):  SYST    ("recvmeth");       break;
      case (XSPL):  SYST    ("xfplock");        break;
      case (XSUL):  SYST    ("xfulock");        break;
      case (XSRD):  SYST    ("routeddd");       break;
      case (XSRN):  SYSTNUM ("route", item);    break;
      case (XSAM):  SYST    (wordstr);          break;

   /*-----------Host files-----------------------------------------*/
      case (XHI1):  HOST    ("hostinfo");       break;
      case (XHI2):  HOST    ("hostinfo");       break;
      case (XHML):  HOST    ("outmail");        break;
      case (XHSL):  HOST    ("shipper");        break;
      case (XAHMI): HOST    ("mailin.au");      break;
      case (XAHMO): HOST    ("mailout.au");     break;
      case (XAHCI): HOST    ("confin.au");      break;
      case (XAHCO): HOST    ("confout.au");     break;
      case (XAHPI): HOST    ("persin.au");      break;
      case (XAHPO): HOST    ("persout.au");     break;
      case (XHDD):  sprintf (name, "%s/H%03d",   cidstr, cnum);   break;
      case (XHSV):  sprintf (name, "%s/H%03d/shipv%03d", cidstr, cnum, item);
                    break;
      case (XHRV):  sprintf (name, "%s/H%03d/recvv%03d", cidstr, cnum, item);
                    break;
      case (XHXA):  sprintf (name, "%s/H%03d/xact%s",  cidstr, cnum,
                                    strito36 (std, item, 4));
                    break;

   /*-----------Other CaucusLink units-------------------------------*/
      case (XPXH): sprintf (name, "%s/SHIP%03d/H%03d%s", cidstr,
                                   cnum, item, strito36 (std, resp, 4));
                   break;
      case (XPXT): sprintf (name, "%s/SHIP%03d/T%03d%s", cidstr,
                                   cnum, item, strito36 (std, resp, 4));
                   break;
      case (XPXJ): sprintf (name, "%s/SHIP%03d/J%03d%s.%s",
                            cidstr, cnum, item, strito36 (std,  resp, 4),
                                                strito36 (std2, lnum, 3));
                   break;
      case (XPRH): sprintf (name, "%s/SHIP%03d/RESHIP/H%03d%s",
                            cidstr, cnum, item, strito36 (std, resp, 4));
                   break;
      case (XPRT): sprintf (name, "%s/SHIP%03d/RESHIP/T%03d%s",
                            cidstr, cnum, item, strito36 (std, resp, 4));
                   break;
      case (XPNH): sprintf (name, "%s/SHIP%03d/CANTSHIP/H%03d%s",
                            cidstr, cnum, item, strito36 (std, resp, 4));
                   break;
      case (XPNT): sprintf (name, "%s/SHIP%03d/CANTSHIP/T%03d%s",
                            cidstr, cnum, item, strito36 (std, resp, 4));
                   break;
      case (XPAK): sprintf (name, "%s/SHIP%03d/a%03d%s",
                            cidstr, cnum, item, strito36 (std, resp, 4));
                   break;
      case (XRXC): sprintf (name, "%s/RECV%03d/CHUNK/x%s",
                            cidstr, cnum, strito36 (std, item, 7));
                   break;
      case (XRCC): sprintf (name, "%s/RECV%03d/chunkctr", cidstr, cnum);
                   break;
      case (XRCH): sprintf (name, "%s/RECV%03d/chunkhis", cidstr, cnum);
                   break;
      case (XRRX): sprintf (name, "%s/RECV%03d/RERECV/r%03d%s",
                              cidstr, cnum, item, strito36 (std, resp, 4));
                   break;
      case (XRRL): sprintf (name, "%s/RECV%03d/recvlock", cidstr, cnum);
                   break;

   /*-------------------File Attachments------------------------------------*/
      case (XACD): sprintf (name, "%s/ATTACH/C%04d", cidstr, cnum);
                  break;
      case (XAUD): sprintf (name, "%s/ATTACH/USER%03d/%s",
                                   cidstr, udnum, wordstr);
                  break;
      case (XARF):
         /*** Handle 10K Item and response attachments in this way (each):
         /    Use 000-999 (decimal), then A00 (12960) to GXZ (21959). */
         if (item > 999) strito36 (item_str, item + TEN_K_OFFSET, 3);
         else            sprintf  (item_str, "%03d", item);

         if (resp > 999) strito36 (resp_str, resp + TEN_K_OFFSET, 3);
         else            sprintf  (resp_str, "%03d", resp);

         sprintf (name, "%s/ATTACH/C%04d/%s%s", cidstr, cnum,
                  item_str, resp_str);          break;

      case (XART):
         /*** Only one temporary file per user. */
         sprintf (name, "%s/ATTACH/C%04d/%s.tmp", cidstr, cnum, wordstr);
         break;

      case (XAMF):
         sprintf (name, "%s/ATTACH/USER%03d/%s/m%03d",
                        cidstr, udnum, wordstr, resp);

      case (XAMT):
         /*** Only one temporary file per sender/receiver pair. */
         sprintf (name, "%s/ATTACH/USER%03d/%s/xxx.tmp",
                        cidstr, udnum, wordstr);
         break;

      default:
         buglist (bugtell ("Bad case in unitname!", n, item, wordstr));
         sysexit (1);
      }
   }
#endif

   chxfree (uid);

   RETURN  (1);
}
