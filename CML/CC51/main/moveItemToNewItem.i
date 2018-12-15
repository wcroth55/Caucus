#
#---moveItemToNewItem.i
#
#   Input
#      inc(1) source cnum
#      inc(2) source item label
#      inc(3) target cnum
#
# CR: 4/22/2010 New file
#---------------------------------------------------------------------

include $(main)/linkItemToNewItem.i $inc(1) $inc(2) $inc(3)

#---Finally, delete the old rows.
set deleteOldEntries \
    DELETE FROM conf_has_items WHERE cnum=$(sourceCnum) AND $(itemClause)
set h $sql_query_open ($(deleteOldEntries))
eval  $sql_query_close($(h))
