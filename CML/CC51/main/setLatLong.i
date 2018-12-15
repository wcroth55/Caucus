#
#---setLatLong.i   Set user's latitude and longitude if desired.
#
#   If swebd.conf has the right keys configured, AND this interface
#   has the latitude_longitude user field enabled, AND there is
#   no value set for this user, THEN get the user's IP address,
#   and ask maxmind.com for the equivalent latitude & longitude...
#   which is often good, but can be quite wrong.  Save the result
#   or "(none)" in case of error.
#
# CR 05/14/08 New file.
# CR 06/29/08 Remove spaces from curlText before processing.
#---------------------------------------------------------------

#---Is swebd.conf configured with a maxmind.com web services key?
if $and ($not_empty ($config(MaxMindKey)) $not_empty($config(curl)))

   #---Does this interface have the latitude_longitude field enabled?
   if $plus (0 $sql_query_select (\
                SELECT 1 FROM user_iface WHERE iface='$(iface)' \
                   AND name='latitude_longitude'))
   
      #---If the user's value is empty, get it from the cache or maxmind.
      include $(main)/getLatLong.i
      if $empty  ($user_info ($userid() latitude_longitude))
         eval $set_user_info ($userid() latitude_longitude $(latlong))
      end
   end
end
#curlText=US,MI,Ann Arbor,42.273399,-83.713303

