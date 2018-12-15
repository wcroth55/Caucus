#
#---viewitemBlogApprover.i
#
#   Input
#     vi_inum   item #


set apQuery \
       SELECT r.approved FROM resps r, items i \
        WHERE i.id = $(vi_inum) AND i.id = r.items_id \
          AND i.type = 'modblog' \
          AND r.rnum = 0 AND r.version =  \
              (SELECT MAX(q.version) FROM resps q WHERE q.items_id=$(vi_inum) AND rnum=0)
set apResult $sql_query_select ($(apQuery))
set args $arg(2)+$arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)
if $equal ($(apResult) 0)
   "<a href="blogApprover.cml?1+$(args)"  title="Approve this blog entry">Approve</a> |
   "<a href="blogApprover.cml?-1+$(args)" title="Reject this blog entry">Reject</a> |
end
elif $equal ($(apResult) -1)
   "<a href="blogApprover.cml?1+$(args)"  title="Approve this blog entry">Approve</a> |
end

