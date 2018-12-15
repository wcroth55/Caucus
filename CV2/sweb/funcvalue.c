
/*** FUNC_VALUE.   Return the resulting value of a particular function.
/
/    func_value (result, name, arg, conf, cmlargs, vars, form,
/                        ignore, incvars, macs, cfile, otab, tg, aref);
/ 
/    Parameters:
/       Chix          result;  (append resulting value here)
/       Chix          name;    (function name)
/       Chix          arg;     (function argument)
/       SWconf        conf;    (server configuration info)
/       char         *cmlargs; (CML script arguments)
/       Vartab        vars;    (CML variable list)
/       Vartab        form;    (form data variable list)
/       int           ignore;  (no purpose)
/       Chix          incvars; (text of 'include' variables)
/       Vartab        macs;    (defined macros)
/       CML_File      cfile;   (original cml file)
/       Obtab         otab;    (object table)
/       Target        tg;      (output target)
/       Chix          aref;    (array reference from $array_eval())
/
/    Purpose:
/       Append value of function NAME with arguments ARG to RESULT.
/
/    How it works:
/       $host()             hostname
/       $pid()              pid & security code
/       $shell(F)           output from running program F
/       $lower(s)           lower-case string S
/       $arg(n)             CML argument number N (1st is 1)
/
/    Returns:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  funcvalue.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/10/95 17:48 New function. */
/*: CR  3/25/95 13:52 Convert to chix. */
/*: CR  4/05/00 15:21 Add $config() function. */
/*: CR  4/27/00 13:41 Expand http_referer(), add http_accept(). */
/*: CR  2/08/01 18:00 Add $array...() functions. */
/*: CR  2/09/01 14:50 Add output Target; add $includecml(). */
/*: CR  7/07/01 21:00 Fix $min(), $max() initial values. */
/*: CR 10/16/01 13:50 Add groupmat (permnames, searchall). */
/*: CR 10/30/01 11:36 Use prot_ functions instead of protect variable. */
/*: CR 01/25/02 13:40 Make $random() no-arg proof. */
/*: CR  4/02/02 12:40 Cookie login support: auth_() functions */
/*: CR  6/28/02 change http.accept to http.xaccept to placate AIX. */
/*: CR  8/07/02 Allow auth_cookie() to "log out" as well. */
/*: CR  8/21/02 auth_cookie() embeds process id in cookie name. */
/*: CR 10/16/02 Add $sleep(n) */
/*: CR 10/25/02 Add $escsingle(n) */
/*: CR 11/26/02 Add $charvals(s) */
/*: CR  5/23/03 Add caucus_version(), logevent(). */
/*: CR  6/03/04 Add mkdir(). */
/*: CR  7/05/04 $readfile() converts CR-LF's to LF's; add $unhtml(), $unamp().*/
/*: CR 11/09/04 Add $h2url(). */
/*: CR 12/12/04 Add $encode64(). */
/*: CR 02/17/05 $readln() should handle *any* length line! */
/*: CR 11/23/05 Add $build_name_cascade(). */
/*: CR 09/13/07 Allow item_set and item_del only if NOT protect. */
/*: CR 05/13/08 Add $ip_addr(). */
/*: CR 08/28/09 Make $auth_cookie() with no args logout from auth method 3 as well. */

#include <stdio.h>
#include <ctype.h>
#include "chixuse.h"
#include "sweb.h"
#include "unitcode.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include "api.h"

#if UNIX
#include <unistd.h>
#endif

#define  DONE     goto done
#define  TEMPMAX  2000
#define  STRMED    300
#define  HANDLES   100
#define  LOCKMAX 10000000   /* Ten million */

extern int           cml_debug;
extern struct http_t http;
extern int           g_wrap;
extern int           output_default;

extern long memused;
static long oldused = 0;
static int  level = 0;

