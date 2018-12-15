#
#---local_viewitem_title.i   Local changes to viewitem browser title
#
#   This file controls the title for a "viewitem" page that appears
#   in the browser title bar (usually at the very, very, top of the
#   window).  
#
#   Input:
#      vi_cnum    conf number
#      vi_inum    item id
#      title      item title
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/19/05 New file, based on a hack by Ray Lopez.
#--------------------------------------------------------------------

#---This file controls the title for a "viewitem" page that appears
#   in the browser title bar (usually at the very, very, top of th
#   window).  Here's the standard version:
include $(main)/head.i $arg(3) $quote($(cname) Item $item_label($(vi_cnum) $(vi_inum)) \
                       "$t2hbr($(title))") \
                       indvresp.js 0 1

#---One interesting customization is to show the total number of
#   responses in the title:
#include $(main)/head.i $arg(3) $quote($(cname) \
#                  Item $item_label($(vi_cnum) $(vi_inum)) \
#                  "$t2hbr($(title))" \
#                  %item_lastresp($(vi_inum)) total responses) \
#                  indvresp.js 0 1
#--------------------------------------------------------------------
