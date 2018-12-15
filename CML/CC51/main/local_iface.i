#
#---local_iface.i   Localized interface switching.
#
#   This file is included during the initial setting of the
#   "interface" (iface) and related variables in startup.i.
#
#   If your initial interface needs to do some interface 
#   "switching" during startup, this is the place to do it.
#   If you do change the interface, you must set the variables:
#      iface     (interface directory, default is CC44)
#      main      $(iface)/main
#      help      $(iface)/help
#
#   For example, you might decide that anyone who is a member of
#   a certain conference "xyz" should actually be taken directly into
#   the XYZ interface.  So you would copy this file to your entry
#   interface's main.1 or equivalent directory, and add:
#
#      if $gt_equal ($group_access ($userid() CONF $cl_num(xyz)) 10)
#         set iface XYZ
#         set main  XYZ/main
#         set help  XYZ/help
#      end
#
#   Note that if you need any macros, you must define them yourself
#   in this file; the standard macros.i and related macro-defining
#   files have not yet been included at this point.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/20/04 New file.
#-------------------------------------------------------------------

