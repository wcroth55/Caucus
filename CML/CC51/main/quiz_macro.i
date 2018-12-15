#
#---quiz_macro.i   Implementation of %quiz(key) macro.
#
#   inc(1)   $arg(2)-$arg(6) from caller
#   inc(2)   quiz key
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 02/01/06 New file.
#: CR 02/07/06 Add %quiz() with no args -> My Quizzes page.
#----------------------------------------------------------------------

if $empty($inc(2))
  "<a href="quiz_my.cml?$unique()+$inc(1)+x+x">My&nbsp;Quizzes</a>
  quit
end

set _title $sql_query_select (\
           SELECT DISTINCT q.title FROM quiz q, groups g \
            WHERE q.qkey=$inc(2) AND q.deleted=0 AND g.owner='Quiz' \
              AND g.name=q.qkey AND g.userid='$userid()' AND g.access >= $priv(minimum))
if $empty($(_title))
   "<i>(No quiz)</i>
end
else
   "<a href="quiz_take.cml?$unique()+$inc(1)+$inc(2)+x">$unquote($(_title))</a>
end

#-----------------------------------------------------------------------
