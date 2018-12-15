/*** VAR_SUBSTITUTE.  Parse and substitute macro variables.
/
/    var_substitute (str, args, master);
/
/    Parameters:
/       Chix   str;       (command line)
/       Chix   args;      (command line args for macros)
/       Master master;    (master struct info for conference)
/
/    Purpose:
/       Parse a command line STR for any Caucus variables, and
/       replace them with the appropriate values.
/
/    How it works:
/       ARGS contains the "original" command line (if relevant),
/       which is used to supply the values of the command line
/       argument variables $1, $2, etc.  ARGS may be a nullchix.
/
/       Var_substitute() makes multiple passes over STR, in two groups.
/       First, var_substitute() makes multiple passes over STR until all
/       variables are substituted.  For example, if V0 contains
/       "$(userid)", then $(V0) will expand to the user's userid.
/
/       After all of the variables have been subtituted, var_substitute
/       makes one pass (left to right), doing its best to replace the
/       macro functions with the strings they evaluate to.  It is not
/       difficult to confuse this pass, but it won't blow up - it will
/       just give the wrong result.
/
/    Returns: 1
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  mac_subs()
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  gen/varsubs.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/11/89 10:40 New function. */
/*: CR 10/22/89 19:28 Add $(udnum), $(eot), $(cancel). */
/*: CR 10/24/89 12:37 Add $(name). */
/*: CR 10/24/89 13:23 Add 3rd diagnostic argument to sysudnum(). */
/*: CR 11/30/89 16:26 Add $(dict), $(start), $(thisitem). */
/*: CR 12/01/89 13:42 Fix to use all 30 variables! */
/*: JV  8/06/90 22:19 Add Site-Dependent Reg Info. */
/*: CS 11/30/90 12:59 Expand size of var[], lowvar[] to avoid bomb. */
/*: CR  1/07/92 18:10 Remove setopts references. */
/*: DE  4/02/92 14:58 Chixified */
/*: JX  5/27/92 16:19 Cast return from chxvalue. */
/*: JX  6/16/92 17:34 Only copy args if not NULLCHIX. */
/*: CX  6/22/92 11:09 Considerable rewrite! */
/*: CX  8/06/92 11:25 Add $(isorg), $(isread). */
/*: CR 10/11/92 22:05 Add comments. */
/*: CR 10/21/92 15:12 Remove trailing "," in init_varname initialization. */
/*: CR 12/02/92 15:13 Use LASTVAR in place of constant 42. */
/*: CR 12/10/92 15:09 chxalter arg int4. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: JJ  7/21/93 10:50 Add $(thisresp), stubs for $(attname...), $(attnum...). */
/*: JJ  7/21/93 16:24 Add support for macro functions. */
/*: JJ  7/22/93 16:36 Add mac func's: $(attname:), $(attnum:). */
/*: CK  7/27/93 17:11 Use int4s for fun_subs() args. */
/*: CP  7/31/93 13:26 Fun_subs() must a_fre_resp (resp) ! */
/*: CP  8/01/93 23:48 Fix off-by-one error in "$n" loading. */
/*: CK  8/02/93 12:45 Replace min() with (?:), as min is int's only. */
/*: CP  8/06/93 15:26 Use r_bound() to limit first/last responses. */
/*: CP  8/09/93 11:41 Add 'attach' arg to new_parse() call. */
/*: CP  8/18/93 17:24 Add $(terminal), $(showattach). */
/*: JV  9/03/93 15:40 Replace $(attnum with i:r tuple. */
/*: CP  9/03/93 12:08 Make multiple passes over command line. */
/*: CB  9/03/93 16:14 Fix scan/substitute for loop to use LASTVAR, not "41". */
/*: JV  9/07/93 10:57 Add $(transfer), more C macros. */
/*: JV  9/13/93 10:04 Allow mac var's nested in mac fun's. */
/*: CK  9/15/93 13:36 Add 'mod1st' arg to new_parse() call. */
/*: JV  9/21/93 17:32 Fix main for loop. */
/*: JV  9/27/93 13:32 Improve macro function parsing. */
/*: JZ  9/29/93 14:45 Refine macro function parsing. */
/*: JZ 10/04/93 17:08 Add macro functions $(atttype...) & $(attformat...) */
/*: CK 10/17/93 16:30 Add ERRCHK, ERRPOS args to new_parse() calls. */
/*: CR 10/17/93 17:50 Remove funky comment, unused 'rnum, inum'. */
/*: JZ 10/18/93 10:28 Expand comments, remove dust bunnies. */
/*: CP 11/18/93 15:35 Add cnum, inum args to r_bound() call. */
/*: CP 11/24/93 15:39 $(attnum) must use 'itemx:respx', not range! */
/*: CP 12/10/93 21:31 Add 'final' arg to r_bound() call. */
/*: CK 12/21/93 16:27 $(attnum expands to multiple i:r pairs... */
/*: CR  7/05/94 13:28 Use trim_filename() after a_get_resp(). */

