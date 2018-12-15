
/*** SHOW_FORMAT.  Show text according to format string.
/
/    ok = show_format (out, fstr, item, thisresp, resp1, resp2, lnum, 
/                           the_title, header);
/
/    Parameters:
/       int   ok;          (success?)
/       int   out;         (Unit number for output)
/       Chix  fstr;        (format for display)
/       int   item;        (item number of text)
/       int   thisresp;    (response number of text)
/       int   resp1;       (1st response number in range)
/       int   resp2;       (2nd response number in range)
/       int   lnum;        (start with line number lnum)  (ignored)
/       Chix  the_title;   (title of item)
/       Chix  header;      (header of item)               (ignored)
/ 
/    Purpose:
/       Display the text of a response according to a particular format.
/
/       On unit OUT, display ITEM response THISRESP, starting with line
/       LNUM, according to the format string contained in FSTR.  If
/       THE_TITLE is not empty, it contains the title  for this item.
/       If HEADER is not empty, it contains the header for this item.
/
/    How it works:
/       Display response RESP to item ITEM, starting with line number LNUM,
/       according to the format string in FSTR.  If a title or header is
/       needed, look first in THE_TITLE and HEADER, and if not there, go
/       to the response partfile for the data.
/
/       The special format codes consist of an "&" followed by an optional
/       length field followed by a single letter code.  The codes are:
/          &a   author (not including "(userid)")
/          &b   bytes of text in response  (actually chixels)
/          &c   conference name
/          &d   date
/          &e   extent (size) of attached file (empty if no file)
/          &f   file name of attached file (empty if no file)
/          &h   hostname
/          &i   item number
/          &l   number of responses to this item
/          &n   lines of text in response
/          &o   object type of attached file (empty if no file)
/          &r   response number
/          &R   2nd response number (ignored for all other purposes)
/          &s   subject or title
/          &t   time
/          &u   userid of author
/          &x   text (of item or response, or else brief introduction)
/          &X   a single line of the item/response/whatever
/          &z   tab to this position (display width)
/
/    Returns: 0   if the response does not exist
/             1   on successful completion
/             n>0 if user interrupts display at line n.
/
/    Side effects:      none
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  resp/showform.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/06/90 17:45 New function. */
/*: CR  8/24/90 14:17 Replace all '%' with '&'. */
/*: CR  9/26/90 10:31 Make Lint happy if JAPAN is OFF. */
/*: CR  1/29/91 17:25 &a code now split into &a and &u. */
/*: CR  6/19/91 16:31 Add &h host information. */
/*: CX 10/20/91 12:46 Chixify. */
/*: CR 10/29/91 11:45 Add confnum arg to getline() call. */
/*: CR  1/20/92 16:51 Add cnum arg to load_header() call. */
/*: JX  5/15/92 17:19 Return number of lines if user interrupt. */
/*: CX  6/18/92 14:20 buildline() call needs ALLCHARS if length <= 0. */
/*: CR 10/07/92 22:30 Chxclear(how_d) before chxformat! */
/*: CR 10/11/92 22:31 Remove CQ from buildline() calls. */
/*: JV 10/15/92 15:59 Reorder chxfree's. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: CL 12/11/92 13:48 Long Chxvalue N. */
/*: JV  4/02/93 11:38 Add attach arg to load_header(). */
/*: JV  6/07/93 17:00 Add text length info. */
/*: CP  6/30/93 17:23 Add 'resp2' argument, &R and &X. */
/*: CP  7/02/93 14:14 Integration of 2.5's. */
/*: CK  7/28/93 14:37 Use THE_TITLE if possible, else a_get_resp() it. */
/*: CP  8/01/93 10:50 Add a_mak...() name arg. */
/*: CP  8/23/93 16:36 Add attachment codes: &f, &e, &o. */
/*: CK  9/20/93 15:15 &R appears only if resp2 != resp. */
/*: CR 11/11/93 13:33 Change args: resp->thisresp, add resp1. */

#include <stdio.h>
#include "caucus.h"
#include "api.h"
#define  ATNULL         ( (Attachment) NULL)
#define  HAS_ATTACH(r)  (r->attach != ATNULL  &&  r->attach->size > 0)

extern struct master_template master;
extern union  null_t          null;
extern Chix                   confname;
extern short                  confnum;

