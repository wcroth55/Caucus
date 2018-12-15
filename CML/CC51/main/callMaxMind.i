#
#---callMaxMind.i   "Call" the MaxMind.com geolocation service.
#
#   Output: l
#      latlong      latitude/longitude based on user's current IP address.
#      geoLocation  city/state/country name, ditto.
#
#: CR 07/17/08 Extract common code from setLatLong.i
#: CR 08/19/10 Add geoLocation.
#------------------------------------------------------------------

#curlText=US,MI,Ann Arbor,42.273399,-83.713303

   set curlText $xshell (1 1 1 $config(curl) \
       "http://geoip1.maxmind.com/b?l=$config(MaxMindKey)&i=$ip_addr()")
   set latlong (none)
   if $greater ($str_index(, $(curlText)) 0)
      set curlText $replace(32 _ $(curlText))
      set curlText $replace(, 32 $(curlText))
      set latlong  $word (4 $(curlText)),$word(5 $(curlText))
      set geoLocation $unquote($replace(_ 32 $word (3 $(curlText)) $word(2 $(curlText)) $word(1 $(curlText))))
   end
