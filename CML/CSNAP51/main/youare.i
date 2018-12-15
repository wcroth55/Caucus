#
#---YOUARE.I    Display person's name as a link to their page.
#
#   Purpose:
#
#   Inputs:
#      inc(1)   userid
#
#   How it works
#
#   Notes:
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR 12/04/02 Add -p option, do not build/display personal info.
#==========================================================================

set show_youare 0

if $and ($(do_people) $(show_youare))
   set per_name $user_info ($inc(1) fname) $user_info ($inc(1) lname)
   "You are <A HREF="../PEOPLE/$inc(1).htm">$(per_name)</A>.
end

