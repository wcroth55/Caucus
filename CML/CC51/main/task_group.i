#
#---task_group.i  Display details about a group for a task.
#
#   inc(1)   owner
#   inc(2)   name
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 10/05/05 New file.
#------------------------------------------------------------

if $equal ($inc(1) CONF)
   "%displayed_conf_name($inc(2))
end
elif $equal ($inc(1) Caucus4)
   "$inc(2)
end
elif $equal ($inc(1) Filesafe)
   "Filesafe $inc(2)
end
elif $equal ($inc(1) MGR)
   "Manager $inc(2)
end
else
   "$inc(1) $inc(2)
end
