#
#---css.i   Include all CSS style sheets.
#
#:CR 09/21/04 New file.
#:CR 03/30/05 Add button_label class for toolbar buttons.
#:CR 01/08/07 Add CSS localizations by interface.
#----------------------------------------------------------------------

#---CSS rules cascade in this order:
#   1. Browser-dependent rules written from CML
#   2. Standard caucus5.css file
#   3. Local modifications local.css file
#   4. Interface-specific local css file
#   5. Interface-setting for overall font and size
#   6. User-chosen setting for overall font and size

#---(1) Left-indenting works slightly different on different browsers (sigh).
#   Maybe this should be moved to some kind of selector-kludge inside
#   caucus5.css ?
"<style type="text/css">
#  if $(is_explorer5)
      ".indented  { text-indent: -1.5em;   padding-left: 2em; }
#  end
#  else
#     ".indented  { text-indent: -2em;     padding-left: 2em; }
#  end

#     set button_label_opts font-size:  9px;  bottom: -2px;
      set button_label_opts font-size: 10px;  bottom: -7px;

   ".button_label {
   "   position: absolute;   font-family: Arial;
   "   width: 100%;  left: 0px;      z-index: 5;
   "   text-align: center;          color: #4f4f4f;
   "   $(button_label_opts)
   "}

"</style>

#---(2, 3) Get standard caucus5.css, then override with local site modifications
#   if any.
"<link rel="stylesheet" type="text/css" href="../caucus5.css">
"<link rel="stylesheet" type="text/css" href="../local.css">

#---(4) Interface-specific css file
if $not_empty ($(css_file))
   "<link rel="stylesheet" type="text/css" href="../$(css_file)">
end

#---(5, 6) Interface-wide font/size settings, user-chosen font/size settings.
"<style type="text/css">
   set font_family $unquote ($word (1 $quote($(ucss_font)) $quote($(css_font))))
   set font_size   $unquote ($word (1 $quote($(ucss_size)) $quote($(css_size))))
"  BODY { font-family: $(font_family);  font-size: $(font_size); }
"</style>