#include <stdio.h>
#include "caucus.h"
#include "api.h"
#include "modifier.h"

#define  TWONULLS  nullchix, nullchix
#define  MAC_VAR 0
#define  MAC_FUN 1
#define  RLNULL  ((Rlist) NULL)

extern struct this_template thisitem;
extern Userreg              thisuser;
extern Chix   confid, confname;
extern short  confnum;
extern Chix   memvar[];
extern Chix   sitevar[];
extern union  null_t        null;

/*** Names of macro variables. */
#define ARGS_BEGIN             17
#define LASTVAR_NEEDS_MEM      ARGS_BEGIN + 8
#define MEMVARS_BEGIN          ARGS_BEGIN + 9
#define SITEVARS_BEGIN         MEMVARS_BEGIN + 10
#define LASTVAR                SITEVARS_BEGIN + 9
static char *init_varname[LASTVAR+1] = {
 /*  0 */ "$(userid)",
 /*  1 */ "$(confid)",
 /*  2 */ "$(confname)",
 /*  3 */ "$(confnum)",
 /*  4 */ "$(udnum)",      /* User directory number, e.g. 001 in USER001. */
 /*  5 */ "$(eot)",        /* End Of Text character string. */
 /*  6 */ "$(cancel)",     /* <CANCEL> character sequence. */
 /*  7 */ "$(name)",       /* User's full name. */
 /*  8 */ "$(dict)",       /* User's current dictionary number. */
 /*  9 */ "$(start)",      /* User's current STARTMENU. */
 /* 10 */ "$(thisitem)",   /* Number of current item. */
 /* 11 */ "$(isorg)",      /* Is this person an organizer? */
 /* 12 */ "$(isread)",     /* Is this person a readonly member? */
 /* 13 */ "$(thisresp)",   /* Number of current response. */
 /* 14 */ "$(terminal)",   /* User's terminal type. */
 /* 15 */ "$(showattach)", /* Show attachments? */
 /* 16 */ "$(transfer)",   /* File Transfer Protocol. */
 /* args        17 */ "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9",
 /* mem vars    26 */ "$(v0)", "$(v1)", "$(v2)", "$(v3)", "$(v4)",
                      "$(v5)", "$(v6)", "$(v7)", "$(v8)", "$(v9)",
 /* site-dep    36 */ "$(u0)", "$(u1)", "$(u2)", "$(u3)", "$(u4)",
                      "$(u5)", "$(u6)", "$(u7)", "$(u8)", "$(u9)"
};

/*** Names of macro functions. */
#define LASTFUN 3
static char *init_funname[LASTFUN+1] = {
 /*  0 */ "$(attname:",   /* attname:range */
 /*  1 */ "$(attnum:",    /* attnum:item:resp:fname. */
 /*  2 */ "$(atttype:",   /* atttype:item:resp. */
 /*  3 */ "$(attformat:", /* attformat:item:resp. */
};

/*** Names and values of variables. */
static Chix varname[LASTVAR+1];
static Chix varptr [LASTVAR+1];

static Chix funname[LASTFUN+1];
static Chix funptr [LASTFUN+1];

