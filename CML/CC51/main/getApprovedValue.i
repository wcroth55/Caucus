#
#---getApprovedValue.i
#
#   Input
#      inc(1)  item id
#
#   Output
#      approved
#
# CR 07/25/10
#---------------------------------------------------------------------------------

set apQuery \
       SELECT r.approved FROM resps r \
        WHERE r.items_id = $inc(1)  \
          AND r.rnum = 0 AND r.version =  \
              (SELECT MAX(q.version) FROM resps q WHERE q.items_id=$inc(1) AND rnum=0)

set approved $sql_query_select ($(apQuery))
