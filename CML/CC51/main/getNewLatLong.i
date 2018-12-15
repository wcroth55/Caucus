#
#---getNewLatLong.i   Calculate current latitude/longitude and location name.
#
#   Input:
#      latlong       default latlong as returned by MaxMind
#      geoLocation   default location name as returned by MaxMind
#
#   Output:
#      newLatLong    latitude/longitude
#      newLocation   location name
#
#--------------------------------------------------------------------------------
set newLatLong $word (1 \
      $sql_query_select(SELECT latlong  FROM usersip2latlong WHERE userid='$userid()' AND ip='$ip_addr()') \
      $(latlong))
set newLocation $unquote(\
      $sql_query_select(SELECT location FROM usersip2latlong WHERE userid='$userid()' AND ip='$ip_addr()'))
if $empty ($(newLocation))
   set newLocation $(geoLocation)
end
