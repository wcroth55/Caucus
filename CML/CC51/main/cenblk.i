#
#---CENBLK.I   Interpret individual blocks on "Caucus Center" page.
#
#   Inputs
#     w1    1st character of center block
#     w2    word 2 and on ($rest(2 ...)) of center block
#     ib    center block #
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/03/00 15:48 Wrap <FORM> just around 'Join conf by name'.
#: CR 01/13/02 19:30 v, h, q options
#: CR 10/20/04 Apply $mac_expand() to w2.
#: CR 07/09/07 Apply extra $reval() to raw CML text or item.
#-----------------------------------------------------------------------

   #---Caucus Banner
   if $equal ($(w1) b)
      "<P>
      include $(main)/cen_adv.i
   end

   #---Caucus version
   elif $equal ($(w1) v)
      include $(main)/cen_version.i
   end

   #---Browser version
   elif $equal ($(w1) h)
      include $(main)/cen_browser.i
   end

   #---Caucus toolbar
   elif $equal ($(w1) q)
      include $(main)/cenrow1.i center
   end

   #---Your conferences
   elif $equal ($(w1) y)
      "<TABLE CELLSPACING=0 CELLPADDING=0 border=0>
      include $(main)/cen_myconfs.i
      "</TABLE>
   end

   #---Other conferences
   elif $equal ($(w1) o)
      "<TABLE CELLSPACING=0 CELLPADDING=0 border=0>
      include $(main)/cen_other.i
      "</TABLE>
   end

   #---Notebook entries
   elif $equal ($(w1) n)
      "<TABLE CELLSPACING=0 CELLPADDING=0>
      include $(main)/cen_note.i $quote($mac_expand($(w2)))
      "</TABLE>
   end

   #---Misc text
   elif $equal ($(w1) x)
      "$unquote($mac_expand ($(w2)))
   end

   #---(none)
   elif $equal ($(w1) 0)
   end

   #---raw CML text or an item
   elif $not_empty  ($rest (2 $site_data (0 - if_$(iface)_blk$(ib))))
      "$reval($mac_expand($reval ($rest (2 $site_data (0 - if_$(iface)_blk$(ib))))))
   end

#-----------------------------------------------------------------------
