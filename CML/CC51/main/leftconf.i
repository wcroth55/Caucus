#
#---leftconf.i   Detect and log leaving or entering a conference or item.
#
#   inc(1)   conference number (or 'x')
#   inc(2)   item number       (or 'x')   defaults to 'x' if not supplied.
#
#   globals:
#      event_cnum     most recent conference
#      event_inum     most recent item
#      time_cnum      time entering a (different) conference
#      time_inum      time entering a (different) item
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 06/01/03 New file.
#: CR 04/20/07 Log leaving conf in user_in_conf file.
#-------------------------------------------------------------------------

set new_cnum $if ($equal ($inc(1) x) 0 $inc(1))
set old_cnum $(event_cnum)
set inc2     $word (1 $inc(2) x)
set new_inum $if ($equal ($(inc2) x) 0 $(inc2))
set chg_inum 0

#---Did we change confs?
if $not_equal ($(new_cnum) $(event_cnum))
   set chg_inum 1

   #---Did we leave a conf?
   if $(event_cnum)
      eval $logevent (leftconf $(iface) $minus ($time() $(time_cnum)) \
                      0 $(event_cnum))

      set leftq INSERT INTO user_in_conf (userid, cnum, lastin) \
                VALUES ('$userid()', $(event_cnum), now()) \
                    ON DUPLICATE KEY UPDATE lastin=now()
      set lefth $sql_query_open ($(leftq))
      eval      $sql_query_close($(lefth))

      set event_cnum 0
   end

   #---Did we enter a conf?
   if $(new_cnum)
      set time_cnum  $time()
      set event_cnum $(new_cnum)

      set leftq INSERT INTO user_in_conf (userid, cnum, lastin) \
                VALUES ('$userid()', $(event_cnum), now()) \
                    ON DUPLICATE KEY UPDATE lastin=now()
      set lefth $sql_query_open ($(leftq))
      eval      $sql_query_close($(lefth))
   end
end

#---Did we change item?
if $or ($(chg_inum)  $not_equal ($(new_inum) $(event_inum)))

   #---Did we leave an item?
   if $(event_inum)
      eval $logevent (leftitem $(iface) $minus ($time() $(time_inum)) \
                      0 $(old_cnum) $(event_inum))
      set event_inum 0
   end

   #---Did we enter an item?
   if $(new_inum)
      set time_inum  $time()
      set event_inum $(new_inum)
   end
end
#-------------------------------------------------------------------------
