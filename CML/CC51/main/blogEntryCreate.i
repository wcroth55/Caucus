#
#---blogEntryCreate.i
#
#   inc(1)  target directory
#   inc(2)  file name (to create)
#   inc(3)  name of top-level blog
#   inc(4)  conf num
#   inc(5)  parent blog item
#   inc(6)  list of items for this blog page
#   inc(7)  list of all blog entries (for archive links)
#
#:CR 06/26/10
#-------------------------------------------------------------------

"  target= $inc(1)
"  name=   $inc(2)
"  items=  $inc(3)
"  entries=$inc(6)
"  entries=$inc(7)

eval $output($inc(1)/$inc(2) 644)

"<html>
"<head>
"<style type="text/css">
"   .header {
"      position: relative;
"      top: -10px;
"      left: -10px;
"      font-family: Arial, Helvetica, sans-serif;
"      font-size: 80%;
"   }
"   .mainDiv { 
"      text-align: left;
"      width: 90%; 
"      margin-left: auto;  
"      margin-right: auto; 
"      font-family: Arial, Helvetica, sans-serif;
"   }
"   .homeFeed {
"      font-family: Arial, Helvetica, sans-serif;
"      font-size: 80%; 
"   }

"   .archive {
"      float: right;
"      text-align: left;
"      width: 200px;
"      font-family: Arial, Helvetica, sans-serif;
"      font-size: 80%; 
"      margin-left: 2em;
"   }
"   .archiveText {
"      font-family: Arial, Helvetica, sans-serif;
"      font-size: 80%; 
"      white-space: nowrap;
"   }
"   .spacer {
"      width: 3em;
"   }
"   .archiveTitle {
"      color: green;
"   }
"   .blogTitle {
"      font-size: 140%; 
"      font-weight: bold;
"   }
"   .home {
"      font-size: 80%;
"   }
"   .entryTitle {
"      font-size: 80%; 
"   }
"   .entryTitleLeft {
"      color: red;
"   }
"   .entryTitleRight {
"      font-size: 80%;
"      white-space: nowrap;
"   }
"   .entryText {
"      font-size: 80%;
"   }
"   hr {
"      color: blue;
"   }
"</style>
"<script language="javascript">
"   function rss() {
"      alert('RSS is not yet enabled.');
"   }
"</script>
"</head>
"
"<body>
"<div class="header">
    set parentQuery SELECT text, prop FROM resps  \
                     WHERE items_id=$inc(5) AND rnum=0 \
                     ORDER BY version DESC LIMIT 1
    set h2 $sql_query_open ($(parentQuery))
    eval   $sql_query_row  ($(h2))
    eval   $sql_query_close($(h2))
    include $(main)/setProhibitedTags.i $inc(4) $inc(5) 0
    set m_inum $inc(5)
    set m_rnum 0
    include $(main)/showResponseText.i $(prop)
"<div>

"<div class="mainDiv">
"   <table cellspacing="0" cellpadding="0" border="0">
"   <tr valign="top">
"   <td>
       for articleId in $inc(6)
          set articleQuery \
              SELECT i.title, date_format(r.time, '%b %e, %Y') day, r.text, r.prop FROM items i, resps r \
               WHERE i.id=$(articleId) AND r.items_id=i.id AND r.rnum=0 AND r.approved=1 \
               ORDER BY r.version DESC LIMIT 1

          set h2 $sql_query_open ($(articleQuery))
          while  $sql_query_row  ($(h2))
"            <table cellspacing=0 cellpadding=0 width="100%">
"            <tr>
"            <td><div class="entryTitleLeft">$(title)</div></td>
"            <td align="right"><div class="entryTitleRight">$(day)</div></td>
"            </table>
"            <hr>

"            <div class="entryText">
"              <p>
               include $(main)/setProhibitedTags.i $inc(4) $inc(5) 0
               set m_inum $(articleId)
               set m_rnum 0
               include $(main)/showResponseText.i $(prop)
"            </div>
          end
          eval   $sql_query_close($(h2))
       end

"   </td>
"
"   <td><div class="spacer">&nbsp;</td>
"
"   <td>
"      <div class="homeFeed">
"         <table cellspacing="0" cellpadding="0">
"         <tr>
"         <td class="homeFeed">
"            <a href="index.html">Home</a> |
"            <a href="#" onClick="rss();">Subscribe to feed</a>&nbsp;
"         </td>
"         <td class="homeFeed">
"            <a href="#" onClick="rss();"
"                ><img src="$(http)://$host()/~$caucus_id()/GIF50/iRssFeed.gif" 
"                   border=0></a>
"         </td>
"         </table>
"      </div>
"      <p/>
"      &nbsp;
"      <p/>
"      <div class="archiveTitle">ARCHIVES</div>
"      <hr>
"      <div class="archiveText">
          for articleId in $inc(7)
             set articleQuery \
                 SELECT i.title FROM items i WHERE i.id=$(articleId)
"            <a href="$(articleId).html">$unquote($sql_query_select($(articleQuery)))</a><br>
          end
"      </div>
"   </td>
"   </table>

"</div>
"</body>
"</html>
eval $output()
