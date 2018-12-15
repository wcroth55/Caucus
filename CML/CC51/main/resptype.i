#
#---RESPTYPE.I     Show pull-down menu with type of response text.
#
#   inc(1)   form field name
#   inc(2)   1=>allow all text types, 0=>only allow permitted types
#   inc(3)   type of text (one of format, literal, html)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#-------------------------------------------------------------------

set type $if ($empty($inc(3)) format $inc(3))

set type.format
set type.html
set type.literal
set type.cml
set type.$(type) SELECTED

"&nbsp;<SELECT NAME="$inc(1)">
"  <OPTION $(type.format)  VALUE="format"  >word-wrapped
"  <OPTION $(type.literal) VALUE="literal" >"literal"
"  <OPTION $(type.html)    VALUE="html"    >HTML
if $or ($not_empty ($(type.cml))  $inc(2) \
        $not_equal (x$site_data($arg(3) - allowcml) x0) )
  "<OPTION $(type.cml)     VALUE="cml"     >CML
end
"</SELECT>&nbsp;

#------------------------------------------------------
