#
#---deleteResponse.i   Mark selected response as deleted.
#
#   inc(1)  item id
#   inc(2)  rnum
#   inc(3)  deletion epoch time
#   inc(4)  isOrganizer?  1=true, 0=false
#
# CR 05/01/2010
# CR 03/13/2011 Enforce user must be organizer, or own response.
#-------------------------------------------------------------

set query UPDATE resps SET deleted=$inc(3) WHERE items_id=$inc(1) AND rnum=$inc(2)
if $not($inc(4))
   set query $(query) AND userid='$userid()'
end

set h $sql_query_open ($(query))
eval  $sql_query_close($(h))

eval $logevent (delresp $(iface) 0        0    conf $inc(1) $inc(2) $inc(2) $userid())
