#
#---getProhibitedTagList.i
#
#   Input:
#      inc(1) "bits" value for response
#      org_taglist  prohibited tag list for organizer
#      usr_taglist  prohibited tag list for regular users
#
#   Output:
#      prohibited
#
#-----------------------------------------------

   if $equal($inc(1) 16)
      set prohibited $(org_taglist)
   end
   else
      set prohibited $(usr_taglist)
   end
