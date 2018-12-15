
/*** FUNC_AD.   Return the resulting value of ad_xxx(), chg_xxx() functions.
/
/    func_ad (result, what, arg, conf);
/ 
/    Parameters:
/       Chix                result;  (append resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/       struct sweb_conf_t *conf;    (server configuration info)
/
/    Purpose:
/       Append value of ad_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/ 
/    Known bugs:
/      Some of the ad_resp and ad_item code could be combined...
/
/    Home:  funcad.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/06/95 22:23 New function. */
/*: CR  3/16/00 15:49 Add chg_author() function.  Sanity-check CNUM. */
/*: CR  6/05/04 $add_resp() can take zero-long text (see pos assignments) */
/*: CR  7/02/04 Add logger() debugging when add_response fails. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern Cstat  g_cstat;
extern Pstat  g_pstat;
extern Vartab g_icache;

extern int   cml_debug;
extern int   g_wrap;
extern int   g_mytext;

FUNCTION  func_ad (
   Chix   result,
   char  *what,
   Chix   arg,
   struct sweb_conf_t *conf)
{
   static Resp  resp  = (Resp)  NULL;
   static Chix  author, temp;
   char         tempstr[100];
   int4   cnum, item, rnum, pos, pos0, code, prop, zp;
   int    ok, add, is_add_resp, is_add_item;
   ENTRY ("func_ad", "");

   /*** Initialization. */
   if (resp == (Resp) NULL)  {
      resp   = a_mak_resp ("func_ad resp");
      author = chxalloc (L(50), THIN, "func_ad author");
      temp   = chxalloc (L(50), THIN, "func_ad temp");
   }

   /*** $AD_AUTHOR(name).   Prepare psuedonymous author name for next ad. */
   if (streq (what, "ad_author")) {
      chxcpy (author, arg);
      RETURN(1);
   }

   /*** Most ad_xxx() functions take a conference number and a
   /    property number. */
   pos  = L(0);
   cnum = chxint4 (arg, &pos);
   prop = chxint4 (arg, &pos);
   if (pos  == L(0))  { BAD("NO CNUM");    RETURN(1); }
   if (cnum <=   0)   { BAD("NO CONF");    RETURN(1); }

   is_add_resp = (streq (what, "ad_resp")  ||  streq (what, "add_resp"));
   is_add_item =  streq (what, "add_item");

   /*** ADD RESPONSE: $ad_resp (cnum prop inum text) or
   /                  $add_resp(cnum prop inum owner author copied text)  */
   if (is_add_resp) {
      item = chxint4 (arg, &pos);
      if (pos == L(0))  { BAD("NO INUM");    RETURN(1); }

      ok = (conf_access (cnum) >= privValue("include"));
      if (ok) {
         resp->cnum     = cnum;
         resp->inum     = item;
         resp->textprop = prop;
         resp->bits     = (conf_access(cnum) >= privValue("instructor") ? M_ISORG : 0);
         chxclear  (resp->copied);
         if (streq (what, "add_resp")) {
            chxtoken  (resp->owner,  nullchix, 4, arg);
            chxtoken  (resp->author, nullchix, 5, arg);
            chxrepval (resp->author, L(0), THIN, L(1), L(' '));
            chxtoken  (temp, nullchix, 6, arg);
            if (chxvalue(temp, L(0)) != '0') {
               sysuserid (resp->copied);
               chxcatval (resp->copied, THIN, L(' '));
               chxrepval (temp, L(0),   THIN, L(1), L(' '));
               chxcat    (resp->copied, temp);
            }
            pos = chxtoken (resp->text->data, nullchix, 7, arg) - 1;
            if (pos < 0)  pos = chxlen(arg)-1;
         }
         else {
            sysuserid  (resp->owner);
            chxclear   (resp->author);
            if (NOT EMPTYCHX(author))  chxcpy  (resp->author, author);
            else                       func_my (resp->author, "my_name", arg);
            chxclear   (author);
         }
         chxclear   (resp->text->data);
         col_wrap   (resp->text->data, arg, ++pos, g_wrap);
         ok = a_add_resp (resp, 0, A_WAIT);
         if (ok != A_OK) {
            char temp[2000];
            sprintf (temp, "JD:a_add_resp(%d,%d)=%d", cnum, item, ok);
            logger  (1, LOG_FILE, temp);
         }
  
         ok = (ok == A_OK);

         /*** If adding response succeeded, reload conf status and
         /    rewrite pers status (if changed). */
         if (ok) {
            g_cstat->cnum = 0;
            touch_it (cnum, 0, 0);
            if (mod_add_pstat (g_pstat, resp->inum, resp->rnum, g_mytext))
               a_chg_pstat (g_pstat, P_EVERY, A_WAIT);
         }
      }
/*    chxcatval (result, THIN, (ok ? '1' : '0'));  */

      if (ok)  sprintf (tempstr, "%d", resp->rnum);
      else     strcpy  (tempstr, "0");
      chxofascii (temp, tempstr);
      chxcat     (result, temp);
   }

   /*** $CHG_RESP(cnum prop inum rnum text).   CHANGE RESPONSE. */
   else if (streq (what, "chg_resp")) {
      pos0 = pos;
      item = chxint4 (arg, &pos);
      rnum = chxint4 (arg, &pos);
      if (pos == pos0)  { BAD("NO INUM");    RETURN(1); }

      ok = (conf_access (cnum) >= privValue("include"));
      if (ok) {
         resp->cnum     = cnum;
         resp->inum     = item;
         resp->rnum     = rnum;
         resp->textprop = prop & M_TEXTTYPE;
         resp->bits     = prop & M_BITS;
         chxclear   (resp->text->data);
         col_wrap   (resp->text->data, arg, ++pos, g_wrap);
         code = a_chg_resp (resp, P_TEXT | P_BITS, A_WAIT);
         ok = (code == A_OK);
      }
      chxcatval (result, THIN, (ok ? '1' : '0'));
      func_re (result, "re_clear", arg, 0);   /* Clear resp cache! */
   }

   /*** $CHG_AUTHOR(cnum inum rnum owner author).   Organizer only! */
   else if (streq (what, "chg_author")) {
      pos  = 0;
      cnum = chxint4 (arg, &pos);
      pos0 = pos;
      item = chxint4 (arg, &pos);
      rnum = chxint4 (arg, &pos);
      if (pos == pos0)  { BAD("NO INUM");    RETURN(1); }

      ok = (conf_access (cnum) >= privValue("organizer"));
      if (ok) {
         ok  = 0;
         resp->cnum = cnum;
         resp->inum = item;
         resp->rnum = rnum;
         pos = chxtoken (resp->owner, nullchix,  4, arg);
         pos = chxtoken (nullchix, resp->author, 5, arg);
         if (pos > 0) {
            code = a_chg_resp (resp, P_OWNER | P_NAME, A_WAIT);
            ok = (code == A_OK);
         }
      }
      chxcatval (result, THIN, (ok ? '1' : '0'));
      func_re (result, "re_clear", arg, 0);   /* Clear resp cache! */
   }

   /*** $CHG_TITLE(cnum prop inum title).   CHANGE RESPONSE. */
   else if (streq (what, "chg_title")) {
      item = chxint4 (arg, &pos);
      if (pos == L(0))  { BAD("NO INUM");    RETURN(1); }
      ++pos;

      ok = (conf_access (cnum) >= privValue("include")  &&  chxvalue (arg, pos) != L(0));
      if (ok) {
         resp->cnum     = cnum;
         resp->inum     = item;
         resp->rnum     = 0;

         chxclear   (resp->title);
         chxcatsub  (resp->title, arg, pos, ALLCHARS);
         code = a_chg_resp (resp, P_TITLE, A_WAIT);
         ok = (code == A_OK);
         resp->exists = ok;
         item_cache (cnum, item, g_icache, resp);
      }
      chxcatval (result, THIN, (ok ? '1' : '0'));
      func_re (result, "re_clear", arg, 0);   /* Clear resp cache! */
   }

   /*** ADD ITEM: $ad_item (cnum prop title text) or
   /              $add_item(cnum prop owner author title text)  */
   else if (is_add_item) {

      /*** Can this user add an item? */
      chxclear (temp);
      func_co  (temp, "co_add", arg, conf, 0);
      zp  = L(0);
      add = chxint4 (temp, &zp);
      ok  = (conf_access(cnum) + add*10 >= privValue("instructor"));

      if (ok) {
         resp->cnum     = cnum;
         resp->inum     = -1;
         resp->textprop = prop;
         resp->bits     = (conf_access(cnum) >= privValue("instructor") ? M_ISORG : 0);
         chxclear (resp->copied);

         if (streq (what, "add_item")) {
            chxtoken (resp->owner,  nullchix, 3, arg);
            chxtoken (resp->author, nullchix, 4, arg);
            chxrepval(resp->author, L(0), THIN, L(1), L(' '));
            chxtoken (resp->title, nullchix, 5, arg);
            chxrepval(resp->title, L(0), THIN, L(1), L(' '));
            chxtoken (temp, nullchix, 6, arg);
            if (chxvalue(temp, L(0)) != '0') {
               sysuserid (resp->copied);
               chxcatval (resp->copied, THIN, L(' '));
               chxrepval (temp, L(0),   THIN, L(1), L(' '));
               chxcat    (resp->copied, temp);
            }
            pos = chxtoken (resp->text->data, nullchix, 7, arg);
         }
         else {
            sysuserid  (resp->owner);
            chxclear   (resp->author);
            if (NOT EMPTYCHX(author))  chxcpy  (resp->author, author);
            else                       func_my (resp->author, "my_name", arg);
            chxclear   (author);
            while (chxvalue (arg, pos) == ' ')  ++pos;
            chxnextline (resp->title, arg, &pos);
         }

         chxclear    (resp->text->data);
         col_wrap    (resp->text->data, arg, pos, g_wrap);
         code = a_add_resp (resp, 0, A_WAIT);
         ok = (code == A_OK ? resp->inum : 0);

         /*** If adding item succeeded, reload conf status and
         /    rewrite pers status (if changed). */
         if (ok) {
            g_cstat->cnum = 0;
            touch_it (cnum, 0, 0);
            if (mod_add_pstat (g_pstat, resp->inum, resp->rnum, g_mytext))
               a_chg_pstat (g_pstat, P_EVERY, A_WAIT);
         }
      }
      sprintf    (tempstr, "%d", ok);
      chxofascii (temp, tempstr);
      chxcat     (result, temp);
   }

   else  chxcat (result, CQ("<I>NO FUNC</I>"));

   RETURN(1);
}
