#
#---quiz_prob_otherans.i   Display text of "other" answers to a quiz problem.
#
#   Quiz answers that go into a text box (one row or multi-row box) aren't
#   easily marked as correct/incorrect/etc., and don't show up in the
#   quiz_graphprob graph.  So we need a way to display just *those* 
#   answers in a list.
#
#   inc(1)  quiz key
#   inc(2)  problem key
#
#: CR 03/28/2006
#-------------------------------------------------------------------------
   set q2 SELECT akey, text1, type qtype, text2, answer, points, newrow \
            FROM quizanswers \
           WHERE pkey=$inc(2)  AND type IN ('text', 'box', 'number') \
           ORDER BY sortorder
   set h2 $sql_query_open ($(q2))
   while  $sql_query_row  ($(h2))
      "<b>$(text1) $(text2):</b>
      "<ul>

      set q3 SELECT DISTINCT answer FROM quizscore WHERE akey=$(akey) \
              ORDER BY answer
      set h3 $sql_query_open ($(q3))
      while  $sql_query_row  ($(h3))
         "<li class="liTight">$(answer)
      end
      eval   $sql_query_close($(h3))
      "</ul>

   end
   eval   $sql_query_close($(h2))
#-------------------------------------------------------------------------
