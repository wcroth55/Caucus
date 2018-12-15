
/*** SYSPDIR.  Get part file directory listing.
/
/    ok = syspdir (pdir, u, confnum, item, owner)
/
/    Parameters:
/       int    ok;             (success?)
/       struct pdir_t *pdir;   (resulting partfile directory list)
/       int    u;              (desired unit code number)
/       int    confnum;        (conference number)
/       int    item;           (item number, if relevant)
/       Chix   owner;          (owning userid, if relevant)
/
/    Purpose:
/       Get a list of the existing partfiles for a specific unit code.
/
/    How it works:
/       Examines the directory specified by unit code U, conference
/       CONFNUM, item ITEM, userid OWNER.  Builds a sorted list of the 
/       matching partfiles in PDIR.  Each entry in the list has the
/       response number (or equivalent) and line number.
/       
/    Returns: 1 on success
/             0 on error
/
/    Error Conditions: can't open directory to read
/  
/    Called by:  getline(), getmline(), etc.
/
/    Operating system dependencies: light
/
/    Known bugs:
/       The server/client code is broken and out-of-date.
/       The PR (primos) code does not have the XARF stuff.
/
/    Home:  sys/syspdir.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

extern Chix    confid;
extern int     debug;

FUNCTION  syspdir (pdir, u, confnum, item, owner)
   struct pdir_t *pdir;
   int    u, confnum, item;
   Chix   owner;
{
   int4   str36toi();
   char  *strito36();

   char   match[20], searchdir[200], std[12], file[256];
   char   confid_str[200], owner_str[80], nresp_str[8];
   int    nresp, nlnum, i, j, udnum, start, error;
   short  mlen, dup;


   ENTRY ("syspdir", "");

   pdir->type = u;

   /*** Defaults are for USER files. */
   mlen = 1;
   udnum = sysudnum (owner, 0, u + 1000);

   ascofchx (confid_str, confid, L(0), L(200));
   if (owner != nullchix) {
      ascofchx (owner_str,  owner,  L(0), L(80));
      sprintf  (searchdir, "%s/USER%03d/%s", confid_str, udnum, owner_str);
   }

   switch (u) {
      case (XUSF):   strcpy  (match, "s");                        break;
      case (XUMF):   strcpy  (match, "m");                        break;
      case (XUPA):   strcpy  (match, "p");                        break;

      case (XCSF):   sprintf (searchdir, "%s/C%04d", confid_str, confnum);
                     strcpy  (match, "s");                        break;

      case (XCRF):   sprintf (searchdir, "%s/C%04d", confid_str, confnum);
                     if (item > 999) strito36 (match, 
                                               L(item + TEN_K_OFFSET), 3);
                     else            sprintf  (match, "%03d", item);
                     mlen = 3;   break;

      case (XARF):   sprintf (searchdir, "%s/ATTACH/C%04d", confid_str, confnum);
                     if (item > 999) strito36 (match, 
                                               L(item + TEN_K_OFFSET), 3);
                     else            sprintf  (match, "%03d", item);
                     mlen = 3;   break;

      case (XPAK):   sprintf (searchdir, "%s/SHIP%03d", confid_str, confnum);
                     strcpy  (match, "a");                        break;

      case (XRXC):   sprintf (searchdir, "%s/RECV%03d/CHUNK", confid_str,
                              confnum);
                     strcpy  (match, "x");                        break;
   }

   /*** Make sure the directory really exists. */
   pdir->parts = 0;
   if (NOT sysisdir (searchdir))      RETURN (1);

   /*** Examine each entry in the directory.  If it matches MATCH,
   /    add it to the sorted list of partfiles. */
   for (start=1;  sysgfdir (searchdir, file, start, &error) == 1;  start = 0) {

      if (match[0] != file[0])                     continue;
      if (mlen > 1  &&  (match[1] != file[1]  ||
                         match[2] != file[2]))     continue;

      switch (u) {
      case XPAK:
         sscanf (file+mlen, "%3d", &nresp);
         strsub (std, file, mlen+3, 4);
         nlnum = str36toi (std);
         break;
      case XRXC:
         strsub (std, file, mlen, 7);
         nlnum = str36toi (std);

         break;
      case XCRF:
         /*** NT doesn't know upper vs lower case; XCRF files must
         /    be 10 chars int4. */
         if (*(file+8) == '\0')  continue;

         if (*(file+mlen) >= '0' && *(file+mlen) <= '9')
            sscanf (file+mlen, "%3d%4d", &nresp, &nlnum);
         else {                                  /* Respnum is > 999. */
            sscanf (file+mlen, "%3s%4d", nresp_str, &nlnum);
            nresp = str36toi (nresp_str);
            nresp -= TEN_K_OFFSET;
         }
         break;
      case XARF:
         if (*(file+mlen) >= '0' && *(file+mlen) <= '9')
            sscanf (file+mlen, "%3d", &nresp);
         else {                                  /* Respnum is > 999. */
            sscanf (file+mlen, "%3s", nresp_str);
            nresp = str36toi (nresp_str);
            nresp -= TEN_K_OFFSET;
         }
         break;
      case XUPA:
         sscanf (file+mlen, "%4d%3d", &nresp, &nlnum);
         break;
      default:
         sscanf (file+mlen, "%3d%4d", &nresp, &nlnum);
         break;
      }
            

      if (u == XPAK  &&  nresp != item)            continue;

      /*** Find where in PDIR this entry belongs.  Remove duplicates. */
      for (dup=0, i=pdir->parts-1;    i>=0;    --i) {
         if (       nresp <  pdir->resp[i])   continue;
         if (       nresp >  pdir->resp[i])   break;
         if (       nlnum >  pdir->lnum[i])   break;
         if (dup = (nlnum == pdir->lnum[i]))  break;
      }
      if (dup)  continue;

      /*** Slip this entry into PDIR. */
      for (j=pdir->parts-1;   j>i;    --j)  {
         pdir->resp[j+1] = pdir->resp[j];
         pdir->lnum[j+1] = pdir->lnum[j];
      }

      pdir->resp[i+1] = nresp;
      pdir->lnum[i+1] = nlnum;
      ++pdir->parts;
      if (pdir->parts >= PDIRMAX-3)  break;
   }

   RETURN   (1);
}