FUNCTION  var_substitute (str, args, master)
   Chix   str, args;
   Master master;
{
   static Chix  s_eot, s_onoff;
   static int    firsttime = 1;
   int    i, length, type, found, lookfor;
   Chix   comline, var, lowvar, onofftable;
   Chix   syscancel();
   int4   cval, pos, endvar, paren, colon, blank;
   int    index;

   ENTRY ("var_substitute", "");

   /*** The very first time var_subs() is called, allocate the chix
   /    for the names of all of the variables, and put the names in
   /    those chix. */
   if (firsttime) {
      firsttime = 0;
      for (index=0;  index <= LASTVAR;  index++) {
         varname[index] = chxalloc (L(10), THIN, "varsubs varname[]");
         chxofascii (varname[index], init_varname[index]);
      }
      for (index=0;  index <= LASTFUN;  index++) {
         funname[index] = chxalloc (L(10), THIN, "varsubs funname[]");
         chxofascii (funname[index], init_funname[index]);
      }
  
      /*** The first few variables and the functions need their own storage.
      /    The last 20 (v0-v9, u0-u9) are just pointers to the
      /    already existing memory variables. */
      for (i=0;   i<=LASTVAR_NEEDS_MEM;   ++i)
         varptr[i] = chxalloc (L(20), THIN, "varsubs varptr[]");

      for (i=0;   i<=LASTFUN; ++i)
         funptr[i] = chxalloc (L(20), THIN, "varsubs funptr[]");
   }

   /*** Load up "$n" command line arg values. */
   comline = (args==nullchix ? CQ("") : args);
   for (i=1;   i<10;   ++i)
     chxtoken (varptr[i+ARGS_BEGIN-1], nullchix, i, comline);

   /*** Load up memory variable values (as pointers to real thing). */
   for (i=0;   i<10;   ++i)  varptr[i+MEMVARS_BEGIN]  = memvar [i];
   for (i=0;   i<10;   ++i)  varptr[i+SITEVARS_BEGIN] = sitevar[i];

   var        = chxalloc (L(210), THIN, "varsubs var");
   lowvar     = chxalloc (L(210), THIN, "varsubs lowvar");
   onofftable = chxalloc ( L(20), THIN, "varsubs onofftable");

   /*** Load up miscellaneous ("$(userid", etc) variable values. */
   sysuserid (varptr[ 0]);
   chxcpy    (varptr[ 1], confid);
   chxcpy    (varptr[ 2], confname);
   chxclear  (varptr[ 3]);
   chxformat (varptr[ 3], CQ("%03d"), L(confnum), L(0), TWONULLS);
   chxclear  (varptr[ 4]);
   chxformat (varptr[ 4], CQ("%03d"), L(sysudnum (thisuser->id, -1, 0)), L(0),
               nullchix, nullchix);
   mdtable   ("set_Aeot",    &s_eot);
   value_setopts (varptr[5], 10, thisuser, s_eot);
   chxcpy    (varptr[ 6], syscancel());
   chxcpy    (varptr[ 7], thisuser->name);
   chxclear  (varptr[ 8]);
   chxformat (varptr[ 8], CQ("%d"),   L(thisuser->dict),  L(0), TWONULLS);
   chxcpy    (varptr[ 9],  thisuser->startmenu);
   chxclear  (varptr[10]);
   chxformat (varptr[10], CQ("%d"),   L(thisitem.number), L(0), TWONULLS);
   chxclear  (varptr[11]);
   chxformat (varptr[11], CQ("%d"),   L(thisuser->is_organizer), L(0), TWONULLS);
   chxclear  (varptr[12]);
   chxformat (varptr[12], CQ("%d"),   L(thisuser->read_only), L(0), TWONULLS);
   chxclear  (varptr[13]);
   chxformat (varptr[13], CQ("%d"),   L(thisitem.rnum), L(0), TWONULLS);
   chxcpy    (varptr[14], thisuser->terminal);
   mdstr     (onofftable, "onofftable", &s_onoff);
   chxtoken  (varptr[15], nullchix, 2 - thisuser->showattach, onofftable);
   chxcpy    (varptr[16], thisuser->transfer);

   /*** Now find each $ variable in turn, and either count up its size,
   /    or do the substitution... */

   for (lookfor=MAC_VAR; lookfor <= MAC_FUN; lookfor++, found=0) {
   do {
      for (found=pos=0;  cval=chxvalue(str, L(pos));  ++pos)  if (cval=='$') {
  
         type = MAC_VAR;
  
         /*** Pluck out the $ variable name, and *only* the name.
         /    Determine the type (variable or function). */
         if ( (blank = chxindex (str, pos, CQ(" "))) < 0)  blank = 1000;
         if ( (paren = chxindex (str, pos, CQ(")"))) < 0)  paren = 1000;
         if ( (colon = chxindex (str, pos, CQ(":"))) < 0)  colon = 1000;
         else if (colon < paren  &&  colon < blank) type = MAC_FUN;
  
         /*** (Note that the 'type' may appear to be MAC_FUN, even
         /    when the "$(..." thing is really a MAC_VAR.  But 
         /    the 2nd pass logic corrects for this.  Ugh.) */
         if (type == MAC_VAR)
            endvar = (blank < paren+1 ? blank : paren+1);
         else
            endvar = colon+1;
  
         if (cf_digit ((int) chxvalue (str, L(pos+1)))) endvar = pos+2;
         chxclear  (var);
         chxcatsub (var, str, pos, endvar - pos);
  
         chxcpy    (lowvar, var);
         jixreduce (lowvar);
 
         /* Only process macro functions on second pass, to allow
         /    macro variables nested in macro functions. */
/*       if (type == MAC_FUN && lookfor==MAC_FUN && paren < blank) { */
         if (type == MAC_FUN && lookfor==MAC_FUN) {
            if (fun_substitute (str, lowvar, pos, endvar, paren, master))
               found = 1;
            continue;
         }
  
         for (i=0;   i<=LASTVAR;   ++i)  if (chxeq (lowvar, varname[i])) {
            length = chxlen (varptr[i]);
            chxalter (str, pos, var, varptr[i]);
            pos  += length - 1;
            found = 1;
            break;
         }
      }
   } while (found == 1);
   } /* for (lookfor...) */

   chxfree (lowvar);
   chxfree (var);
   chxfree (onofftable);

   RETURN (1);
}

