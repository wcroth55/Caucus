#
#---quiz_summary.i   Implementation of %quiz_summary(itemlabel) macro.
#
#   inc(1)   $arg(1)-$arg(8) from caller
#   inc(2)   quiz item label
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 05/19/06 New file.
#: CR 05/25/06 Rewrite to do permission check first.
#----------------------------------------------------------------------

set args $replace(+ 32 $inc(1))
set arg3 $word (3 $(args))

set itemid $item_id ($(arg3) $inc(2))
set qkey $sql_query_select (SELECT qkey FROM items WHERE id=$(itemid))

set _qs SELECT title, seesummary FROM quiz WHERE qkey=$(qkey)
set _qh $sql_query_open ($(_qs))
eval    $sql_query_row  ($(_qh))
eval    $sql_query_close($(_qh))

set access 1
if $equal ($(seesummary) takers)
   set access $gt_equal (%access($(arg3)) $priv(minimum))
end
elif $equal ($(seesummary) editors)
   set access $gt_equal (%access($(arg3)) $priv(instructor))
end

if $(access)
   "<a href="quiz_summary.cml?$inc(1)">Summary: $(title)</a>
end
else
   "<u>Summary: $(title)</u>
end

