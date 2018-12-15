#
#---calculateProhibitedTagLists.i
#
#   Input:
#      inc(1)    conf num
#      tag_list  master prohibited tag list
#
#   Output:
#      org_taglist merged list of prohibited tags for an organizer
#      usr_taglist merged list of prohibited tags for a regular user
#
#   Builds the org and user tag lists out of the merge of the
#   manager tag list, and any existing user/org taglists for
#   this conference.
#
#   Just for reference, the full tag list is stored locally for both users
#   and organizers, but only the allow/prohibit field is allowed to be
#   different for organizers, so we can do most of our checking against the
#   local user list.
#
#:CR 03/10/2011
#---------------------------------------------------------------------

set org_taglist
set usr_taglist

#---For each tag-set in the master (manager-created) list of tags...
for mtag mpro mwhat in $replace (, 32 $(tag_list))

   #---An instance in the org set overrides, else use master.
   set oadd $(mtag),$(mpro),$(mwhat)
   for otag opro owhat in $replace (, 32 $site_data($inc(1) - tag_list_org))
      if $equal ($(mtag) $(otag))
         set oadd $(otag),$(opro),$(owhat)
         break
      end
   end
   set org_taglist $(org_taglist) $(oadd)

   #---An instance in the user set overrides, else use master.
   set uadd $(mtag),$(mpro),$(mwhat)
   for utag upro uwhat in $replace (, 32 $site_data($inc(1) - tag_list_user))
      if $equal ($(mtag) $(utag))
         set uadd $(utag),$(upro),$(uwhat)
         break
      end
   end
   set usr_taglist $(usr_taglist) $(uadd)
end
