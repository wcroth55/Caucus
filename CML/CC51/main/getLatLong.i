#
#---getLatLong.i   Get the user's latitude and longitude.
#
#   If swebd.conf has the right keys configured, AND this interface
#   has the latitude_longitude user field enabled, AND there is
#   no value set for this user, THEN get the user's IP address,
#   and their latitude & longitude.  (Use the cached table if
#   possible, otherwise ask maxmind.com for it based on the IP
#   address...  which is often good, but can be quite wrong.
#   or "(none)" in case of error.
#
#   Output:
#      latlong
#      geoLocation   (city/state/country)
#
# CR 08/09/10 New file.
#---------------------------------------------------------------

set latlong
set geoLocation

#---Is swebd.conf configured with a maxmind.com web services key?
if $and ($not_empty ($config(MaxMindKey)) $not_empty($config(curl)))

   #---Does this interface have the latitude_longitude field enabled?
   if $plus (0 $sql_query_select (\
                SELECT 1 FROM user_iface WHERE iface='$(iface)' AND name='latitude_longitude'))
   
      set latlong     $sql_query_select(SELECT latlong  FROM usersip2latlong WHERE userid='$userid()' AND ip='$ip_addr()')
      set geoLocation $sql_query_select(SELECT location FROM usersip2latlong WHERE userid='$userid()' AND ip='$ip_addr()')
      if $or ($empty($(latlong)) $empty($(geoLocation)))

         set latlong              $sql_query_select(SELECT latlong  FROM ip2latlong WHERE ip='$ip_addr()')
         set geoLocation $unquote($sql_query_select(SELECT location FROM ip2latlong WHERE ip='$ip_addr()'))
         if $or ($empty($(latlong)) $empty($(geoLocation)))
            include $(main)/callMaxMind.i
            set query REPLACE INTO ip2latlong (ip, latlong, location, added)  \
                            VALUES ('$ip_addr()', '$(latlong)', '$(geoLocation)', now())
            set h $sql_query_open ($(query))
            eval  $sql_query_close($(h))
         end
      end
   end
end