FUNCTION  show_format (out, fstr, item, thisresp, resp1, resp2, lnum, 
                            the_title, header)
   int    out, item, thisresp, resp1, resp2, lnum;
   Chix   fstr, the_title, header;
{
   static Chix atsign = nullchix;
   int    success, got_text=0;
   int4   s, i, t, pos, v, length;
   Chix   line, otype;
   Chix   authorname, athost, userid, host, newline, title;
   Chix   temp, how_s, how_d, time, date;
   Resp   resp_obj, tresp;
   Attach_Def  adformat;

   ENTRY ("showformat", "");

   /*** Get header info about the response from  a_get_resp(). */
   resp_obj       = a_mak_resp("showform resp_obj");
   resp_obj->cnum = confnum;
   resp_obj->inum = item;
   resp_obj->rnum = thisresp;
   if (a_get_resp (resp_obj, P_TITLE, A_WAIT) != A_OK   ||
         EMPTYCHX (resp_obj->owner)) {
       a_fre_resp (resp_obj);
       RETURN (0);
   }

   athost     = chxalloc ( L(20), THIN, "show_form athost");
   authorname = chxalloc ( L(60), THIN, "show_form authorname");
   date       = chxalloc ( L(20), THIN, "show_form date");
   host       = chxalloc ( L(20), THIN, "show_form host");
   how_d      = chxalloc (L(100), THIN, "show_form how_d");
   how_s      = chxalloc (L(100), THIN, "show_form how_s");
   line       = chxalloc ( L(20), THIN, "show_form line");
   newline    = chxalloc (  L(8), THIN, "show_form newline");
   temp       = chxalloc (L(100), THIN, "show_form temp");
   time       = chxalloc ( L(20), THIN, "show_form time");
   title      = chxalloc ( L(80), THIN, "show_form title");
   userid     = chxalloc ( L(20), THIN, "show_form userid");
   chxcpy (newline, CQ("\n"));

   /*** Pluck out the "pure" userid, host, and author names. */
   chxbreak  (resp_obj->owner, userid, host, CQ("@"));
   removeid  (authorname, resp_obj->author, userid, athost);
   mdstr     (athost, "peo_Katsign", &atsign);
   chxcat    (athost, host);

   /*** Get the time and date. */
   chxtoken (time, nullchix, 2, resp_obj->date);
   chxtoken (date, nullchix, 1, resp_obj->date);

   /*** Copy the title, if it exists. */
   if (the_title != nullchix  &&  NOT EMPTYCHX(the_title))
      chxcpy (title, the_title);
   if (EMPTYCHX(title)  &&  thisresp==0)  chxcpy (title, resp_obj->title);

   /*** Loop through the format string, processing all "&x" directives,
   /    and displaying all other text as is.  */
   buildline (out, nullchix, L(0), L(-1), BLINIT);
   success = 1;
   pos     = L(0);   /* Char position in text of response.  See 'x', 'X'. */
   for (s=0;   v = chxvalue (fstr, L(s));   ) {

      /*** If no more "&"s left, display to the end of the string. */
      if ( (t = chxindex (fstr, s, CQ("&"))) < 0) {
         buildline (out, fstr, s, ALLCHARS, BLADD);
         break;
      }

      /*** Treat escaped "&"s as ordinary text to be displayed. */
      else if (chxvalue (fstr, L(t+1)) == '&') {
         buildline (out, fstr, s, t, BLADD);
         s = t + 2;
         continue;
      }

      /*** Remaining code in loop processes "&x" forms.   Start by
      /    displaying all text before the "&". */
      buildline (out, fstr, s, t-1, BLADD);
      s = t + 1;

      /*** Parse out the string length, including "-" => left-justify, and
      /    make a chxformat string to use to display the info. */
      length = chxint4 (fstr, &s);
      if (s == t+1) {
         length = 0;
         chxcpy    (how_s, CQ("%s"));
         chxcpy    (how_d, CQ("%d"));
      }
      else {
         chxclear  (how_s);
         chxclear  (how_d);
         chxformat (how_s, CQ("%%%ds"), length, L(0), nullchix, nullchix);
         chxformat (how_d, CQ("%%%dd"), length, L(0), nullchix, nullchix);
      }

      /*** Decide what to display according to "x" code in "&x". */
      chxclear (temp);
      v = chxvalue (fstr, L(s));
      switch (v) {

         case ('a'):   /* Author */
            chxformat (temp, how_s, L(0), L(0), authorname, nullchix);   break;

         case ('b'):   /* Bytes in text */
            if (NOT got_text) a_get_resp (resp_obj, P_RESP, A_WAIT);
            got_text = 1;
            chxformat (temp, how_d, L(resp_obj->text->chars), L(0),
                       nullchix, nullchix);                          break;

         case ('c'):   /* Conference */
            chxformat (temp, how_s, L(0), L(0), confname, nullchix);     break;

         case ('d'):   /* Date */
            chxformat (temp, how_s, L(0), L(0), date, nullchix);         break;

         case ('e'):   /* attachment file Extent */
            if (HAS_ATTACH(resp_obj))
                 chxformat (temp, how_d, resp_obj->attach->size, L(0),
                                                 nullchix, nullchix);
            else chxformat (temp, how_s, L(0), L(0), CQ(""),   nullchix);
            break;

         case ('f'):   /* attachment File name */
            chxformat (temp, how_s, L(0), L(0), 
               HAS_ATTACH(resp_obj) ? resp_obj->attach->db_name : CQ(""),
                                               nullchix);
            break;

         case ('i'):   /* Item */
            chxformat (temp, how_d, (int4) item, L(0), nullchix, nullchix);
            break;

         case ('l'):   /* Last response # */
            chxformat (temp, how_d, (int4) master.responses[item], L(0),
                       nullchix, nullchix);                          break;

         case ('n'):   /* Lines of text */
            if (NOT got_text) a_get_resp (resp_obj, P_RESP, A_WAIT);
            got_text = 1;
            chxformat (temp, how_d, L(resp_obj->text->lines), L(0),
                       nullchix, nullchix);                          break;

         case ('o'):   /* attachment file Object type. */
            if (HAS_ATTACH(resp_obj)  && 
               (adformat = resp_obj->attach->format) != NULL)
                 otype = adformat->format;
            else otype = CQ("");
            chxformat (temp, how_s, L(0), L(0), otype, nullchix);
            break;

         case ('r'):   /* This response # */
            chxformat (temp, how_d, (int4) resp1,  L(0), nullchix, nullchix);
            break;

         case ('R'):   /* 2ndary response # */
            if (resp2 != resp1)
               chxformat (temp, how_d, (int4) -resp2, L(0), nullchix, nullchix);
            break;

         case ('t'):   /* Time */
            chxformat (temp, how_s, L(0), L(0), time, nullchix);         break;

         case ('u'):   /* Userid */
            chxformat (temp, how_s, L(0), L(0), userid, nullchix);       break;

         case ('h'):
            if (NOT EMPTYCHX(host))
               chxformat (temp, how_s, L(0), L(0), athost, nullchix);
            break;

         case ('z'):                                            /* Tab pos. */
            buildline (out, temp, L(0), L(0), (int) length);            break;

         /*** Subject, i.e. item title.  */
         case ('s'):
            if (EMPTYCHX(title)) {
               tresp       = a_mak_resp("showform tresp");
               tresp->cnum = confnum;
               tresp->inum = item;
               tresp->rnum = 0;
               a_get_resp (tresp, P_TITLE, A_WAIT);
               chxcpy (title, tresp->title);
               a_fre_resp (tresp);
            }
            chxformat (temp, how_s, L(0), L(0), title, nullchix);
            break;

         /*** Text of response/item/whatever.  In this case only, the
         /    length field refers to the number of spaces to indent
         /    each line of text. */
         case ('x'):
            if (NOT got_text) a_get_resp (resp_obj, P_RESP, A_WAIT);
            got_text = 1;
            if (length > 0)
               chxformat (temp, how_s, L(0), L(0), CQ(" "), nullchix);

            for (i=0;   chxnextline (line, resp_obj->text->data, &pos) > 0;
                        ++i) {
               if (length > 0)  buildline (out, temp, L(0), ALLCHARS, BLADD);
               buildline (out, line,     L(0), ALLCHARS, BLADD);
               buildline (out, newline,  L(0), L(0),       BLADD);
               if (sysbrktest())  { success = i;   break; }
            }
            chxclear (temp);
            break;

         /*** Just a single line of the text of the item/resp. */
         case ('X'):
            if (NOT got_text) a_get_resp (resp_obj, P_RESP, A_WAIT);
            got_text = 1;
            chxnextline (temp, resp_obj->text->data, &pos);
            break;

         default:   break;
      }

      /*** Enforce maximum length of info, and display it. */
      if (sysbrktest())  break;
      buildline (out, temp, L(0), (length>0 ? length-1 : ALLCHARS), BLADD);
      ++s;
   }
   buildline (out, nullchix, L(0), L(-1), BLFLUSH);

   SUCCEED;

done:
   a_fre_resp (resp_obj);
   chxfree (athost);
   chxfree (authorname);
   chxfree (date);
   chxfree (host);
   chxfree (line);
   chxfree (how_s);
   chxfree (how_d);
   chxfree (newline);
   chxfree (time);
   chxfree (title);
   chxfree (temp);
   chxfree (userid);

   RETURN (success);
}
