
/*** NAME_TO_NETID.  Convert a name to a set of id@chn pairs.
/
/    name_to_netid (name, id, chn, local, list, error);
/
/    Parameters:
/       Chix     name;    (some words in a person's name, else "")
/       Chix     id;      (userid if known, else "")
/       Chix     chn;     (person's host name, if known, else "")
/       int      local;   (1==>only people on this host, 0==>anywhere)
/       Namelist list;    (place to put list of matches found)
/       int     *error;   (returned error code, from namelist.h)
/
/    Purpose:
/       There are many places in Caucus where we have only partial
/       information about a person: some part of their name, OR their
/       userid, OR their home host.
/
/       Name_to_netid() takes such partial information and attempts
/       to produce a matching list of people, specified as pairs of 
/       userid and chn.
/
/    How it works:
/       Name_to_netid() scans the local names001 files to find possible
/       matches against the information in the arguments NAME, ID, and CHN.
/       Any of these arguments may contain only "", meaning "no information,
/       could match anything", with the exception that one of NAME or ID
/       *must* contain something.
/
/       If CHN is "", and LOCAL is 1, only people on the current host will
/       be found.  Otherwise, any host name may match.
/
/       The LIST argument must be an already allocated (and empty) Namelist.
/       Matches are returned as entries in the namelist.  Each entry 
/       contains a string of the form "id@chn" or "id".  (The latter form
/       implies that this is an id on the local host.)
/
/    Returns: 1 if one or more matches were returned in LIST.
/             0 if no matches were found
/             0 if an error occurred (and no matches were found)
/
/    Error Conditions: returns value in ERROR:
/      NLE_NOERR      no errors, matches (if any) are in LIST.  
/      NLE_EMPTY      NAME, and ID were both empty.
/      NLE_DBERR      database error
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions, XFU
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/nametone.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/09/91 18:47 New function. */
/*: CR  7/10/91 11:22 Adjust specs.  Write code. */
/*: CR  7/16/91 16:06 Fix special cases. */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: CR 11/07/91 14:18 Add confnum arg to matchnames() call. */
/*: JV 12/03/91 15:46 Allow substring matches for CHN's. */
/*: DE  4/14/92 14:26 Chixified */
/*: JX 10/07/92 08:48 Fixify. */
/*: CR 10/12/01 16:40 Ignore @'s!  Replace with "^" for now. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION    name_to_netid (name, id, chn, local, list, error)
   Chix     name, id, chn;
   int      local;
   Namelist list;
   int     *error;
{
   int      ok;
   Chix     temp, userid, host, thishost, list_str;
   Namelist nameparts, matches, p, chn_nl;
   Namelist nlnames(), nlnode();
   int      success;

   ENTRY ("name_to_netid", "");

   temp     = chxalloc (L(100), THIN, "name_to_netid temp");
   userid   = chxalloc (L(100), THIN, "name_to_netid userid");
   host     = chxalloc (L(100), THIN, "name_to_netid host");
   thishost = chxalloc (L(100), THIN, "name_to_netid thishost");
   list_str = chxalloc (L(100), THIN, "name_to_netid list_str");
   success  = 0;

   *error = NLE_NOERR;
   getmyhost (thishost);
   if (EMPTYCHX(name)  &&  EMPTYCHX(id))  { *error = NLE_EMPTY;   FAIL; }

   /*** If we were given a CHN and it's not ours, find all the CHN's
   /    in the routing table that match it, create a list out of them. */
   if (NOT EMPTYCHX (chn)) {
      chn_nl = nlnode (1);
      nladd (chn_nl, chn, 0);
      ok = matchnames (XSRD, XSRN, 0, null.chx, chn_nl, AND, &matches, NOPUNCT);
      nlfree (chn_nl);
      if (NOT ok) {
         nlfree (matches); *error = NLE_DBERR; FAIL; }

      /*** Create a node in the linked list for each CHN match. */
      for (p=matches->next;  p!=null.nl; p = p->next) {
         chxtoken  (temp, null.chx, 1, p->str);
         chxclear  (list_str);
         chxformat (list_str, CQ("%s^%s"), L(0), L(0), name, temp);
         nladd (list, list_str, 0);
      }
      nlfree (matches);
      SUCCEED;
   }

   /*** Consider the case where we know a particular CHN (or that we
   /    are only interested in people on the local host). */
   if (local) {
/* if (NOT EMPTYCHX(chn)  ||  local) { */

      /*** In the easiest possible case, we were also given a userid.
      /    Just hand it back to the caller (with the CHN, if it's there)! */
      if (NOT EMPTYCHX (id)) {
        chxcpy (temp, id);
         if (NOT EMPTYCHX (chn)) { chxcat (temp, CQ("^")); chxcat (temp, chn);}
         nlcat  (list, temp, 0);
         SUCCEED;
      }

      /*** Otherwise, there are nameparts in NAME.  Find those matches. */
      nameparts = nlnames (name, null.chx);
      if (NOT matchnames (XSND, XSNN, 0, nullchix, nameparts, AND,
                          &matches, NOPUNCT))
         goto err;

      /*** Then reduce them to the set that have a host name matching
      /    CHN (or no host name if LOCAL is set). */
      for (p=matches->next;   p!=null.nl;   p = p->next) {
         chxtoken (temp, null.chx, 2, p->str);
         chxbreak (temp, userid, host, CQ("^"));
         if (EMPTYCHX (host))  getmyhost (host);
         
         if (NOT EMPTYCHX(chn) && chxeq (host, chn)  ||
             (local && chxeq (host, thishost)) )
            nladd (list, temp, 0);
      }
   }

   /*** Now consider the case with no CHN, possibly matching people on
   /    *any* host. */
   else {

      /*** Build a list of nameparts to match against.  If an ID was
      /    specified, just add that as another namepart. */
      nameparts = nlnames (name, null.chx);
      if (NOT EMPTYCHX (id))  nladd (nameparts, id, 0);

      if (NOT matchnames (XSND, XSNN, 0, null.chx, nameparts, AND,
                          &matches, NOPUNCT))
         goto err;

      /*** If an ID was specified, add only those matches with ID to
      /    LIST.  Otherwise, add all the matches. */
      for (p=matches->next;   p!=null.nl;   p = p->next) {
         chxtoken (temp, null.chx, 2, p->str);
         chxbreak (temp, userid, host, CQ("^"));
         
         if (NOT EMPTYCHX(id)  &&  NOT chxeq (id, userid))  continue;
         nladd (list, temp, 0);
      }
   }

   nlfree (nameparts);
   nlfree (matches);
   SUCCEED;

err:
   nlfree (nameparts);
   *error = NLE_DBERR;
   FAIL;

done:

   chxfree (temp);
   chxfree (userid);
   chxfree (host);
   chxfree (thishost);
   chxfree (list_str);

   RETURN (success);
}
