#
#---setPersonName.i   Set person info to display with an item or response.
#
#   Input:
#      inc(1)           popup=>link is always a pop-up, empty=>may link to self page.
#      owner            userid of owner of response
#      mail_out         email participation userid
#      author_shows_as  How author name should be displayed for email responses
#      fname            owner's first name
#      lname            owner's last name
#
#   Output:
#      personNameLink   active link with person's name
#      personNameNote   text that appears after person's name (e.g. (userid))
#
#-----------------------------------------------------------------------------------

set personNameNote
if   $equal (x$(owner) x$(mail_out))
   set personNameLink  $(author_shows_as)
   set personNameNote  (via email)
end
elif $equal ($(owner) _)
   set personNameLink Anonymous
end
elif $empty ($(fname)$(lname))
   set personNameLink ($(owner))
end
else
   set personNameLink  %person($(owner) $quote($(fname) $(lname)))
   if $equal (x$inc(1) xpopup)
      set personNameLink %personpop($(owner))
   end
   set personNameLink $(personNameLink)  \
       $ifthen(0$(showUserid) <span class="response_uid">($(owner))</span>)
end
