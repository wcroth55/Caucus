#
#---ebox_chosen.i    Turn off pop-up "you have a choice of editors!" window.
#
#   This file is called (included) from ebox form processor pages
#   that are posting a response.  It makes sure to set the user var
#   'ebox' to something(!) so the user isn't annoyed by more pop-ups
#   about how they can now choose different editors.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  1/15/03 New file.
#------------------------------------------------------------------

   #---If user posted a response, and has not chosen a default editor,
   #   make it "(default)" so they're not bothered by the pop-up anymore!

   #---Take out the is_explorer55 once we have the applet editor in place!
   if $and (0$(alert_ebox) $empty ($user_data ($userid() ebox)))
      eval $set_user_data ($userid() ebox (default))
   end
