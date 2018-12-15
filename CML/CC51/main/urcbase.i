#
#---URCBASE.I     Define values for urcbase and urtbase.
#
#   Included by cml pages that need some aspect of the (custom) new-user-
#   registration info (such as manager creating a new user, or a person
#   editing their own personal info).
#
#   If a new-user-reg interface value exists for REG$cml_dir(), then
#   use those values.  Otherwise, use the default "urcroot" values.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 01/21/01 11:26 Changes for interface-based new user reg page.
#: CR 09/02/04 Check for *any* REG_iface/reg_ variable.
#------------------------------------------------------------------------

if $empty ($list_site_data (0 - REG_$(iface)/reg_))
   set urcbase $(urcroot)
   set urtbase $(urtroot)
end
else
   set urcbase REG_$(iface)/reg_
   set urtbase REG_$(iface)/reg_t_
end
