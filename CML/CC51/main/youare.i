#
#---YOUARE.I
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/07/01 14:00 Add "faculty" (organizer) group conf creation link
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#: CR 02/27/05 Display both manage & customize links at the same time.
#: CR 04/13/05 C5 code merge.
#: CR 09/04/09 Make all href targets explicitly go to mainWindow (in case of frames!)
#--------------------------------------------------------------------

set args $arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)
set c         C
set you_cus   $gt_equal (%access($arg(3)) $priv(instructor))

if $empty ($arg(4))
   set args x+x+x+x+x
end

"   <nobr>You are 
"      <a href="$(mainurl)/me.cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)+$userid()"
"         target="$(mainWindow)" %help(h_youare)
"      >$user_info($userid() fname) $user_info($userid() lname)</a>.
"   </nobr>
"   <br>

   set num_mgr_groups $sizeof ($mgr_groups ($userid()))
   if $and ($equal ($(num_mgr_groups) 2)  \
            $mgr_in_group ($userid() conferenceCreate))
      "You can
      "<A HREF="$(mainurl)/cus_create.cml?$(nch)+$arg(2)+$arg(3)+x+x+x+x+x"
      " %help(h_manage) target="$(mainWindow)">
      "create</A> new conferences$if ($not($(you_cus)) .&nbsp;&nbsp; , or )
      set c c
   end

   elif $(num_mgr_groups)
      "You can
      "<A HREF="$(mainurl)/manage.cml?$(nch)+$arg(2)+$arg(3)+x+x+x+x+x" 
      "   %help(h_manage)  target="$(mainWindow)">
      "manage</A> this site$if ($not ($(you_cus)) .&nbsp;&nbsp; , or )
      set c c
   end

   if $gt_equal (%access($arg(3)) $priv(instructor))
      " <nobr><A HREF="$(mainurl)/custom.cml?$(nch)+$(nxt)+$arg(3)+x+x+x+x+x"
      "%help(h_customize)>$(c)ustomize</A> this conference.&nbsp;</nobr>
   end

#-----------------------------------------------------------------------------
