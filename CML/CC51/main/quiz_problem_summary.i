#
#---quiz_problem_summary.i   Implementation of %quiz_problem_summary(key) macro.
#
#   inc(1)   $arg(1)-$arg(9) from caller
#   inc(2)   problem key
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
# Provides link to quiz problem summary, if user is allowed to see it.
#
#: CR 05/25/06 New file.
#----------------------------------------------------------------------

set args $replace(+ 32 $inc(1))
set arg3 $word (3 $(args))

set _qs SELECT p.title, q.seesummary, q.qkey \
          FROM quizproblems p, quiz q, quizmap m \
         WHERE p.pkey = $inc(2)  AND m.pkey = $inc(2) AND m.qkey = q.qkey \
         LIMIT 1

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
   "<a href="quiz_summary.cml?$inc(1)+$inc(2)">Summary: $(title)</a>
end
else
   "<u>Summary: $(title)</u>
end
