#
#---YOUARE.I
#
#: CR 04/07/01 14:00 Add "faculty" (organizer) group conf creation link
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#: CR 02/27/05 Display both manage & customize links at the same time.
#: CR 04/13/05 C5 code merge.
#--------------------------------------------------------------------

set args $arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)
set c         C
set you_cus   $gt_equal (%access($arg(3)) $(priv_instructor))

if $empty ($arg(4))
   set args x+x+x+x+x
end

"<FONT SIZE=-1
#   ><NOBR>You are %person($userid() $quote(%my_name())).&nbsp;&nbsp;</NOBR>
"   ><NOBR>You are %person($userid() $quote(%my_name())).</NOBR>
"   <br>

   set num_mgr_groups $sizeof ($mgr_groups ($userid()))
   if $and ($equal ($(num_mgr_groups) 2)  \
            $mgr_in_group ($userid() createConference))
      "You can
      "<A HREF="cus_create.cml?$(nch)+$arg(2)+$arg(3)+x+x+x+x+x"
      " %help(h_manage)>
      "create</A> new conferences$if ($not($(you_cus)) .&nbsp;&nbsp; , or )
      set c c
   end

   elif $(num_mgr_groups)
      "You can
      "<A HREF="manage.cml?$(nch)+$arg(2)+$arg(3)+x+x+x+x+x" %help(h_manage)>
      "manage</A> this site$if ($not ($(you_cus)) .&nbsp;&nbsp; , or )
      set c c
   end

   if $gt_equal (%access($arg(3)) $(priv_instructor))
      " <nobr><A HREF="custom.cml?$(nch)+$(nxt)+$arg(3)+x+x+x+x+x"
      "%help(h_customize)>$(c)ustomize</A> this conference.&nbsp;</nobr>
   end
"</FONT>

#-----------------------------------------------------------------------------
