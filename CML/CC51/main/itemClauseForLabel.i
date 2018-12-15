#
#---itemClauseForLabel.i
#
#   Input:
#      $inc(1)  item label (e.g. "1.2")
#
#   Output:
#      itemClause   matching SQL clause (e.g. "item0=1 AND item1=2")
#
# CR 04/22/2010
#------------------------------------------------------------------

set tuplet $replace(. 32 $inc(1))
set itemClause item0=$word(1 $(tuplet))
count i 2 $sizeof($(tuplet))
   set itemClause $(itemClause) AND item$minus($(i) 1)=$word($(i) $(tuplet))
end