FUNCTION  func_value (Chix result, Chix name, Chix arg,
                      SWconf conf, char *cmlargs,
                      Vartab vars, Vartab form,
                      int ignore, Chix incvars, Vartab macs, CML_File cfile,
                      Obtab otab, Target tg, Chix aref)
{
   static FILE *handle[HANDLES];
   static Chix blanks  = (Chix) NULL;
   static Chix wildcard;
   static char cookie_path[STRMED] = { 0 };
   static int  allow_functions  = 1;
   static int  array_eval_depth = 0;
   Chix   interim, word1, word2, word3;
   Chix   t1, t2;
   Chix   newaref, aetemp;
   FILE  *fp;
   char   temp[TEMPMAX], temp2[STRMED], temp3[STRMED], what[100], tmpdir[128],
          oname[100];
   char  *mode, *tp;
   int    num, width, sign, percent, first, type, lang, blank, fd, lnum, mnum;
   int    success, last, slash, permission, save_mask, q0, q1, nofunc, slot;
   int4   which, len, op1, op2, op3, pos, pos2;
   int4   val0, val1, val2, val3, persist;
   double dbl0, dbl1, dbl2, dbl3;
   int4   unique();
   int4   ccode[2];
   int    year, month, day, hour, minute, sec, wday, broke, contd, ok, is_int;
   int    precise, digits, pg, found, protect;
   int    pid;
   long   tstart;
   char   map[256];
   char  *url, *copy2lib();
   Target ctg, target();
   char  *format_number();
   char  *caucus_version();
   long   systimems();
   ENTRY ("func_value", "");

   ++level;

   /*** Initialize static data elements. */
   if (blanks == (Chix) NULL) {
      blanks   = chxalloc ( L(100), THIN, "func_value blanks");
      wildcard = chxalsub (CQ("*"), L(0), L(5));
      chxformat (blanks, chxquick("%100s",0), L(0), L(0),
                         chxquick(" ",    1), nullchix);
      for (num=0;   num<HANDLES;   ++num)  handle[num] = NULL;
      srand (((unsigned int) time(NULL)) % 1019);
   }
   interim = chxalloc (L(1000), THIN, "func_value interim");
   word1   = chxalloc ( L(100), THIN, "func_value word1");
   word2   = chxalloc ( L(100), THIN, "func_value word2");
   word3   = chxalloc ( L(100), THIN, "func_value word3");

   /*** Turn the NAME into an ascii string WHAT.   Decode the field
   /    WIDTH, if any. */
   ascofchx (what, name, L(0), L(100));
   width = 0;
   if ( (percent = strindex (what, "%")) >= 0) {
      what[percent] = '\0';
      sscanf (what+percent+1, "%d", &width);
   }

   /*** Begin evaluation functions... sorted into blocks by the
   /    1st letter of the function name, to improve speed.
   /    Isolated functions are directly coded here.  Related groups
   /    of functions have their own separate module.
   /
   /    Note that some basic functions are always allowed.  Others are
   /    unavailable if $passcheck() failed or "protected" mode is on. */
   protect = prot_top();

   chxclear (interim);
  
   nofunc = 0;

   /*** Debugging: if all else fails, log every single CML function call. */
/* sprintf (temp, "%s (%s)", what, ascquick(arg));
   logger (1, LOG_FILE, temp);
*/
  

   switch (what[0]) {

   case 0: /* no function name => variable expansion, e.g. "$(varname)". */
      chxtoken (word1, nullchix, 1, arg);

      /*** Object reference. */
      if (chxvalue (word1, L(0)) == 0177) {
         if (NOT ob_value (interim, word1, otab, cfile))
               chxofascii (interim, otab->err);
      }

      /*** Ordinary variable reference. */
      else for (num=0;   num<vars->used;   ++num) {
         if (chxeqf (vars->name[num], word1)) {
            chxcpy  (interim, vars->value[num]);
            break;
         }
      }
      break;

   case 'a':
      if      (streq (what, "admin_mail"))
         chxcat (result, CQ(conf->adminmail));

      else if (streq (what, "and")) {  /* arbitrary # of args */
         val1 = val2 = 1;
         pos  = pos2 = L(0);
         do {
            pos  = pos2;
            val1 = val1  &&  val2;
            val2 = chxint4 (arg, &pos2);
         } while (pos2 != pos);
  
         sprintf    (temp, "%d", (pos2 == L(0) ? L(0) : val1));
         chxofascii (interim, temp);
      }

      else if (streq (what, "append")) {
         val1 = '0';
         if (allow_functions && NOT protect) {
            chxtoken (word1, nullchix, 1, arg);
            ascofchx (temp, word1, L(0), L(200));
            mode = "a";
            nt_chkfile (temp, 0700);
            if ( (fp = fopen (temp, mode)) != NULL) {
               val1 = '1';
               pos = chxtoken (word1, nullchix, 2, arg);
               if (pos >= 0) write_from_jix (fp, arg, pos, conf->diskform+1);
               fclose (fp);
            }
         }
         chxcatval (interim, THIN, val1);
      }

      else if (streq (what, "arg")) {
         chxnum     (arg, &which);
         strtoken   (temp, (int) which, cmlargs);
         chxofascii (interim, temp);
      }

      else if (streq (what, "addtarget")) {
         func_addtarget (arg, conf);
         chxcat (interim, arg);
      }

      else if (streq (what, "auth_method")) {
         if (NOT protect) {
            sprintf (temp, "%d", conf->auth_method);
            chxofascii (interim, temp);
         }
      }

      else if (streq (what, "auth_pw")) {
         if (NOT protect) {
            if (chxtoken (word1, nullchix, 1, arg) >= 0)
               ascofchx (conf->password, word1, 0, 40);
            chxofascii (interim, conf->password);
         }
      }

      else if (streq (what, "auth_cookie")) {
         if (NOT protect) {
            pid = (int) getpid();
            if (chxtoken (word1, nullchix, 1, arg) >= 0) {
               cookie_path[0] = '\0';
               if (chxtoken (word2, nullchix, 2, arg) >= 0)
                  sprintf (cookie_path, "; path=%s", ascquick (word2));

               sprintf (conf->caucus_ver, "%09d", rand() % 99999997);
               sprintf (temp, "Set-Cookie: caucus_usr%d=%s%s\n"
                              "Set-Cookie: caucus_ver%d=%s%s\n",
                        pid, ascquick(word1),  cookie_path,
                        pid, conf->caucus_ver, cookie_path);
            }
            else sprintf (temp, "Set-Cookie: caucus_usr%d=%s\n"
                                "Set-Cookie: caucus_ver%d=x%s\n"
                                "Set-Cookie: CAUCUSID=; path=/\n",
                                 pid, cookie_path, pid, cookie_path);
            chxofascii (interim, temp);
         }
      }

      else if (streq (what, "asc2dec")) {
         for (pos=0;   val1 = chxvalue (arg, pos);   ++pos) {
            sprintf (temp, "%d ", (int) val1);
            chxcat  (interim, CQ(temp));
         }
      }

      else if (streq (what, "add_user_info")) {
         if (allow_functions && NOT protect) func_var (interim, what, arg);
      }

      else if (streq (what, "asynch")) {
         if (allow_functions && NOT protect)
            func_shell (interim, 0, 0, 0, arg, conf);
      }

      /*** $ad_...() functions. */
      else if (strindex (what, "ad_")  == 0    ||
               strindex (what, "add_") == 0 ) {
         if (allow_functions && NOT protect)
            func_ad (interim, what, arg, conf);
      }

      else if (streq (what, "array_eval")) {
         if (array_eval_depth > 20) {
            chxtoken  (word1, nullchix, 1, arg);
            strcpy    (temp, ascquick(word1));
            array_err (cfile, temp, "array_eval recursion limit");
         }
         else {
            ++array_eval_depth;
            newaref = chxalloc ( L(100), THIN, "func_value newaref");
            aetemp  = chxalloc ( L(100), THIN, "func_value aetemp");
            chxcpy (newaref, arg);
  
            /* get value of cell */
            chxclear (word1);
            if (func_ar (word1, "array", arg, conf, cmlargs, vars, form,
                         protect, incvars, macs, cfile, otab, tg, aref)) {
  
               /* call funcmac, funcscan on it to do full evaluation. */
               func_mac   (aetemp, word1,  conf, "", vars, form,
                                   incvars, macs, cfile, otab, tg, newaref);
               func_scan  (result, aetemp, conf, "", vars, form,
                                   incvars, macs, cfile, otab, tg, newaref);
            }
            chxfree (aetemp);
            chxfree (newaref);
            --array_eval_depth;
         }
      }

      else if (strindex (what, "array")  == 0)
         func_ar (interim, what, arg, conf, cmlargs, vars, form,
                  protect, incvars, macs, cfile, otab, tg, aref);

      else nofunc = 1;
      break;
  
   case 'b':
      if (streq (what, "between")) {
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         ok = parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         ok = parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg);
         ok = parse_next_num (&val3, &dbl3, &is_int, &precise, &pos, arg);
         sprintf (temp, "%d",
            (is_int ? (val1 <= val2  &&  val2 <= val3 ? 1 : 0) :
                      (dbl1 <= dbl2  &&  dbl2 <= dbl3 ? 1 : 0)));
         chxofascii (interim, temp);
      }

      else if (strindex (what, "bit_") == 0 )   func_bit (interim, what, arg);
 
      else if (streq (what, "browser_format")) {
         sprintf (temp, "%d", conf->browserform);
         chxofascii (interim, temp);
      }

      else if (streq (what, "bufpause")) {
         /*#if LNX12*/
         /*    chxnum (arg, &op1); */
         /*    sleep  ((int) op1); */
         /*#endif*/
      }

      else if (streq (what, "build_name_cascade")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         build_name_cascade (val1);
      }

      else nofunc = 1;
      break;

   case 'c':
      if (streq (what, "caucus_id"))  chxofascii (interim, conf->caucus_name);

      else if (streq (what, "charval")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         sprintf (temp, "%d", chxvalue (arg, pos+1+val1));
         chxofascii (interim, temp);
      }

      else if (streq (what, "charvals")) {
         for (pos=0;   val1 = chxvalue (arg, pos);   ++pos) {
            sprintf (temp, " %d", val1);
            chxcat  (interim, CQ(temp));
         }
      }

      else if (streq (what, "char")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         chxcatval (interim, THIN, L(val1));
      }
  
      else if (streq (what, "commaList"))   commaList (result, arg);

      /*** $CLEANHTML "Clean" HTML filter. */
      else if (streq (what, "cleanhtml"))
         func_chtml  (interim, arg, conf, cmlargs, vars,
                               form, incvars, macs, cfile, tg, aref);
  
      else if (streq (what, "close")) {
         if (chxnum (arg, &val1)) {
            if (val1 > 0) {
               fclose (handle[val1-1]);
               handle[val1-1] = NULL;
            }
         }
      }
  
      else if (streq (what, "cml_dir")) {
         strcpy (temp, conf->cmlname);
         for (slash = strlen(temp) - 1;   slash > 0;   --slash)
            if (temp[slash]=='/')   break;
         temp[slash] = '\0';
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "cml_page")) {
         pos = chxindex (arg, L(0), CQ("?"));
         if (pos < 0)  pos = chxindex (arg, L(0), CQ("#"));
         if (pos < 0)  pos = chxlen   (arg) - 1;
         pos2 = chxrevdex (arg, pos, CQ("/")) + 1;
         chxcatsub (interim, arg, pos2, pos - pos2);
      }

      else if (streq (what, "cache_timeout")) {
         if (allow_functions && NOT protect) {
            pos  = L(0);
            val1 = chxint4 (arg, &pos);
            if (val1 > 0)  conf->cache_timeout = val1;
         }
      }

      else if (streq (what, "cache_clear")) {
         conf->reload = 1;
      }

      else if (streq (what, "cml_path")) {
         if (allow_functions && NOT protect)
            chxcat (interim, CQ(conf->cml_path));
      }

      else if (streq (what, "caucus_path")) {
         if (allow_functions && NOT protect)
            chxcat (interim, CQ(conf->caucus_path));
      }

      else if (streq (what, "caucus_lib")) {
         if (allow_functions && NOT protect)
            chxcat (interim, CQ(conf->caucuslib));
      }

      else if (streq (what, "config")) {
         if (chxtoken (word1, nullchix, 1, arg) >= 0) {
            for (num=0;   conf->config[num] != NULL;   ++num) {
               chxtoken  (word2, nullchix, 1, conf->config[num]);
               if (chxeq (word2, word1)) {
                  chxtoken (nullchix, word2, 2, conf->config[num]);
                  chxcat   (interim,  word2);
                  break;
               }
            }
         }
      }

      else if (strindex (what, "cell") == 0)
          func_ar (interim, what, arg, conf, cmlargs, vars, form,
                   protect, incvars, macs, cfile, otab, tg, aref);

      else if (streq (what, "create_conf")) {
         if (allow_functions && NOT protect)  create_conf (result, arg, conf);
      }

      else if (streq (what, "copy2lib")) {
         if (allow_functions && NOT protect) {
            chxtoken (word1, nullchix, 1, arg);
            chxtoken (word2, nullchix, 2, arg);
            ascofchx (temp,  word1, L(0), L(200));
            ascofchx (temp2, word2, L(0), L(200));
            if ( (url = copy2lib (temp,  temp2, conf)) != NULL)
              chxofascii (interim, url);
         }
      }

      /*** $copyfile (source target perms)   perms is in octal!! */
      else if (streq (what, "copyfile")) {
         if (allow_functions && NOT protect) {
            chxtoken (word1, nullchix, 3, arg);
            ascofchx (temp,  word1, L(0), L(200));
            sscanf   (temp, "%o", &num);
            chxtoken (word1, nullchix, 1, arg);
            chxtoken (word2, nullchix, 2, arg);
            ascofchx (temp,  word1, L(0), L(200));
            ascofchx (temp2, word2, L(0), L(200));
            sprintf  (temp3, "%d", cml_copyfile (temp,  temp2, num));
            chxcat   (interim, CQ(temp3));
         }
      }

      else if (strindex (what, "conf_v") == 0)
         { if (allow_functions) func_var (interim, what, arg); }

      /*** $cl_...() functions. */
      else if (strindex (what, "cl_") == 0)
         { if (allow_functions) func_cl (interim, what, arg, conf); }
  
      /*** $co_...() functions. */
      else if (strindex (what, "co_") == 0 ) {
         if (allow_functions) func_co (interim, what, arg, conf, protect);
      }

      else if ( strindex (what, "chg_") == 0) {
         if (allow_functions && NOT protect)
            func_ad (interim, what, arg, conf);
      }

      /*** $clear_site_var(), $clear_conf_var(), etc. */
      else if (strindex (what, "clear_")    == 0) {
         if (allow_functions && NOT protect) func_var (interim, what, arg);
      }

      else nofunc = 1;
      break;

   case 'd':
      if (streq (what, "dateof")) {
         pos  = 0;
         val1 = chxint4 (arg, &pos);
         sysdaytime (interim, word2, 0, val1);
         chxcat     (interim, CQ(" "));
         chxcat     (interim, word2);
      }
  
      else if (streq (what, "debug")) {
         if (allow_functions && NOT protect) {
            pos  = L(0);
            val1 = chxint4 (arg, &pos);
            cml_debug = val1;
            sprintf    (temp, "%d", val1);
            chxofascii (interim, temp);
      /*    if (val1  &&  sublog == NULL) {
               systmpdir (tmpdir);
               sprintf   (temp, "%s/sub%07d.log", tmpdir, getpid());
               sublog = fopen (temp, "w");
      /     } */
         }
      }
  
      else if (streq (what, "dec2hex")) {
         pos  = 0;
         val1 = chxint4 (arg, &pos);
         sprintf  (temp, "%x", val1);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "disk_failure")) {
         if (disk_failure (0, &type, temp2)) {
            sprintf (temp, "%d %s", type, temp2);
            chxcat  (interim, CQ(temp));
         }
      }
  
      else if (streq (what, "divide")) {
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         ok = parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         ok = parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg);
         if (ok  &&  val2 != 0) {
            sprintf (temp, "%d", val1 / val2);
            chxofascii (interim, temp);
         }
      }
  
      else if (streq (what, "dosfile")) {
         chxclear  (word1);
         chxcatsub (word1, arg, L(0), L(8));
         t1 = chxquick (".", 0);
         t2 = chxquick ("_", 1);
         while  (chxalter (word1, L(0), t1, t2)) ;
         chxcat (interim, word1);
      }

      else if (streq (what, "delfile")) {
         if (allow_functions && NOT protect) {
            ascofchx  (temp, arg, L(0), TEMPMAX);
            sysunlink (temp);
         }
      }
  
      else if (streq (what, "deldir")) {
         if (allow_functions && NOT protect) {
            ascofchx  (temp, arg, L(0), TEMPMAX);
            sysrmdir  (temp);
         }
      }
  
      else if (streq (what, "dirlist"))
         { if (allow_functions && NOT protect)  dirlist (interim, arg); }
  
      else nofunc = 1;
      break;

   case 'e':
   case 'E':
      if (streq (what, "err")  ||  streq (what, "ERR")) {
         chxcatval (interim, THIN, L(0177));
         chxcat    (interim, CQ("ERR"));
      }

      else if (streq (what, "empty")) {
         chxtoken (word1, nullchix, 1, arg);
         chxcatval(interim, THIN, L( chxvalue (word1, L(0)) == 0 ? '1' : '0') );
      }
  
      else if (streq (what, "epoch")) {
         val1 = epoch_of (arg);
         sprintf (temp, "%d", val1);
         chxofascii (interim, temp);
      }

      else if (streq (what, "epochOfYYYY")) {
         val1 = epoch_of_yyyy (arg);
         sprintf (temp, "%d", val1);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "equal")) {
         chxtoken  (word1, nullchix, 1, arg);
         chxtoken  (word2, nullchix, 2, arg);
         chxcatval (interim, THIN, (chxeqf (word1, word2) ? '1' : '0'));
      }

      else if (streq (what, "eval"))  ;

      else if (streq (what, "encode64")) {
         if ( (fp = fopen (ascquick (arg), "r")) != NULL) {
            encode64 (fp, interim);
            fclose   (fp);
         }
      }

      /*** $exists(file)   Does file exist? */
      else if (streq (what, "exists")) {
         val1 = '0';
         if (chxtoken (word1, nullchix, 1, arg) >= 0) {
            ascofchx  (temp, word1, L(0), L(TEMPMAX));
            if (syscheck (temp))  val1 = '1';
         }
         chxcatval (interim, THIN, val1);
      }
  
      /*** $ESCQUOTE()   Escape double-quote chars into &quot; */
      else if (streq (what, "escquote"))  func_escquote (interim, arg);
  
      /*** $ESCSINGLE()   Escape single-quote chars into "\'". */
      else if (streq (what, "escsingle"))  func_escsingle (interim, arg);

      /*** $encode (cval str) */
      else if (streq (what, "encode"))  {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         chxtoken (nullchix, word1, 2, arg);
         cencode (interim, val1, word1);
      }

      else if (streq (what, "email_start")) {
         chxcat (interim, CQ(lowerBoundary()));
      }

      else if (streq (what, "email_stop")) {
         chxcat (interim, CQ(upperBoundary()));
      }

      else if (streq (what, "email_extract")) {
         chxtoken (word1, nullchix, 1, arg);
         chxtoken (nullchix, word2, 2, arg);
         extractNewTextFromHtmlEmailReply (interim, word2, word1);
      }
  
      else if (streq (what, "errors"))  {
         chxcat   (interim, cfile->errtext);
         chxclear (cfile->errtext);
      }

      else nofunc = 1;
      break;

   case 'f':
      /*** HTML FORMs data. */
      if (streq (what, "form")) {
         for (num=0;   num<form->used;   ++num) {
            if (chxeqf (form->name[num], arg)) {
               chxcpy  (interim, form->value[num]);
               break;
            }
         }
      }

      else if (streq (what, "file")) {
         if (allow_functions && NOT protect) {
            sprintf (temp, "%s/%s", conf->cml_path, ascquick(arg));
            if ( (fp = fopen (temp, "r")) != NULL) {
               for (first=1;   readcml (fp, conf, word1, 0, &lnum);   first=0) {
                  if (NOT first) chxcatval (interim, THIN, L('\n'));
                  func_scan (word2,   word1, conf, cmlargs, vars, form,
                                      incvars, macs, cfile, otab, tg, aref);
                  chxcat    (interim, word2);
               }
               fclose (fp);
            }
         }
      }

      else if (streq (what, "find_it"))
         { if (allow_functions)   search_it     (interim, arg, 1); }

      else if (streq (what, "find_in_item"))
         { if (allow_functions)   find_in       (interim, arg, conf); }

      else if (streq (what, "find_filter"))
         { if (allow_functions)   search_filter (interim, arg, 1); }

      else if (streq (what, "file_data")) {
         if (allow_functions && NOT protect)
            chxcatval (interim, THIN, (file_data (arg) ? L('1') : L('0')));
      }

      else nofunc = 1;
      break;

   case 'g':
      if (streq (what, "greater")) {
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         ok = parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         ok = parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg);
         sprintf    (temp, "%d", (is_int ? (val1 > val2 ? 1 : 0) :
                                           (dbl1 > dbl2 ? 1 : 0)));
         chxofascii (interim, temp);
      }

      else if (streq (what, "group_update"))  {
         while (NOT syslocker (1, 1, 9)) syssleep (3);
         ok = group_update(arg);
         syslocker (0, 1, 9);
         chxcatval (interim, THIN, (ok ? L('1') : L('0')));
      }

      else if (streq (what, "group_update_rule"))  {
         chxtoken (word1, nullchix, 1, arg);
         ok = chxtoken (word2, nullchix, 2, arg) > 0  &&
              chxtoken (word3, nullchix, 3, arg) < 0;
         if (ok) {
            while (NOT syslocker (1, 1, 9)) syssleep (3);
            ok = group_update_rule (word1, word2);
            syslocker (0, 1, 9);
         }
         chxcatval (interim, THIN, (ok ? L('1') : L('0')));
      }

      else if (streq (what, "group_access"))  {
         chxtoken (word1, nullchix, 1, arg);
         chxtoken (word2, nullchix, 2, arg);
         chxtoken (word3, nullchix, 3, arg);
         val1 = group_access (word1, word2, word3);
         sprintf    (temp, "%d", val1);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "gt_equal")) {
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         ok = parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         ok = parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg);
         sprintf    (temp, "%d", (is_int ? (val1 >= val2 ? 1 : 0) :
                                           (dbl1 >= dbl2 ? 1 : 0)));
         chxofascii (interim, temp);
      }

      else if (streq (what, "gen_sort"))     gen_sort  (result, arg);

      else if (streq (what, "goodbye")) {
         if (allow_functions && NOT protect) conf->timeout = 60;
      }

      else if (streq (what, "groupperm")) {
         val1 = 0;
         for (which=1;  chxtoken (word1, nullchix, which, arg) >= 0;  ++which) {
            jixreduce (word1);
            if ( (found = tablefind (conf->grouppermnames, word1)) >= 0) {
               chxtoken (word2, nullchix, found+2, conf->grouppermnames);
               if (chxnum (word2, &val2))    val1 = val1 | val2;
            }
         }
         sprintf (temp, "%ld", val1);
         chxofascii (interim, temp);
      }

      else nofunc = 1;
      break;

   case 'h':
      if (streq (what, "hex2dec")) {
         ascofchx (temp, arg, L(0), L(20));
         sscanf   (temp, "%x", &val1);
         sprintf  (temp, "%d", val1);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "host"))    chxofascii (interim, conf->hostname);
  
      /*** $HTTP_USER_AGENT()   CGI environment variable. */
      else if (streq (what,  "http_user_agent"))
         chxofascii (interim, http.user_agent);

      /*** $HTTP_REMOTE_USER()   CGI env var REMOTE_USER or AUTH_USER. */
      else if (streq (what,  "http_remote_user"))
         chxofascii (interim, conf->remote);
  
      /*** $HTTP_REFERER()   CGI env var HTTP_REFERER. */
      else if (streq (what,  "http_referer")) {
         pos = L(0);
         op1 = chxint4 (arg, &pos);
         if (pos == 0)  op1 = 1;
         chxcat (interim, CQ ((op1 ? http.referer1 : http.referer0)));
      }

      /*** $HTTP_ACCEPT()   CGI env var HTTP_ACCEPT. */
      else if (streq (what, "http_accept"))
         chxcat (interim, CQ (http.xaccept));

      else if (streq (what, "http_lib")) {
         if (allow_functions && NOT protect)
            chxcat (interim, CQ(conf->httplib));
      }
  
      /*** $H2URL   HTML-to-linked-URL filter. */
      else if (streq (what, "h2url")) {
         func_h2url (arg, conf, cmlargs, vars, form, incvars, macs,
                     cfile, tg, aref);
         chxcpy (interim, arg);
      }

      else nofunc = 1;
      break;

   case 'i':
      if (streq (what, "if")) {
         pos = L(0);
         op1 = chxint4 (arg, &pos);
         if (op1) chxtoken (interim, nullchix, 2, arg);
         else     chxtoken (nullchix, interim, 3, arg);
      }
  
      else if (streq (what, "ifthen")) {
         pos = L(0);
         op1 = chxint4 (arg, &pos);
         if (op1) chxtoken (nullchix, interim, 2, arg);
      }
  
      else if (streq (what, "inc")) {
         chxnum      (arg, &which);
         chxtoken    (interim, nullchix, which, incvars);
         chxrepval   (interim, L(0), THIN, L(1), L(' '));
      }
  
      else if (streq (what, "is_num")) {
         pos2 = pos = L(0);
         chxint4 (arg, &pos);
         sprintf (temp, "%d", pos);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "is_passwd"))
         chxcatval (interim, THIN, L(NULLSTR(conf->pw_prog) ? '0' : '1'));
  
      else if (streq (what, "item_parse"))    item_parse (result, arg);

      else if (streq (what, "item_sort"))     item_sort  (result, arg);

      else if (streq (what, "item_add"))      item_add   (result, arg, conf);

      else if (streq (what, "item_id"))       item_id    (result, arg);

      else if (streq (what, "item_label"))    item_label (result, arg);

      else if (streq (what, "item_del")) {
         if (NOT protect)                     item_del   (result, arg);
      }

      else if (streq (what, "item_tuplet"))   item_tuplet(result, arg);

      else if (streq (what, "ip_addr"))       chxofascii (interim, conf->ip_addr);

      else if (streq (what, "item_set")) {
         if (NOT protect)                     item_set   (result, arg);
      }

      else if (streq (what, "item_set_seen")) item_set_seen (result, arg);

      /*** $it_...() functions. */
      else if (strindex (what, "it_") == 0)
         { if (allow_functions)  func_it (interim, what, arg, conf, protect); }

      /*** $item_var, etc.. */
      else if (strindex (what, "item_") == 0)
         { if (allow_functions)  func_var (interim, what, arg); }

      else if (streq (what, "includecml")) {
         chxtoken (word1, nullchix, 1, arg);
         chxtoken (nullchix, word2, 2, arg);
         ctg = target (0, result);

         lnum = include_cml (ctg, word1, vars, form, conf, "", &broke, word2,
                             macs, cfile, otab, &contd, aref);
      }

      else if (streq (what, "in_group")) {
         chxtoken (word1, nullchix, 1, arg);
         chxtoken (word2, nullchix, 2, arg);

         ok = 0;
         if (chxvalue (word2, L(0)) > 0)
            ok = groupmatch (word2, (Namelist) NULL, word1, wildcard,
                             conf->grouppermnames, conf->groupsearch);
         sprintf (temp, "%d", ok);
         chxofascii (interim, temp);
      }

      else nofunc = 1;
      break;

   case 'j':
      if (streq (what, "jshell")) {
         if (allow_functions && NOT protect) {
            pos  = L(0);
            val1 = chxint4 (arg, &pos);
            chxtoken   (nullchix, word1, 2, arg);
            func_shell (interim, (int) val1, 1, 0, word1, conf);
         }
      }

      else nofunc = 1;
      break;

   case 'l':
      if (streq (what, "less")) {
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         ok = parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         ok = parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg);
         sprintf    (temp, "%d", (is_int ? (val1 < val2 ? 1 : 0) :
                                           (dbl1 < dbl2 ? 1 : 0)));
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "lower")) {
         chxcpy   (interim, arg);
         jixreduce (interim);
      }

      /*** Lines (n count str)  starting at line N, get COUNT lines from STR */
      else if (streq (what, "lines")) {
         pos = L(0);
         op1 = chxint4 (arg, &pos);
         op2 = chxint4 (arg, &pos);
         if ( (pos = chxtoken (word1, nullchix, 3, arg)) > 0) {
            for (num=1;   num<op1 && chxnextline(word1, arg, &pos);   ++num) ;
            if (op2 < 0)  op2 = 1000000;
            for (num=0;   num<op2 && (val1 = chxnextline(word1, arg, &pos));
                 num++) {
               chxcat (interim, word1);
               if (val1 == 2)  chxcatval (interim, THIN, L('\n'));
            }
         }
      }

      /*** $lock (locknum max_time_to_wait) */
      else if (streq (what, "lock")) {
         success = 0;
         pos  = 0;
         op1  = chxint4 (arg, &pos);
         pos2 = pos;
         op2  = chxint4 (arg, &pos);
         if (op1 >= 0  &&  op1 < LOCKMAX  &&  pos > pos2  && NOT protect) {
            if (op2 <= 0)  op2 = 60;
            for (tstart = time(NULL);   time(NULL) - tstart < op2; ) {
               if (syslocker (1, 1, op1))   { success = 1;   break; }
               syssleep (2);
            }
         }
         chxcatval (interim, THIN, (success ? L('1') : L('0')));
      }

      else if (strindex (what, "lice_") == 0)
         func_lice (result, what, arg, conf);

      else if (streq (what, "logevent")) {
         func_logevent (arg);
      }

      /*** $list_site_var(), $clear_conf_var(), etc. */
      else if (strindex (what, "list_")    == 0) {
         if (allow_functions && NOT protect) func_var (interim, what, arg);
      }

      else nofunc = 1;
      break;

   case 'm':
      if (streq (what, "mac_expand"))
         func_mac (interim, arg, conf, cmlargs, vars, form,
                            incvars, macs, cfile, otab, tg, aref);
  
      else if (streq (what, "max")) {
         val1    = 0;      dbl1 = 0.;
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         while (parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg)) {
            if (val2 > val1)  val1 = val2;
            if (dbl2 > dbl1)  dbl1 = dbl2;
         }
  
         chxofascii (interim, format_number (val1, dbl1, is_int, precise));
      }
  
      else if (streq (what, "min")) {
         val1    = 0;      dbl1 = 0.;
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         while (parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg)) {
            if (val2 < val1)  val1 = val2;
            if (dbl2 < dbl1)  dbl1 = dbl2;
         }
  
         chxofascii (interim, format_number (val1, dbl1, is_int, precise));
      }
  
      else if (streq (what, "minus")) {
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         ok = parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         ok = parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg);
         val1 = val1 - val2;
         dbl1 = dbl1 - dbl2;
         chxofascii (interim, format_number (val1, dbl1, is_int, precise));
      }
  
      else if (streq (what, "md5hash")) {
         chxclear (word1);
         chxtoken (word1, nullchix, 1, arg);
         sysmd5   (temp, ascquick(word1));
         chxcpy   (interim, CQ(temp));
      }
  
      else if (streq (what, "mult")) {
         val1    = 1;      dbl1 = 1.;
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         digits  = 0;
         while (parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg)) {
            digits += precise;
            precise = 0;
            val1 = val1 * val2;
            dbl1 = dbl1 * dbl2;
         }
  
         chxofascii (interim, format_number (val1, dbl1, is_int, digits));
      }

      /*** $my_...() and set_my_...() functions. */
      else if (strindex (what, "my_")  == 0)
         { if (allow_functions)  func_my (interim, what, arg); }

      /*** $mgr_...() functions. */
      else if (strindex (what, "mgr_") == 0 )
         { if (allow_functions)  func_mgr (interim, what, arg, protect); }

      else if (streq (what, "mac_define"))
         { if (allow_functions && NOT protect) mac_define (macs, arg); }

      else if (streq (what, "mkdir")) {
         ok = 0;
         if (allow_functions && NOT protect) {
            chxcat   (arg, CQ(" 755"));
            chxtoken (word1, nullchix, 1, arg);
            chxtoken (word2, nullchix, 2, arg);
            chxnum   (word2, &val0);
            permission = (val0/100) * 64 + ((val0/10) % 10) * 8 + (val0 % 10);
            ok = sysmkdir (ascquick(word1), permission, NULL);
         }
         chxcatval (interim, THIN, (ok ? L('1') : L('0')));
      }

      else nofunc = 1;
      break;

   case 'n':
      if (streq (what, "newline"))  chxofascii (interim, "\n");

      else if (streq (what, "not")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         sprintf    (temp, "%d", (pos==0L ? 1 : (val1 ? 0 : 1)));
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "not_equal")) {
         chxtoken  (word1, nullchix, 1, arg);
         chxtoken  (word2, nullchix, 2, arg);
         chxcatval (interim, THIN, (chxeqf (word1, word2) ? '0' : '1'));
      }
  
      else if (streq (what, "not_empty")) {
         chxtoken  (word1, nullchix, 1, arg);
         chxcatval (interim, THIN,
                        L( chxvalue (word1, L(0)) == 0 ? '0' : '1') );
      }
  
      else if (streq (what, "numeric")) {
         chxtoken (word1, nullchix, 1, arg);
         pos = 0;
         chxint4 (arg, &pos);
         chxcatval (interim, THIN,
            (chxvalue (word1, pos) == 0 ? L('1') : L('0')) );
      }

      else if (streq (what, "num_sort"))      num_sort  (result, arg);

      else if (streq (what, "new_win")) {
         if (allow_functions) {
            pos  = 0;      val1 = chxint4 (arg, &pos);
            pos2 = pos;    val2 = chxint4 (arg, &pos);
            if (pos > pos2  &&  NOT protect) {
               conf->new_win_x = max (min (val1, 1024), 100);
               conf->new_win_y = max (min (val2, 1024), 100);
            }
            sprintf (temp, "%d %d", conf->new_win_x, conf->new_win_y);
            chxcat  (result, CQ(temp));
         }
      }

      else nofunc = 1;
      break;

   case 'o':
      if (streq (what, "open")) {
         success = 0;
         if (allow_functions && NOT protect) {
            for (num=0;   num<HANDLES;   ++num)
               if (handle[num] == NULL)  break;
            chxtoken  (word1, nullchix, 1, arg);
            chxtoken  (word2, nullchix, 2, arg);
            jixreduce (word2);
            ascofchx  (what, word2, L(0), L(2));
  
            success = (num < HANDLES  &&
                      (what[0]=='r'  ||  what[0]=='a'  ||  what[0]=='w'));
            if (success) {
               if      (what[0]=='w')  nt_mkfile  (ascquick(word1), 0700);
               else if (what[0]=='a')  nt_chkfile (ascquick(word1), 0700);
               handle[num] = fopen (ascquick(word1), what);
               success = (handle[num] != NULL);
            }
         }
  
         if (success)  sprintf (temp, "%d", num+1);
         else          strcpy  (temp, "0");
         chxofascii   (interim, temp);
      }
  
      else if (streq (what, "or")) {
         val1 = val2 = 0;
         pos  = pos2 = L(0);
         do {
            pos  = pos2;
            val1 = val1  ||  val2;
            val2 = chxint4 (arg, &pos2);
         } while (pos2 != pos);
  
         sprintf    (temp, "%d", val1);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "opsys")) {
         sysopsys (temp);
         chxcat   (result, CQ(temp));
      }
  
      else if (strindex (what, "ob_") == 0)
         func_ob (result, what, arg, conf, cfile, otab);

      else if (streq (what, "output")) {
         fd = 0;
         if (allow_functions && NOT protect) {
            if (conf->output > 2)  {
               sysfdclose (conf->output);
               conf->output = output_default;
            }
  
            /*** Pluck out the file name, and create it (but note api a_init()'s
            /    effect on umask...) */
            if (chxtoken (word1, nullchix, 1, arg) >= 0) {
  
               /*** Decode file permission from 2nd arg. */
               chxtoken (word2, nullchix, 2, arg);
               strcpy (temp2, ascquick(word2));
               permission = 0700;
               if (NOT NULLSTR(temp2))  sscanf (temp2, "%o", &permission);
  
               /*** Pluck out file name, and open the file! */
               strcpy (temp, ascquick (word1));
               sysprv (0);
               if ( (fd = sysfdopen (temp, 1, permission)) > 0)
                  conf->output = fd;
               sysprv (1);
            }
            else fd = -1;
         }
  
         sprintf    (temp, "%d", (fd > 0 ? 1 : 0));
         chxofascii (interim, temp);
      }

      else nofunc = 1;
      break;

   case 'p':
      if (streq (what, "pad")) {
         pos   = L(0);
         width = chxint4 (arg, &pos);
      }
  
      else if (streq (what, "page_flush"))  buf_flush (tg->cd);

      /*** $PAGE_xxx() functions. */
      else if (strindex (what, "page_")  == 0)  func_page (interim, what, arg);

      else if (strindex (what, "pw_") == 0)
         func_pw   (result, what, arg, NOT protect, conf);

      else if (streq (what, "passcheck")) {
         if (allow_functions && NOT protect) {
            success = 7;
            if (conf->pw_can_verify)
               success = func_pw (word2, "pw_verify", arg, 0, conf);
            allow_functions = (success == 0);
            sprintf (temp, "%d", (success == 0 ? 1 : 0));
            chxcat  (result, CQ(temp));
         }
      }
  
      else if (streq (what, "pid")) {
         sprintf    (temp, "%07d/%07d", (int) getpid(), conf->secure);
         chxofascii (interim, temp);
      }

      else if (streq (what, "priv"))     func_priv (interim, arg);

      else if (streq (what, "privName")) func_privName (interim, arg);
  
      else if (streq (what, "plus")) {
         val1    = 0;      dbl1 = 0.;
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         while (parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg)) {
            val1 = val1 + val2;
            dbl1 = dbl1 + dbl2;
         }
  
         chxofascii (interim, format_number (val1, dbl1, is_int, precise));
      }
  
      else if (streq (what, "plusmod")) {
         pos  = L(0);
         ok = parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         ok = parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg);
         ok = parse_next_num (&val3, &dbl3, &is_int, &precise, &pos, arg);
         if (ok) {
            sprintf    (temp, "%d", (pos==0L ? 0 : (val1 + val2) % val3) );
            chxofascii (interim, temp);
         }
      }
  
      else if (streq (what, "protect"))   chxcat     (interim, arg);

      else if (streq (what, "pagetime")) {
         sprintf (temp, "%d", conf->pagetime);
         chxofascii (interim, temp);
      }


      /*** $per_...() functions. */
      else if (strindex (what, "per_") == 0)
         { if (allow_functions)  func_per (interim, what, arg, protect); }
  
      /*** $peo_...() functions. */
      else if (strindex (what, "peo_") == 0)
         { if (allow_functions)  func_peo (interim, what, arg); }

      else if (streq (what, "passwd")) {
         if (allow_functions && NOT protect) {
            success = 7;
            if (conf->pw_can_change) {
               success = 0;
               if (conf->pw_can_verify) {
                  chxtoken  (word1, nullchix, 1, arg);
                  chxtoken  (word2, nullchix, 3, arg);
                  chxcatval (word1, THIN, L(' '));
                  chxcat    (word1, word2);
                  success = func_pw (word2, "pw_verify", word1, 0, conf);
               }
               if (success == 0) {
                  chxtoken  (word1, nullchix, 1, arg);
                  chxtoken  (word2, nullchix, 2, arg);
                  chxcatval (word1, THIN, L(' '));
                  chxcat    (word1, word2);
                  success = func_pw (word2, "pw_change", word1, 0, conf);
               }
            }
            sprintf (temp, "%d", success);
            chxcat  (result, CQ(temp));
         }
      }

      else nofunc = 1;
      break;

   case 'q':
      if (streq (what, "quote"))   quoter (interim, what, arg);

      else if (streq (what, "quotient")) {
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         ok = parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         ok = parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg);
         pg = parse_next_num (&val3, &dbl3, &is_int, &precise, &pos, arg);
         if (ok  &&  ((is_int && val2!=0) || (NOT is_int && dbl2!=0.))) {
            if (val2 != 0 )  val1 = val1 / val2;
            dbl1   = dbl1 / dbl2;
            is_int = is_int  &&  ((float) val1 == dbl1);
            chxofascii (interim, format_number (val1, dbl1, is_int,
                                 (pg ? val3 : 10)));
         }
      }

      else nofunc = 1;
      break;

   case 'r':
      if (streq (what, "rest")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         chxtoken (nullchix, interim, val1+1, arg);
      }

      else if (streq (what, "resp_add"))      resp_add  (result, arg);

      else if (streq (what, "resp_set"))      resp_set  (result, arg);

      else if (streq (what, "resp_del"))      resp_del  (result, arg);

      else if (streq (what, "reloaded")) {
         chxcatval (interim, THIN, L(060 + conf->reload));
      }

      else if (streq (what, "random")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         sprintf (temp, "%d", rand() % (val1 > 0 ? val1 : 1));
         chxofascii (interim, temp);
      }

      else if (streq (what, "round")) {
         pos     = L(0);
         is_int  = 1;
         precise = 0;
         ok = parse_next_num (&val1, &dbl1, &is_int, &precise, &pos, arg);
         pg = parse_next_num (&val2, &dbl2, &is_int, &precise, &pos, arg);
         if (ok)
            chxofascii (interim, format_number (val1, dbl1, is_int, val2));
      }
  
      else if (streq (what, "readfile")) {
         if (allow_functions && NOT protect) {
            lang = conf->browserform+1;
            ascofchx (temp, arg, L(0), L(TEMPMAX));
            if ( (fp = fopen (temp, "r")) != NULL) {
               for (first=1;   fgets (temp, TEMPMAX, fp) != NULL;   first=0) {

                  /*** Convert Windows CR/LF to LF. */
                  last = strlen (temp) - 1;
                  if (last > 1  &&  temp[last]=='\n'  &&  temp[last-1]=='\r') {
                     temp[last-1] = '\n';
                     temp[last  ] = '\0';
                  }
                  if (temp[last]=='\r')  temp[last] = '\n';

                  to_jix (interim, temp, 1-first, lang);
               }
               fclose (fp);
            }
         }
      }

      else if (streq (what, "readterm")) {
         fgets (temp, TEMPMAX, stdin);
         chxofascii (interim, temp);
         chxchomp   (interim);
      }
  
      else if (streq (what, "readln")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos) - 1;
         chxtoken (word1, nullchix, 2, arg);
         chxclear (word2);
         success = '0';
         if (val1>=0   &&  val1<HANDLES  &&  handle[val1]!=NULL  &&
                           chxvalue (word1, L(0)) != 0) {
            chxclear (word2);
            while (fgets (temp, TEMPMAX, handle[val1]) != NULL) {
               success = '1';
               to_jix   (word2, temp, 0, conf->diskform+1);
               last = strlen(temp) - 1;
               if (temp[last] == '\n')  break;
            }
            chxchomp (word2);
         }
         vartab_add (vars, word1, word2);
  
         chxcatval (interim, THIN, L(success));
      }
  
      else if (streq (what, "rename")) {
         success = 0;
         if (allow_functions && NOT protect) {
            chxtoken  (word1, nullchix, 1, arg);
            chxtoken  (word2, nullchix, 2, arg);
            ascofchx  (temp,  word1, L(0), L(200));
            ascofchx  (temp2, word2, L(0), L(200));
            success = sysrename (temp,  temp2);
         }
         chxcatval (interim, THIN, L(success ? '1' : '0'));
      }
  
      else if (streq (what, "replace")) {
         for (num=0;   num<2;   ++num) {
            chxtoken   (word1, nullchix, num+1, arg);
            if (chxlen (word1) == 1)   ccode[num] = chxvalue (word1, 0);
            else                       chxnum (word1, &ccode[num]);
         }
         chxtoken (nullchix, interim, 3, arg);
         type = chxtype (arg);
         for (pos=0;   (val1 = chxvalue (interim, pos));   ++pos) {
            if (val1 == ccode[0])   chxsetval (interim, pos, type, ccode[1]);
         }
      }

      else if (streq (what, "reval"))     chxcat     (interim, arg);

      else if (streq (what, "request_uri")) {
         strcpy (temp, (http.request_uri[0] == '\'' ? http.request_uri+1 :
                                                      http.request_uri));
         len = strlen (temp);
         if (temp[len-1] == '\'')  temp[len-1] = '\0';
         chxofascii (interim, temp);
      }
  
      /*** $RHTML    "Reduced" HTML filter. */
      else if (streq (what, "rhtml"))
         func_rhtml (interim, arg, 0, conf, cmlargs, vars,
                              form, incvars, macs, cfile, tg, aref);

      /*** $re_...() functions. */
      else if (strindex (what, "re_") == 0)
         { if (allow_functions)  func_re (interim, what, arg, conf->reload); }

      else nofunc = 1;
      break;

   case 's':
      /*** $SAFEHTML "Safe" HTML filter. */
      if (streq (what, "safehtml"))
         func_rhtml (interim, arg, 1, conf, cmlargs,
                              vars, form, incvars, macs, cfile, tg, aref);
  
      else if (strindex (what, "str") == 0)  func_str(interim, what, arg, conf);

      else if (streq (what, "set_browser_format")) {
         if (allow_functions && NOT protect) {
            pos = 0;
            val1 = chxint4 (arg, &pos);
            conf->browserform = val1;
         }
      }
  
      else if (streq (what, "sizeof")) {
         for (which=pos=0;   chxnextword (word1, arg, &pos) >= 0;   ++which) ;
         sprintf    (temp, "%d", which);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "set")) {
         chxtoken (word1, nullchix, 1, arg);
         chxtoken (nullchix, word2, 2, arg);

         /*** Object reference. */
         if ( (val1 = chxvalue (word1, L(0))) == 0177 || val1=='.')
              ob_set (word1, word2, otab, cfile);

         /*** Normal variable "set". */
         else vartab_add (vars, word1, word2);
      }

      else if (streq (what, "search_filter"))
         { if (allow_functions)  search_filter (interim, arg, 0); }

      else if (streq (what, "search_it"))
         { if (allow_functions)  search_it     (interim, arg, 0); }

      /*** $site_var(), etc. */
      else if (strindex (what, "site_") == 0)
         { if (allow_functions)  func_var (interim, what, arg); }

      else if (streq (what, "set_wrap")) {
         if (allow_functions && NOT protect) {
            pos    = L(0);
            g_wrap = chxint4 (arg, &pos);
         }
      }
  
      else if (strindex (what, "set_co_") == 0) {
         if (allow_functions && NOT protect)
            func_co (interim, what, arg, conf, protect);
      }
  
      else if (strindex (what, "set_it_") == 0) {
         if (allow_functions && NOT protect)
            func_it (interim, what, arg, conf);
      }
  
      else if (strindex (what, "set_my") == 0)
         { if (allow_functions && NOT protect) func_my (interim, what, arg); }
  
      else if (strindex (what, "set_per") == 0) {
         if (allow_functions && NOT protect) func_setper (interim, what, arg);
      }
  
      /*** $set_user_var(), $set_conf_var(), etc. */
      else if (strindex (what, "set_user_") == 0    ||
               strindex (what, "set_conf_") == 0    ||
               strindex (what, "set_site_") == 0    ||
               strindex (what, "set_item_") == 0)
         { if (allow_functions && NOT protect) func_var (interim, what, arg); }

      else if (streq (what, "shell")) {
         if (allow_functions && NOT protect)
            func_shell (interim, 1, 1, 0, arg, conf);
      }
  
      else if (streq (what, "silent")) {
         if (allow_functions && NOT protect)
            func_shell (interim, 0, 1, 0, arg, conf);
      }
  
      else if (streq (what, "sleep")) {
         if (NOT protect) {
            pos  = L(0);
            val1 = chxint4 (arg, &pos);
            sleep (val1);
         }
      }

      else if (strindex (what, "sql_") == 0) {
         if (allow_functions && NOT protect)
            func_sql (interim, what, arg, vars);
      }
  
      else nofunc = 1;
      break;

   case 't':
      /*** $T2ESC()   Pre-escape "&", "<", and ">" characters. */
      if (streq (what, "t2esc"))   func_t2esc  (interim, arg);

      /*** $T2HBR()  Text filter. */
      else if (streq (what, "t2hbr"))     func_t2hbr  (interim, arg);
  
      /*** $T2HTML  Text-to-HTML filter. */
      else if (streq (what, "t2html"))
         func_t2html (interim, arg, conf, cmlargs,
                               vars, form, incvars, macs, cfile, tg, aref);
  
      /*** $T2MAIL  Text-to-mailto: filter. */
      else if (streq (what, "t2mail"))  {
         func_t2mail (arg);
         chxcpy (interim, arg);
      }
  
      /*** $T2URL   Text-to-linked-URL filter. */
      else if (streq (what, "t2url")) {
         func_t2url (arg, conf, cmlargs, vars, form, incvars, macs,
                     cfile, tg, aref);
         chxcpy (interim, arg);
      }
  
      else if (streq (what, "tablefind")) {
         chxtoken  (word1,    nullchix, 1, arg);
         chxtoken  (nullchix, word2,    2, arg);
         jixreduce (word1);
         jixreduce (word2);
         val1 = tablefind (word2, word1);
         sprintf    (temp, "%d", val1+1);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "tablecompare")) {
         chxtoken  (word1,    nullchix, 1, arg);
         jixreduce (word1);
         for (pos=1;   chxtoken (word2, nullchix, pos+1, arg) >= 0;  ++pos) {
            jixreduce (word2);
            num = chxcompare (word1, word2, ALLCHARS);
            if (num <  0) { which = pos - 1;   break; }
            if (num == 0) { which = pos;       break; }
            which = pos;
         }
         sprintf    (temp, "%d", which);
         chxofascii (interim, temp);
      }

      else if (streq (what, "tablematch")) {
         chxtoken  (word1,    nullchix, 1, arg);
         chxtoken  (nullchix, word2,    2, arg);
         val1 = tablefind (word2, word1);
         sprintf    (temp, "%d", val1+1);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "time")) {
         sprintf (temp, "%ld", (long) time(NULL));
         chxofascii (interim, temp);
      }

      else if (streq (what, "timems")) {
         sprintf (temp, "%ld", systimems());
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "timezone")) {
         sprintf (temp, "%d", systimezone());
         chxofascii (interim, temp);
      }

      else if (streq (what, "timeout")) {
         if (NOT protect) {
            num  = conf->timeout;
            pos  = L(0);
            val1 = chxint4 (arg, &pos);
            if (pos > 0  &&  val1 > 0)  conf->timeout = val1;
            sprintf (temp, "%d", num);
            chxofascii (interim, temp);
         }
      }
  
      else if (streq (what, "triplet_sort"))  trip_sort (result, arg);

      else nofunc = 1;
      break;

   case 'u':
      if (streq (what, "unique")) {
         sprintf    (temp, "%d", unique());
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "unquote"))   quoter (interim, what, arg);
  
      else if (streq (what, "upper")) {
         chxcpy   (interim, arg);
         chxupper (interim);
      }
  
      else if (streq (what, "upper1")) {
         type = chxtype (arg);
         chxcpy (interim, arg);
         for (blank=1, pos=0L;   (val1 = chxvalue (interim, pos));   ++pos) {
            if (blank  &&  val1 >= 'a'  &&  val1 <= 'z')
               chxsetval (interim, pos, type, val1 + 'A' - 'a');
            blank = (val1 == ' ');
         }
      }
  
      else if (streq (what, "userid"))  {
         chxofascii (interim, conf->remote);
         jixreduce  (interim);
      }
  
      else if (streq (what, "userids_byname")) func_var (interim, what, arg);

      else if (streq (what, "userids_byid"  )) func_var (interim, what, arg);

      /*** $user_var(). */
      else if (strindex (what, "user_") == 0)
        { if (allow_functions) func_var (interim, what, arg); }

      else if (streq (what, "unlock")) {
         pos = 0;
         op1 = chxint4 (arg, &pos);
         if (op1 > 0)  syslocker (0, 1, op1);
      }

      else if (streq (what, "url_decode")) {
         ascofchx   (temp, arg, 0, TEMPMAX-1);
         fix_query  (temp);
         chxofascii (interim, temp);
      }

      else if (streq (what, "url_encode")) {
         urlEncode (arg, interim);
      }

      else if (streq (what, "unhtml"))  func_unhtml (interim, arg);

      else nofunc = 1;
      break;

   case 'v':
      if (streq (what, "version"))  chxofascii (interim, caucus_version());

      else if (streq (what, "variables")) {

         /*** If a list of words is supplied, display those vars. */
         if (chxtoken (word1, nullchix, 1, arg) >= 0) {
            for (which=1;   chxtoken (word1, nullchix, which, arg) >= 0;
               ++which) {
               var_show (interim, word1, vars, otab, cfile);
            }
         }

         else {
            Namelist list, node, nlnode();
  
            list = nlnode(1);
            for (slot=0;   slot < vars->used;   ++slot) {
               if (NOT EMPTYCHX (vars->name[slot]))
                  nladd (list, vars->name[slot], slot);
            }
            for (slot=0;   slot < otab->used;   ++slot)
               nladd (list, CQ(otab->obj[slot]->name), slot);
  
            for (node=list->next;   node!=(Namelist) NULL;   node=node->next)
               var_show (interim, node->str, vars, otab, cfile);

            nlfree  (list);
         }
      }

      else nofunc = 1;
      break;

   case 'w':
      if (streq (what, "word")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         if (val1 < 0) {
            for (which=pos=0;   chxnextword (word1, arg, &pos) >= 0;   ++which);
            val1 = which + val1;
         }
         if (val1 > 0) chxtoken (interim, nullchix, val1+1, arg);
      }
  
      else if (streq (what, "words")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         val2 = chxint4 (arg, &pos);
         if      (val1 <= 0)  ;
         else if (val2 <  0)  chxtoken (nullchix, interim, val1+2, arg);
         else for (num=0;  num < val2;   ++num) {
            if (chxtoken  (word1, nullchix, val1+2+num, arg) < 0)  break;
            chxcat    (interim, word1);
            chxcatval (interim, THIN, L(' '));
         }
      }

      else if (streq (what, "wordpos")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         pos  = chxtoken (word1, nullchix, 2, arg);
         pos2 = chxtoken (word1, nullchix, val1+1, arg);
         sprintf (temp, "%d", pos2 - pos);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "width")) {
         for (len=pos=0;   (val1 = chxvalue (arg, pos));   ++pos)
            len += jixwidth (val1);
         sprintf    (temp, "%d", len);
         chxofascii (interim, temp);
      }
  
      else if (streq (what, "write")) {
         val1 = '0';
         if (allow_functions && NOT protect) {
            chxtoken (word1, nullchix, 1, arg);
            ascofchx (temp, word1, L(0), L(200));
            mode = (streq (what, "write") ? "w" : "a");
            if (mode[0]=='w')  nt_mkfile  (temp, 0700);
            if (mode[0]=='a')  nt_chkfile (temp, 0700);
            if ( (fp = fopen (temp, mode)) != NULL) {
               val1 = '1';
               pos = chxtoken (word1, nullchix, 2, arg);
               if (pos >= 0) write_from_jix (fp, arg, pos, conf->diskform+1);
               fclose (fp);
            }
         }
         chxcatval (interim, THIN, val1);
      }

      /*** $WRAP2HTML  "Wrapped text"-to-HTML filter. */
      else if (streq (what, "wrap2html"))
         func_wrap2html (interim, arg, conf, cmlargs,
                                  vars, form, incvars, macs, cfile, tg, aref);
  
      /*** $WRAPTEXT(width text)  Wrap ordinary text for output. */
      else if (streq (what, "wraptext"))   wrap_text (interim, arg);
  
      else if (streq (what, "writeln")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos) - 1;
         if (val1>=0   &&  val1<HANDLES  &&  handle[val1]!=NULL) {
            ++pos;
            write_from_jix (handle[val1], arg, pos, conf->diskform+1);
            fputs ("\n", handle[val1]);
            success = '1';
         }
         else success = '0';
         chxcatval (interim, THIN, L(success));
      }

      /*** $WEEKDAY(time)  -- day of week, sunday=0, sat=6. */
      else if (streq (what, "weekday")) {
         pos  = L(0);
         val1 = chxint4 (arg, &pos);
         sysymd (val1, &year, &month, &day, &hour, &minute, &sec, &wday);
         sprintf    (temp, "%d", wday);
         chxofascii (interim, temp);
      }
  
      else nofunc = 1;
      break;

   case 'x':
      if (streq (what, "xshell")) {
         if (allow_functions && NOT protect) {
            pos = L(0);
            val1 = chxint4 (arg, &pos);
            val2 = chxint4 (arg, &pos);
            val3 = chxint4 (arg, &pos);
            chxtoken (nullchix, word1, 4, arg);
            func_shell (interim, (int) val1, (int) val2, (int) val3,
                                 word1, conf);
         }
      }

      else if (streq (what, "xmlesc")) func_xmlesc (interim, arg);

      else nofunc = 1;
      break;

   case 'y':
      if (streq (what, "yyyymmddOf")) {
         pos  = 0;
         val1 = chxint4 (arg, &pos);
         sysymd (val1, &year, &month, &day, &hour, &minute, &sec, &wday);
         sprintf (temp, "%4d-%02d-%02d %02d:%02d", 
                        year, month, day, hour, minute);
         chxcat     (interim, CQ(temp));
      }

      else nofunc = 1;
      break;
   }

   /*** Function does not exist.  Evaluate to " === func(value) ===". */
   if (nofunc) {
      chxcat   (result, CQ(" === "));
      chxcat   (result, name);
      chxcat   (result, CQ("("));
      chxcat   (result, arg);
      chxcat   (result, CQ(") === "));
      if (cml_debug) {
/*       fprintf (sublog, "No function '%s'\n", what); */
/*       fflush  (sublog); */
      }
      /*** E-Report. */
      sprintf (temp, "\n%s:%d No such function $%s()\n",
                      cfile->filename, cfile->lnums[cfile->cdex],
                      what);
      chxcat  (cfile->errtext, CQ(temp));
      DONE;
   }

   /*** In all cases (except "function does not exist"), now trim or
   /    pad INTERIM to the field width, and concatenate to RESULT. */
   if (width == 0)   { chxcat    (result, interim);              DONE; }

   sign  = (width > 0 ? 1 : -1);
   width = width * sign;
   len   = chxlen (interim);
   if (len > width)  { chxcatsub (result, interim, L(0), width);   DONE; }

   if (sign > 0)  chxcatsub (result, blanks, L(0), width-len);
   chxcat (result, interim);
   if (sign < 0)  chxcatsub (result, blanks, L(0), width-len);

done:
   chxfree (interim);
   chxfree (word1);
   chxfree (word2);
   chxfree (word3);
/* if (what[0]  &&  memused > oldused) {
      fprintf (stderr, "%9d %9d %3d %s\n", memused,
                       memused - oldused, level, what);
      oldused = memused;
   }
*/
   --level;
   RETURN  (1);
}


FUNCTION  char *format_number (int val, double dbl, int is_int, int precise) {
   static char temp[100];

   if      (is_int)   sprintf (temp, "%d",            val);
   else if (precise)  sprintf (temp, "%.*f", precise, dbl);
   else               sprintf (temp, "%.0f.",         dbl);
   return (temp);
}

FUNCTION char *caucus_version() {
   return ("5.139");
}
