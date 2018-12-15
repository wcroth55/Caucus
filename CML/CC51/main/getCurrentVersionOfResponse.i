#
#---getCurrentVersionOfResponse.i
#
#   Input:
#      inc(1) item_id
#      inc(2) rnum
#
#   Output:
#      currentVersion
#---------------------------------------------------------------------------

set q SELECT MAX(version) FROM resps WHERE items_id=$inc(1) AND rnum=$inc(2)
set currentVersion $sql_query_select ($(q))
