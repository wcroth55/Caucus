#
#---css.i   Include all CSS style sheets.
#
#
#   inc(1) 1 => override: do not apply leftBox rules.
#   inc(2) conference number (for conf-specific CSS), else 0
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 09/21/04 New file.
#:CR 03/30/05 Add button_label class for toolbar buttons.
#----------------------------------------------------------------------

#---CSS rules cascade in this order:
#   1. Browser-dependent rules written from CML
#   2. Standard caucus.css file
#   3. Local modifications local.css file
#   4. Interface-specific local css file
#   5. Interface-setting for overall font and size
#   6. User-chosen setting for overall font and size

if $and ($(is_netscape4) $not ($(is_netscape5)))
   return
end

#---(1) Left-indenting works slightly different on different browsers (sigh).
#   Maybe this should be moved to some kind of selector-kludge inside
#   caucus.css ?
"<style type="text/css">
   if $(is_explorer5)
      ".indented  { text-indent: -1.5em;   padding-left: 2em; }
      set button_label_opts font-size:  9px;  bottom: -2px;
   end
   else
      ".indented  { text-indent: -2em;     padding-left: 2em; }
      set button_label_opts font-size: 10px;  bottom: -7px;
   end

   ".button_label {
   "   position: absolute;   font-family: Arial;
#  "   width: 100%;  height: 100%;   left: 0px;      z-index: 5;
   "   width: 100%;  left: 0px;      z-index: 5;
   "   text-align: center;          color: #4f4f4f;
   "   $(button_label_opts)
   "}

"</style>

#---(2, 3) Get standard caucus.css, then override with local site modifications
#   if any.
"<link rel="stylesheet" type="text/css" href="/~$caucus_id()/caucus5.css">
"<link rel="stylesheet" type="text/css" href="/~$caucus_id()/local.css">

#---(4) Interface-specific css file
if $not_empty ($(css_file))
   "<link rel="stylesheet" type="text/css" href="/~$caucus_id()/$(css_file)">
end

set confCss $if ($equal($inc(2) x) 0 $inc(2))
"<link rel="stylesheet" type="text/css" href="/~$caucus_id()/conf$(confCss).css">

#---(5, 6) Interface-wide font/size settings, user-chosen font/size settings.
"<style type="text/css">
   set font_family $unquote ($word (1 $quote($(ucss_font)) $quote($(css_font))))
   set font_size   $unquote ($word (1 $quote($(ucss_size)) $quote($(css_size))))
"  body { font-family: $(font_family);  font-size: $(font_size); }
"</style>

if $and (0$(leftBox) $not(0$inc(1)))
   "<style type="text/css">
   "  body {
   "     margin: 0;
   "     padding: 0 0 0 $(leftBoxWidth);
   "     background: #fff;
   "  }
   "  div#leftBox {
   "     overflow: auto;
   "     height: 100%;
   "     width: $(leftBoxWidth);
   "     position: absolute;
   "     top: 0;
   "     left: 0;
   "     background: $(leftBoxColor);
   "     z-index: 10;
   "  }
   "</style>

   "<!--[if lte IE 7]>
   "<link rel="stylesheet" type="text/css" href="/~$caucus_id()/caucus5IE.css">
   "<![endif]-->
end
