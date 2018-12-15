#
#---updateConfLastAccess.i   Update date/time of last access to this conf.
#
#   inc(1)  conference number (cnum)
#
#   To actually update confs.lastAccess, the Config parameter
#   updateConfLastAccess must have a non-empty value in swebd.conf.
#
# CR 09/26/2009 New script.
#------------------------------------------------------------------------

if $not_empty($config(updateConfLastAccess))
   if $not_equal (x$(lastCnumUpdated) x$inc(1))
      set lastCnumUpdated $inc(1)
      set qUpdate UPDATE confs SET lastAccess = unix_timestamp(now()) \
                   WHERE cnum=$inc(1)
      set hUpdate $sql_query_open ($(qUpdate))
      eval        $sql_query_close($(hUpdate))
   end
end
