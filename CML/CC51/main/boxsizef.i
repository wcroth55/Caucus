#
#---BOXSIZEF.I   Process shrink/expand buttons inside a form-processor file.
#
#   Text area boxes have 2 or 3 fields, whose names are based on
#   the "object" name.  E.g., for "resp" objects, the fields are
#      tx_resp   (text)
#      tp_resp   (text type)
#      ti_resp   (title)
#
#  $inc(1)    name of text object ("resp", "greet", etc.)
#
#  $inc(2)    anchor fragment name (to return to, e.g. "#textbox")
#
#  $inc(3)    1 => there is a title, 0 otherwise
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#

   set tx_$inc(1) $form(tx_$inc(1))
   set tp_$inc(1) $form(tp_$inc(1))
   set ti_$inc(1) $form(ti_$inc(1))
   if $not_empty ($form(shrink.x))
      set cols $minus ($(cols) 5)
   end
   if $not_empty ($form(expand.x))
      set cols $plus  ($(cols) 5)
   end
   eval $set_user_data ($userid() boxwide $(cols))

   #---To return to the "calling" page, we need to disassemble the
   #   URL, and add the text, type, and title args onto the end, and
   #   aim it at the appropriate anchor.
   set saved $page_get ($arg(2))
   
   #---(Handle degenerate case)
   if $empty ($(saved))
      set saved center.cml 0+x+x+x+x+x+x
   end
   
   set base $word (1 $(saved))?$(nch)+$word(2 $(saved))+tx_$inc(1)+$(tp_$inc(1))
   if $inc(3)
      set base $(base)+ti_$inc(1)
   end
   "%location($(mainurl)/$(base)$inc(2))
#-----------------------------------------------------------------------
