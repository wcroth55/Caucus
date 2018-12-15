#
#---INDV_COP.I   Handle results of COPY from indvresp.cml.
#
# Input:
#   $form(sconf)     number of conference (to copy/move to)
#   $form(towhere)   how to copy: new, app, brk
#   $form(itemno)    what item to copy to
#   $form(anonly)    anonymous copy checkbox
#   checklist        list of responses
#   icnum            number of current conference
#   iinum            id of current item
#   rpage            name of page to return to
#
# Output:
#   to_cnum          target conference #
#   to_id            target item id.
#   to_rd            target response.
#     
# If the copy fails, indvresp.cml is reloaded with one of the
# following error codes:
#   1 no access to conference
#   2 no item supplied
#   3 no such item
#   4 adding failed (frozen?)
#   5 cannot create new item
#   6 copying not allowed
#   7 no responses in checklist
#   8 attempted recursive copy
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/13/00 23:43 Put "0" in front of use of "$form(anonly)" (copied by bug)
#: CR 10/18/02 %displayed_conf_name() calculates displayable conference name.
#: CR  5/12/03 Get conference number (not name) from sconf.
#: CR 11/29/03 Add anon flag to end of "copied" string.
#: CR 12/22/03 Updated conference 'modified' time variable.
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#: CR 10/13/04 Handle "don't preserve original date" option.
#: CR 11/24/07 Extract most code to itemCopy.i.
#-----------------------------------------------------------------------

if $equal ($(checklist) all)
   set targetItemId 0
   if $not_empty($form(itemno))
      set targetItemId $item_id ($form(sconf) $form(itemno))
   end
   include $(main)/copyItemRecursively.i $(icnum) $form(sconf) $(iinum) \
                   $(targetItemId) 0$form(anonly) 0$form(datenow)
end
else
   include $(main)/itemCopy.i $form(sconf) $form(towhere) $quote( $form(itemno)) \
             0$form(anonly) $quote($(checklist)) $(icnum) $(iinum) 0$form(datenow)
   set to_id $(newItem)
end

#---Handle error returns.
if $not_equal ($(rval) 0)
   if $equal($(rpage) indvresp.cml)
      "%location(indvresp.cml?$unique()+$(icnum)+$(iinum)+$(rval))
   end
   else
      "%location($(rpage)?$arg(1)+$arg(2)+$arg(3)+$arg(4)+$(rval)+\
                 $arg(6)+$arg(7)+arg(8)#Error)
   end
   quit
end

#------------------------------------------------------------------
