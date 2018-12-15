#
#---itemTitle.i   Implementation of %item_title()
#
#   inc(1) item_id
#
#---%itemtitle(inum)
"$unquote($sql_query_select(SELECT title FROM items WHERE id=$inc(1)))
