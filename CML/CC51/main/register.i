#
#---REGISTER.I.  Check validity of answers provided by user to register.cml.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/30/00 Add textbox field type "_x".
#: CR 04/23/00 Use $set_per_xxx() in regfile.
#: CR 05/06/00 Add e-mail welcome, report on new user, mailing list.
#: CR 05/09/00 Change mailing list to use add2maillist shell script.
#: CR 01/28/01 Changes for interface-based new user reg page.
#: CR 03/25/01 Record registering dir name in ur__reg.
#: CR 06/25/02 add authentication method choice in arguments
#: CR 11/30/03 Add closing paren to "$or(..." call after userid & pw test.
#: CR 11/12/04 Use %urcregnums() instead of "count... 1000".
#: CR 12/03/04 Add newly registered userid to special group.
#: CR 02/26/05 Massive rewrite to use SQL/$user_info().
#: CR 07/31/05 Log adduser event.
#: CR 02/08/06 Turn on new-user-registration notification email (regreport.i)
#: CR 02/17/06 Handle creating users that are not activated (confirm email)
#: CR 02/21/06 Fix uploading images, include creating thumbnails.
#: CR 09/12/07 Set user_info.regface.  Only set fields if not-empty.
#: CR 09/05/08 Use $(http) instead of hard-coded http:
#---------------------------------------------------------------------------

#"Content-type: text/plain
#"

set reg_uid    0
set reg_verify 0

set regerrs
set emptyerrs


set query SELECT uc.name FROM user_columns uc, user_iface ui \
           WHERE uc.name = ui.name AND iface='$(iface)' \
             AND ui.on_reg > 0     AND ui.required > 0
set reqdFields $sql_query_select ($(query))
set reg_uid    $tablefind (userid $(reqdFields))

for field in $(reqdFields)

   #---Make sure required fields are not empty.
   if $empty ($form($(field)))
      if $empty ($(emptyerrs))
         set emptyerrs <p/>Some required fields (marked with a \
               <font color=red><b>*</b></font>) are empty. ($(field))
      end
   end

   #---Userid must be a single word.
   if $(reg_uid)
      if $not_empty ($word (2 $form(userid)))
         set regerrs <p/>Your userid must be a single word.
      end
   end

   #---Passwords must match
   if $(reg_uid)
      if $not_equal ($quote($form(_Password_)) $quote($form(_Verify-PW_)))
         set regerrs <p/>Passwords do not match.
      end
   end
end
set regerrs $(regerrs) $(emptyerrs)

if $(reg_uid)
   if $or ($empty ($form(_Password_)) $empty ($form(_Verify-PW_)))
      set regerrs <p/>Passwords may not be empty.
   end
end


##---Add registration interface name (e.g. CC44/reg) to list of variables
##   stored for user when user completes registration.
#set urcdir $str_sub (4 1000 $(urcbase))
#set urcdir $str_sub (0 $minus ($strlen($(urcdir)) 1) $(urcdir))
#set urgout $(urgout)\
#    eval \$set_user_data ($form(id) ur__regdir $(urcdir))$newline()
#set urgout $(urgout)\
#    eval \$set_user_data ($form(id) ur__time $dateof($time()))$newline()

##---Write commands to regfile.
#"if \$not_empty ($(fname)$(lname))
#"eval \$set_user_info  ($form(id) lname $t2esc($(lname)))
#"eval \$set_user_info  ($form(id) fname $t2esc($(fname)))
#"$(urgout)
#"eval \$set_user_info ($form(id) intro \$replace (01 10 \$(bintro)))
#"end
#include $(regis)/local_postreg.i $form(id)
#"eval \$delfile ($(regfile))
#eval $output()

set time0 $timems()

#---Go on to actually register the userid/pw?
if $and ($(reg_uid) $empty($(regerrs)))

   set success $pw_add ($form(userid) $form(_Password_) 1)
#  eval $append (/tmp/newUserReg reg: $minus ($timems() $(time0))$newline())
  
   #---ID already in use
   if $equal ($(success) 2)
      set regerrs $(regerrs) <P/><B>This user ID is already in use.&nbsp;</B> \
                  Please pick another, and try again.
   end
  
   elif $equal ($(success) 6)
      set regerrs $(regerrs) <P/>Your user ID must be <B>less than 80 \
                  characters</B> in length.&nbsp;  Please try again.
   end
  
   elif $equal ($(success) 7)
      set regerrs $(regerrs) <P/>Your user ID must contain only <B>lower case \
                  letters, numbers, an underscore "("_"), a dash ("-"), or \
                  a period (".")</B>.&nbsp; Please try again.
   end
  
   elif $equal ($(success) 8)
      set regerrs $(regerrs) <P/>I'm sorry, an unknown <B>system error</B> \
                  occurred.&nbsp; Please contact the system administrator \
                  at <A HREF="mailto:$admin_mail()">$admin_mail()</A>.
   end

   elif $not_equal ($(success) 0)
      set regerrs $(regerrs) <P/>Unknown error: Userid registration code \
                  $(success).
   end
end


#---If there are no registration errors, go ahead and finish
#   the registration process.
if $empty ($(regerrs))

   #---Actually create the user, and fill in their info fields.
   eval $set_user_info ($form(userid) lname $form(lname))
   eval $set_user_info ($form(userid) regface $(iface))
   eval $set_user_info ($form(userid) active 10)
