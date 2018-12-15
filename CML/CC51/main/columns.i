#
#---COLUMNS.I    Get/set value of 'columns', for use by ilist.i
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#

set columns $site_data($arg(3) - columns)

if $empty($(columns))
   set columns number title author new last
end
