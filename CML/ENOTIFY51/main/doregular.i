#
#---DOREGULAR.I     Process and send e-notification for a specific 
#                   "regular" (no individual notification sched) user.
#
#   Invoked as:
#      doregular.i
#
#   Input variables:
#      cnum      conference number
#      uid       userid
#      groups    item groups (list of duplets of item group T values
#                             and item triplet list variable names)
#
#   Purpose:
#
#   How it works:
#
#   Known bugs:
#
#   History of revisions:
#: CR  3/04/99 16:12 First version.
#: CR  3/27/00 13:32 Use %bad_email_address() macro.
#: CR  8/01/05 Return if user chose "suspend notifications".
#: CR 12/16/05 ur_suspend is a $user_info() field.
#----------------------------------------------------------------------------

#---Ensure user has e-mail!
set user_addr $user_info ($(uid) email)
if $empty ($(user_addr))
#  "
#  "------------------------
#  "No e-mail for $(uid)
   return
end
if %bad_email_address ($(user_addr))
   return
end
if 0$user_info ($(uid) ur_suspend)
   return
end

#---Ensure user still has access to conference
if $less ($group_access ($(uid) CONF $(cnum)) 10)
#  "
#  "------------------------
#  "No access for $(uid)
   return
end

set mempty
set kind regular
include $(en)/sendmail.i groups mempty

#----------------------------------------------------------------------------