#  eval $append (/tmp/newUserReg lname/regface/active: $minus ($timems() $(time0))$newline())

   set query SELECT uc.name, uc.type FROM user_columns uc, user_iface ui \
              WHERE uc.name = ui.name AND iface='$(iface)' \
                AND ui.on_reg > 0
   set h $sql_query_open ($(query))
   while $sql_query_row  ($(h))
      if $equal ($(type) upload)
         if $not_empty ($form(name.$(name)))
            set fname $replace (/ 32 $form(name.$(name)))
            set fname $word ($sizeof($(fname)) $(fname))
            include $(main)/virus_checker.i $form($(name)) $(fname) virus_file
            if $empty ($(virus_file))
               set url   $copy2lib ($form($(name)) PICTURES/$form(userid)/$(fname))
               eval  $set_user_info ($form(userid) $(name) $form(userid)/$(fname))
               include $(main)/methumb.i $form(userid) $(fname)
            end
         end
      end
      elif $not_empty($form($(name)))
         eval $set_user_info ($form(userid) $(name) $form($(name)))
#        eval $append (/tmp/newUserReg $(name): $minus ($timems() $(time0))$newline())
      end
   end

   eval  $sql_query_close($(h))

   #---Log user creation as an event.
   eval $logevent (adduser $(iface) 0 0 0 0 0 0 $form(userid))
#  eval $append (/tmp/newUserReg logevent: $minus ($timems() $(time0))$newline())

   #---Add user to all wildcard matched groups.
   include $(main)/group_addnewusers.i $form(userid)
#  eval $append (/tmp/newUserReg add wildcard matches: $minus ($timems() $(time0))$newline())

   #---Record registration via this interface, in special group.
   include $(main)/group_registered.i  $form(userid)
#  eval $append (/tmp/newUserReg record iface reg: $minus ($timems() $(time0))$newline())

#  #---Option to e-mail welcome, with id and password to newly registered user.
#  if $and ($not_empty ($site_data (0 - $(urcbase)welfrom)) \
#           $not_empty ($config(sendmail)) \
#           $not_empty ($form(em)))

#     eval $mac_define (id \$form(id))
#     eval $mac_define (pw \$form(pw))

#     #---Put it in a file and send it!
#     set newu To: $form(em)$newline()\
#              From: $site_data (0 - $(urcbase)welfrom)$newline()\
#              Subject: $site_data (0 - $(urcbase)welsubj)$newline()\
#              $newline()\
#              $mac_expand ($site_data (0 - $(urcbase)weltext))
#     eval $write  (/tmp/_c_$form(id) $wraptext (60 $(newu)))
#     eval $xshell (1 1 1 $config(sendmail) -t </tmp/_c_$form(id))
#  end

#  #---Add this user to mailing list?
#  if $and ($not_empty ($site_data (0 - $(urcbase)maillist)) \
#           $not_empty ($form(em)))
#     count tries 0 6
#        if $lock (11 5)
#           eval $xshell (0 1 1 \
#              $caucus_path()/add2maillist \
#              $caucus_path() $site_data (0 - $(urcbase)maillist) \
#              $form(em) $form(fn) $form(ln))
#           break
#        end
#     end
#     eval $unlock (11)
#  end

#  #---Report on this registration via e-mail?
   include $(regis)/regreport.i $form(userid)

   #---Do any locally-defined commands?
   include $(regis)/local_prereg.i $form(userid)

#  #---If this is preregistration, run a sweba script to register now,
#  #   and go to the "target" page.
#  if $not_empty($site_data (0 - $(urcbase)prereg))
#     eval $xshell (0 0 1 $caucus_path()/SWEB/sweba \
#                         $caucus_path()/SWEB/swebd.conf TMP/$form(id).urg)
#     "Location: $site_data (0 - $(urcbase)prereg)
#     "
#     eval $goodbye()
#  end

   #---If we require a correct email address, do not fully activate
   #   user, but send them an email with a clickable link that will
   #   complete the process.
   if 0$site_data (0 - reg_confirmEmail_$(iface))
      set confirmCode $random (1000000)
      eval $set_user_data ($form(userid) confirm $(confirmCode))
      eval $set_user_info ($form(userid) active 5)

      set tname /tmp/caucus.$caucus_id().confirmEmail.\
                            $word(1 $replace(/ 32 $pid()))
      eval $output ($(tname) 644)
      "To: $user_info($form(userid) email)
      "Subject: Please confirm $(iface) $host() Caucus registration
      "
      "Dear $user_info($form(userid) fname) $user_info($form(userid) lname) \
           ($form(userid)):
      "$wraptext (60 $lines(2 -1 $site_data (0 - reg_confirmText_$(iface))))
      "
      "  $(http)://$host()/~$caucus_id()/caucus.html?\
                          location=/@$(iface)&confirm=$(confirmCode)
      eval $output()
      eval $xshell  (1 1 1 $config(sendmail) -t <$(tname))
      eval $delfile ($(tname))

      "Content-type: text/html
      "
      "<html><body><blockquote>
      "<p/>
      "Your registration information has been received.<p/>
      "You have just been sent an email confirmation -- please follow the<br>
      "instructions in the email in order to complete your registration.
      "<p/>
      "Thank you.
      "</blockquote>
      "</body>
      "</html>
      eval $goodbye()
      quit
   end

   #---Otherwise, take the user straight into the Caucus Center.

   #---Method one exits this swebs process, and starts a new one
   #   from the web-server-protected sweb directory.
   if $equal ($form(method) 1)
      "Location: $(http)://$host()/$(sweb)/swebsock/0/0/$(main)/center.cml?\
                 0+0+x+x+x+x+x+x
      "
      set ignore $goodbye()
   end

   #---Method 2 continues with the same swebs process, but sets the
   #   userid/passwd cookies.
   else
      set sweb $(reg)
      set mainurl
      "Location: $(http)://$host()/$(sweb)/swebsock/$pid()/$(main)/center.cml?\
                 0+0+x+x+x+x+x+x
      "$auth_cookie ($form(userid) /$(reg)/swebsock/$pid())
      eval $auth_pw ($form(_Password_))
      "
   end

   quit
end
