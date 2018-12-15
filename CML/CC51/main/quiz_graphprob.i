#
#---quiz_graphprob.i.   Graph an entire problem.
#
#   inc(1)   quiz key
#   inc(2)   problem key
#   divisor  number of people who answered problem.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/27/06 New file, hacked from old quiz_prob.cml.
#-----------------------------------------------------------------------


set graphColors #FF0000 #FF00FF #0000FF #00FFFF #00FF00 #FFFF00
set q4 SELECT akey, text1, type, text2, answer, points, newrow \
         FROM quizanswers \
        WHERE pkey=$inc(2) AND type IN ('radio', 'check', 'tf') \
        ORDER BY sortorder

set h4 $sql_query_open ($(q4))
set class grCellTop

set header <div style="page-break-inside: avoid;">\
           <table cellspacing=0 cellpadding=0>
set row -1
while  $sql_query_row  ($(h4))
   if $equal ($(type) tf)
      if $not_empty($(header))
         "$(header)
         set header
      end
      set q5 SELECT count(*) FROM quizscore \
              WHERE pkey=$inc(2) AND akey=$(akey) AND answer='t' AND status >= 2
      include $(main)/quiz_graphrow.i 2 True
      set text1
      set text2
      set class grCell
      set q5 SELECT count(*) FROM quizscore \
              WHERE pkey=$inc(2) AND akey=$(akey) AND answer='f' AND status >= 2
      include $(main)/quiz_graphrow.i 0 False
      set q5
      set text1 &nbsp;
      include $(main)/quiz_graphrow.i 1 
   end
   elif $tablefind ($(type) radio check)
      if $not_empty($(header))
         "$(header)
         set header
      end
      set q5 SELECT count(*) FROM quizscore \
              WHERE pkey=$inc(2) AND akey=$(akey) AND status >= 2
      include $(main)/quiz_graphrow.i 1
   end

   set class grCell
end

if $empty($(header))
   #---Bottom edge of graph.
   "<tr height=1>
   "<td></td>
   "<td></td>
   count i 1 10
      "<td><div class="grBottom">&nbsp;</div></td>
   end
   
   #---Percentage numbers below bottom of graph.
   "<tr><td></td> <td></td>
   "<td><div class="grPcnt">&nbsp;0</div></td>
   "<td><div class="grPcnt">10</div></td>
   "<td><div class="grPcnt">20</div></td>
   "<td><div class="grPcnt">30</div></td>
   "<td><div class="grPcnt">40</div></td>
   "<td><div class="grPcnt">50</div></td>
   "<td><div class="grPcnt">60</div></td>
   "<td><div class="grPcnt">70</div></td>
   "<td><div class="grPcnt">80</div></td>
   "<td><div class="grPcnt">90</div></td>
   "<td><div class="grPcnt">100</div></td>
   "</table></div>
end


eval $sql_query_close($(h4))