/*** FUN_SUBSTITUTE: substitute a macro function with the text it
/    represents.
/
/    fun_substitute (str, lowvar, begin, endname, paren, master)
/
/   Parameters:
/    Chix str,       // Command line which has already had all of its
/                    // macro variables substituted out.
/         lowvar;    // Lower-case copy of the function name, including the
/                    // initial "$(" and final ":"
/    int4 begin,     // Index of function name in STR
/         endname,   // Index of beginning of first argument in STR
/         paren,     // Index of closing paren in STR
/    Master master;  // Master struct info for conference
/
/ */
FUNCTION  fun_substitute (str, lowvar, begin, endname, paren, master)
   Chix   str, lowvar;
   int4   begin, endname, paren;
   Master master;
{
   char modifier[MODIFIERS+5];
   int  i, j, r, error=0, done=0, first, last, errpos, final;
   Chix  range, itemx, respx, rest, file, filetype, filefound;
   Chix  colon;
   Resp  resp;
   Rlist result, rptr;

   ENTRY ("fun_substitute", "");

   file   = chxalloc (L(16), THIN, "funsub file");
   rest   = chxalloc (L(16), THIN, "funsub rest");
   itemx  = chxalloc (L(16), THIN, "funsub itemx");
   respx  = chxalloc (L(16), THIN, "funsub respx");
   range  = chxalloc (L(16), THIN, "funsub range");
   filetype  = chxalloc ( L(20), THIN, "varsubs filetype");
   filefound = chxalloc ( L(80), THIN, "varsubs filefound");

   result = RLNULL;
   resp = a_mak_resp("varsubs resp");

   for (j=0; j<=LASTFUN; ++j) if (chxeq (lowvar, funname[j])) {
      switch (j) {
      case 0:    /* $(attname:Item:Resp)   */
      case 2:    /* $(atttype:Item:Resp)   */
      case 3:    /* $(attformat:Item:Resp) */
         chxcatsub (range, str, endname, paren-endname);
         chxbreak  (range, itemx, respx, CQ(":"));
         new_parse (range, &result, modifier, nullchix, confnum,
                            &thisitem, 1, 1, 1, &errpos);

         /* Now go through all of the resposes in RANGE and see if any have
         /  an attachment. */
         resp->cnum = confnum;
         for (rptr=result->next;  rptr!=RLNULL;  rptr=rptr->next) {
            for (i=rptr->i0;  i<=rptr->i1;  ++i) {
               resp->inum = i;
               if (NOT r_bound (&first, &last, &final, rptr, confnum, i, 
                                            master->responses[i]))  continue;
               for (r=first;  r<=last;  ++r) {
                  resp->rnum = r;
                  if (a_get_resp (resp, P_TITLE, A_WAIT) != A_OK) {
                     error=1; break; }
                  trim_filename (resp->attach->db_name);
                  if (chxlen (resp->attach->db_name) > 0) {
                     done=1; error=0; break; }
               }
               if (done) break;
            }
            if (done) break;
         }
  
         chxcat   (range, CQ(")"));
         chxalter (str, begin, range, CQ(""));       /* rm the range text */

         if (done)                                   /* subst the name */
            switch (j) {
            case 0:   /* attname */
               chxalter (str, begin, lowvar, resp->attach->db_name);
               break;
            case 2:   /* atttype */
               if (resp->attach->format->data_type == DATA_ASCII)
                  mdstr (filetype, "mac_Tascii", null.md);
               else
                  mdstr (filetype, "mac_Tbinary", null.md);
               chxalter (str, begin, lowvar, filetype);
               break;
            case 3:   /* attformat */
               chxalter (str, begin, lowvar, resp->attach->format->format);
               break;
            }
         else     /* Couldn't find it, substitute with null string. */
            chxalter (str, begin, lowvar, CQ(""));
         break;

      case 1:    /* $(attnum:Item_Range:Resp_Range:Filename) */
         colon = chxquick (":", 8);
         chxcatsub (range, str, endname, paren-endname);
         chxbreak  (range, itemx, rest, colon);
         chxbreak  (rest,  respx, file, colon);
         chxconc   (itemx, itemx, colon);
         chxcat    (itemx, respx);

         new_parse (itemx, &result, modifier, nullchix, confnum,
                           &thisitem, 1, 1, 1, &errpos);

         /* Now go through all of the resposes in RANGE and see if any have
         /  an attachment with the name FILE. */
         resp->cnum = confnum;
         for (rptr=result->next;  rptr!=RLNULL;  rptr=rptr->next) {
            for (i=rptr->i0;  i<=rptr->i1;  ++i) {
               resp->inum = i;
               if (i == 0)       continue;
               if (NOT r_bound (&first, &last, &final, rptr, confnum, i, 
                                           master->responses[i]))  continue;
               for (r=first;  r<=last;  ++r) {
                  resp->rnum = r;
                  if (a_get_resp (resp, P_TITLE, A_WAIT) != A_OK) 
                     { error=1;   break; }
                  trim_filename (resp->attach->db_name);
                  if (chxindex  (resp->attach->db_name, L(0), file) == 0) {
                     chxformat (filefound, CQ("%d:%d "), L(i), L(r),
                                           nullchix, nullchix);
                  }
               }
            }
         }

         chxcat   (range, CQ(")"));
         chxalter (str, begin, range, CQ(""));    /* rm the range text */
  
         if (EMPTYCHX(filefound))  chxofascii (filefound, "0");
         chxalter (str, begin, lowvar, filefound);
         break;

      default:
        buglist ("Past valid values in switch in fun_substitute.\n");
        error = 1; break;
      }
      if (error || done) break;
   }

   a_fre_rlist (result);
   a_fre_resp  (resp);
   chxfree (respx);
   chxfree (itemx);
   chxfree (range);
   chxfree (file);
   chxfree (rest);
   chxfree (filetype);
   chxfree (filefound);

   RETURN (done && NOT error ? 1 : 0);
}
