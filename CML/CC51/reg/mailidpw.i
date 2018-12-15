#
#---MAILIDPW.I  Send mail to the newly registered user with their id and
#               password.
#
#   This file is set up for a specific event (Common Knowledge, 
#   "A Caucus Conversation with Nancy Dixon").  But with minor
#   modifications, it could be used for any event.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/30/00 13:03 Add textbox field type "_x".
#: CR 04/06/00 13:31 Use $config() to get mailx if possible.
#------------------------------------------------------------------------

if $and ($not_empty ($form(em)) \
         $greater ($str_index (@ $form(em)) 0) )

   #---Figure out location of mail client.
   set mailx /usr/bin/mailx
   if $empty ($config())
      set mailx $word (1 $config(mailx) $(mailx))
   end

   #---Set text of e-mail to be sent.
   set newu Thank you for registering for "A Caucus Conversation with Nancy\
    $newline()Dixon".  When the conference space opens on April 10, please\
    $newline()join us at http://www.caucuscare.com/commonknowledge.\
    $newline()\
    $newline()Your userid is "$form(id)", and your password is "$form(pw)".\
    $newline()Please keep them in a safe place.

    #---Put it in a file and send it!
    eval $write  (/tmp/_c_$form(id) $(newu))
    eval $xshell (1 1 1 $(mailx) -s "Welcome to Common Knowledge" \
                        $form(em) </tmp/_c_$form(id))
end
