#
#---updateRespEditIdAndTime.i
#
#   inc(1) item_id
#   inc(2) rnum
#   inc(3) 1=new response
#
#---------------------------------------------------------------------
   set geoLocationPhrase
   if $inc(3)
      include $(main)/getNewLatLong.i
      set geoLocationPhrase , ip='$ip_addr()', lat_long='$(newLatLong)', location='$(newLocation)'
   end

   include $(main)/getCurrentVersionOfResponse.i $inc(1) $inc(2)
   set query UPDATE resps SET editid='$userid()', edittime=now() $(geoLocationPhrase) \
              WHERE items_id=$inc(1) AND rnum=$inc(2) AND version = $(currentVersion)
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
