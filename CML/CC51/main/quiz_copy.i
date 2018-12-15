#
#---quiz_copy.i    Do the work to copy a quiz item.
#
#   inc(1)   source item id
#   inc(2)   target conf num
#   inc(3)   title to assign to newly copied quiz
#   inc(4)   'breakout' or 'new'
#   inc(5)   label of (parent) target item, if inc(4)=breakout
#
# Output:
#   newItem  id of newly created (quiz) item.
#
#:CR 01/20/2011  extracted from copy_quiz.cml.
#---------------------------------------------------------------------------

#eval $append(/tmp/work5 Q                  tgtConf=$inc(2) srcId=$inc(1) tgtLabel=$inc(5)$newline())
set sourceQkey $sql_query_select (SELECT qkey FROM items WHERE id=$inc(1))
set sourceType $sql_query_select (SELECT type FROM items WHERE id=$inc(1))

#---Create the empty quiz.
set q1 INSERT INTO quiz (qkey, userid, title, visible0, visible1, onepage) \
       VALUES (NULL, '$userid()', '$escsingle($inc(3))', \
                '$(visible0)', '$(visible1)', $(onepage))
set h1 $sql_query_open ($(q1))
eval   $sql_query_close($(h1))
set qnew $sql_last_insert()

#---Create the matching item.
set textprop 2
set bits 0
set parent 0
if $equal ($inc(4) breakout)
   set parent $item_id($inc(2) $inc(5))
   if $less ($(parent) 0)
      set parent 0
   end
end
set newItem $item_add ($inc(2) $(parent) 0 $userid() $inc(3))
if $less ($(newItem) 0)
   set newItem $item_add ($inc(2) 0 0 $userid() $inc(3))
end
eval $resp_add ($inc(2) $(newItem) $userid() $(textprop) $(bits) 0 &nbsp;)
set qq UPDATE items SET qkey=$(qnew), type='$(sourceType)' WHERE id=$(newItem)
set qh $sql_query_open ($(qq))
eval   $sql_query_close($(qh))

#---For each problem in turn...
set q2 \
    SELECT p.pkey, p.category, p.title, p.text, p.texttype, p.totalpoints, \
           p.reference \
      FROM quizmap m, quizproblems p \
     WHERE m.qkey = $(sourceQkey) AND m.pkey = p.pkey AND p.deleted=0 \
     ORDER BY m.sortorder
set    order 0
set h2 $sql_query_open ($(q2))
while  $sql_query_row  ($(h2))
   include $(main)/quiz_copyprob.i $(qnew) $(pkey)
end
eval   $sql_query_close($(h2))
