
/*** FUNC_PW.   PassWord (and userid) functions.
/
/    code = func_pw (result, what, arg, allow, conf);
/   
/    Parameters:
/       int      code;    (integer value of return code)
/       Chix     result;  (append resulting value here)
/       char    *what;    (function name)
/       Chix     arg;     (function argument)
/       int      allow;   (allow protected functions)
/       SWconf   conf;    (sweb configuration params)
/
/    Purpose:
/       Evaluate the CML functions:
/          $pw_can_add()
/          $pw_can_change()
/          $pw_can_delete()
/          $pw_can_reset ()
/          $pw_can_verify()
/          $pw_add    (id pw [override])
/          $pw_change (id pw)
/          $pw_delete (id)
/          $pw_verify (id pw)
/          $pw_source ()
/
/    How it works:
/
/    Returns (for pw_can... functions)
/             1 on success
/             0 if not allowed
/
/    Returns: (for pw_... functions)
/             0 on success
/            -1 on "not allowed" by permission bits or swebd.conf PW_CAN...
/             1 on bad action
/             2 on userid already exists (add a user) 
/             3 on userid does not exist (chg password) 
/             4 on bad password
/             6 on userid too int4 (add)
/             7 on bad chars in userid (add)
/             8 on temp password file busy 
/             9 on can't read  password file 
/            10 on can't write to temp password file 
/            11 on error on close, disk could be full 
/            12 auth source does not permit this action
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/funcpw.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/13/97 New function. */
/*: CR 01/25/02 Add pw_can_reset. */
/*: CR 09/18/02 Add ldap verify */
/*: CR 01/28/03 Add ldap_opt arg to ldap_verify(). */
/*: CR 01/03/04 Revamp handling of optional method parameter. */
/*: CR 09/17/04 pw_add(), _del, _chg uses 2nd method (if there is one)
/               and ldap is 1st method. */
/*: CR 01/29/08 Call pw_extern with pw_prog. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  int  func_pw (Chix result, char *what, 
                        Chix arg,    int allow, SWconf conf)
{
   static char source[100] = {""};
   int    value, override, code, internal, prog, ldap, rc;
   char   temp[256], id[200], self[200], pw_prog[200], pw_prog2[200];
   char   arg2[200], arg3[200], arg4[200];
   char  *over, *method;
   Chix   word, ignore;
   char  *strtoken();
   ENTRY ("func_pw", "");

   if (strindex (what, "pw_can") == 0) {
      value = 0;
      ascofchx (temp, arg, L(0), L(200));
      strlower (temp);
      ldap = streq (temp, "ldap");

      if      (streq (what, "pw_can_add"))     
               value = (ldap ? conf->ldap_can_add    : conf->pw_can_add);

      else if (streq (what, "pw_can_change"))
               value = (ldap ? conf->ldap_can_change : conf->pw_can_change);

      else if (streq (what, "pw_can_delete"))
               value = (ldap ? conf->ldap_can_delete : conf->pw_can_delete);

      else if (streq (what, "pw_can_reset" ))
               value = (ldap ? conf->ldap_can_reset  : conf->pw_can_reset);

      else if (streq (what, "pw_can_verify")) 
               value = (ldap ? conf->ldap_can_verify : conf->pw_can_verify);

      chxcatval (result, THIN, value ? L('1') : L('0'));
      RETURN (value);
   }

   /*** Password verification source (e.g. "ldap" or "internal") */
   if (streq (what, "pw_source")) {
      chxcat  (result, CQ(source));
      RETURN (0);
   }

   /*** What pw methods are supported?  (e.g. "ldap", "internal", 
   /    "ldap internal")... */
   if (streq (what, "pw_methods")) {
      chxcat (result, CQ(conf->pw_prog));
      RETURN (0);
   }

   /*** Parse ID, PW, OVERRIDE, and METHOD arguments. */
   ascofchx (temp, arg, L(0), L(200));
   strtoken (id,   1, temp);
   strtoken (arg2, 2, temp);
   strtoken (arg3, 3, temp);
   strtoken (arg4, 4, temp);

   method = "";
   if      (streq (what, "pw_add")   ) method = arg4;
   else if (streq (what, "pw_delete")) method = arg2;
   else if (streq (what, "pw_change")) method = arg3;

   if (NULLSTR(method)) {
      strtoken (pw_prog,  1, conf->pw_prog);
      strtoken (pw_prog2, 2, conf->pw_prog);
      if (streq (pw_prog, "ldap")  &&  NOT NULLSTR(pw_prog2))
         strcpy (pw_prog, pw_prog2);
      method =  pw_prog;
   }
   internal = streq (method, "internal")  ||  NULLSTR(method);
   ldap     = streq (method, "ldap");

   /*** Get SELF, the current userid. */
   word   = chxalloc (L(20), THIN, "func_pw word");
   ignore = chxalloc (L(20), THIN, "func_pw ignore");
   sysuserid (word);
   ascofchx  (self, word, L(0), L(200));

   code     = -1;   /* resulting error or success code.  -1 => not allowed. */

   /*** ADD requires OVERRIDE arg or MGR_MKID manager bit. */
   if (streq (what, "pw_add")  &&  conf->pw_can_add  &&  allow) {
      over     = arg3;
      override = (NULLSTR(over) ? 0 : atoi(over));
      if (override || mgr_in_group ("userCreate systemAllPrivileges")) {
         if (ldap)  code = 12;
         else {
            code = (internal
                 ? pw_actions ("add", conf->pw_path, id, arg2, conf->pw_lock) 
                 : pw_extern  ("add", conf->pw_path, id, arg2, conf->pw_lock,
                                      conf->pw_prog) );
         }
      }
   }

   /*** CHANGE can be done by self, or with MGR_CHGPASS manager bit,
   /    or by REG (no userid) interface if can_pw_reset is on. */
   if (streq (what, "pw_change")  && conf->pw_can_change  &&  allow) {
      if (streq (self, id) || 
          mgr_in_group ("passwordChangeUsers passwordChangeManagers "
                        "passwordChangeAnyone systemAllPrivileges")   ||
         (NULLSTR(conf->remote)  &&  conf->pw_can_reset) ) {
         if (ldap)  code = 12;
         else {
            code = (internal
              ? pw_actions ("change", conf->pw_path, id, arg2, conf->pw_lock) 
              : pw_extern  ("change", conf->pw_path, id, arg2, conf->pw_lock,
                                      conf->pw_prog) );
         }
      }
   }

   /*** DELETE requires MGR_RMID manager bit. */
   if (streq (what, "pw_delete")  &&  conf->pw_can_delete  &&  allow) {
      if (mgr_in_group ("userDelete systemAllPrivileges")) {
         if (ldap)  code = 12;
         else {
            code = (internal
               ? pw_actions ("delete", conf->pw_path, id, arg2, conf->pw_lock) 
               : pw_extern  ("delete", conf->pw_path, id, arg2, conf->pw_lock,
                                       conf->pw_prog) );
         }
      }
   }

   /*** Anyone can verify... at least, so far. */
   if (streq (what, "pw_verify")  &&  conf->pw_can_verify  &&  allow) {

      /*** Check against all supported methods... */
      for (prog=1;   strtoken (pw_prog, prog, conf->pw_prog) != NULL;  ++prog) {
         if (streq (pw_prog, "ldap")) {
            rc = ldap_verify (id, arg2, conf->ldap_host, conf->ldap_port,
                              conf->ldap_opt, conf->ldap_bindas, 
                              conf->ldap_userdn, conf->ldap_pwname);
            if      (rc ==  0)  code = 0;
            else if (rc == -5)  code = 4;
            else                code = 9;
         }
         else {
            code = (streq (pw_prog, "internal")
                ? pw_actions ("verify", conf->pw_path, id, arg2, conf->pw_lock) 
                : pw_extern  ("verify", conf->pw_path, id, arg2, conf->pw_lock,
                                        pw_prog) );
         }
         if (code == 0)  break;
      }

      /*** Save source of verification the 1st time it succeeds. */
      if (code == 0  &&  NULLSTR(source))  strcpy (source, pw_prog);
   }

   sprintf (temp, "%d", code);
   chxcat  (result, CQ(temp));

   chxfree (word);
   chxfree (ignore);
   RETURN  (code);
}
