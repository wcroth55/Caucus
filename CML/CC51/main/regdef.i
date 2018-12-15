#
#---REGDEF.I   Default fields for customizable user registration
#              page (and person page, and person change-info page).
#
#   inc(1)     "base" value for site-var names for these fields.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/21/00 14:06 Add default toolbar field type.
#: CR 05/07/00 20:42 Add space for urc macro field.
#: CR 01/21/01 11:26 Changes for interface-based new user reg page.
#--------------------------------------------------------------------------

eval $set_site_data (0 - $inc(1)1  0 0 0 tb 20 0 x x )
eval $set_site_data (0 - $inc(1)2  1 0 0 ht 20 0 x x Use this page to register \
        a <B>user ID</B> (or "<B>username</B>") and a <B>password</B><BR> \
        for Caucus.&nbsp; Your user ID identifies you to Caucus.&nbsp; \
        Choose a user ID<BR> and a password that you will remember.)
eval $set_site_data (0 - $inc(1)3  1 0 0 ht 20 0 x x &nbsp;<BR>\
        Fields marked with a <FONT COLOR=RED SIZE=+1>*</FONT> are \
        required.<BR>&nbsp;<BR>)
eval $set_site_data (0 - $inc(1)4  1 0 0 er 20 0 x x)
eval $set_site_data (0 - $inc(1)5  1 0 0 id 20 1 x x User ID: (all one word) )
eval $set_site_data (0 - $inc(1)6  1 0 1 pw 15 1 x x Password: )
eval $set_site_data (0 - $inc(1)7  1 0 1 pv 15 1 x x Password again, to confirm:)
eval $set_site_data (0 - $inc(1)8  1 1 1 fn 20 1 x x First (middle, etc.) Name:)
eval $set_site_data (0 - $inc(1)9  1 1 1 ln 20 1 x x Last Name:)
eval $set_site_data (0 - $inc(1)10 1 1 1 em 30 0 x x E-mail:)
eval $set_site_data (0 - $inc(1)11 0 1 1 hp 30 0 x x Home Page:)
eval $set_site_data (0 - $inc(1)12 0 1 1 pu 30 0 x x Picture URL:)
eval $set_site_data (0 - $inc(1)13 0 1 1 tp 20 0 x x Telephone:)
eval $set_site_data (0 - $inc(1)14 0 1 1 bi 40 0 x x Brief Introduction:)
