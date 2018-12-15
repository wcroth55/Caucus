#
#---TUTOR2.I    Text of tutorial page 2.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#----------------------------------------------------------------------

"<A NAME="top">
"<H3>Structure of Caucus Conferences</H3>
"</A>

"<P>
"Caucus structures the virtual space on a
"<B>host computer site</B> into <B>conferences, 
"items</B>, and <B>responses</B>.&nbsp;
"Conferences are <B>where</B>
"specific groups of people meet; items are the "threads" of their
"discussions; responses are the individual comments by the people.

"<P>
"  Conferences can either be "open" to anyone having access to the
"  Caucus host, or "private" (reserved for the exclusive
"  use by a particular group).

"<P>
"  The diagram below shows one way to visualize this structure.&nbsp;
"  Each conference consists of a group of people 
"  (the <B>participants</B>), 
"  and the <B>items</B> they are discussing.

"<IMG SRC="$(img)/dstruct.gif" WIDTH=414 HEIGHT=346 ALT="">
" <P>
"  Each item is a separate discussion.&nbsp;
"  An item is started by a person, and then
"  the other participants may add their responses at any time.

"<P>
"The <B>responses</B> are appended over time to the end of an item, 
"and may contain ordinary text, HTML, graphics, word processor or other
"files, links to other pages, or even links to
"other conferences, items, or responses on the Caucus host.

"<P>
"When you look at an item, Caucus displays the contents of the
"item <B>and</B> all of its responses as a page in your browser.&nbsp;
"(If the item is very long, Caucus may break it up into separate
"pages, to improve your speed.)&nbsp;
"Caucus treats an item and its responses as a single
"<B>conversation</B> amongst a group of people, rather than as separate, 
"isolated pieces of data.

"<P>
"Caucus remembers what you have seen, and tells you about any
"new items or responses that have recently been added.&nbsp;
"In fact, most Caucus users just click <B>once</B> to see all
"of the new material in their favorite conferences.&nbsp;


"<P>
"<H3><IMG SRC="$(img)/clearpix.gif" WIDTH=1 HEIGHT=15 ALT=""><BR>
"Navigating through Caucus</H3>

"Caucus works with your browser to show you web pages that
"contain the information in the conferences, items, and
"responses.&nbsp;
"These web pages are <B>dynamic</B>; that just means 
"they may be different each time you look at them, as more
"information is added or changed in the conferences.

"<P>
"You <B>navigate</B> through these pages the same way you would
"on any web site; by clicking on the links and buttons you see on
"each page.&nbsp;
"(You can also use the <B>Back</B> and <B>Forward</B> buttons on
"your web browser to move around.)&nbsp;
"The layout of the Caucus pages echoes the structure of conferences,
"items, and responses shown in the diagram above.

"<UL>
"<LI>The <B>$(center_name)</B> corresponds to the "host" box in 
"    the diagram.&nbsp;
"    From here you can see all of the conferences on the host that
"    are open to you.&nbsp;
"    You can also choose some of them to put in your <B>Personal 
"    Conference List</B>, which will give you instant updates about
"    any new activity in those conferences.
"    <P>

"<LI><B>Conference Home Pages</B> correspond to the conference circles
"    in the diagram.&nbsp;
"    Each conference has its own home page, with specific 
"    information by and about the <B>organizer</B>, the person who
"    is in charge of the conference.&nbsp;
"    From a conference home page, you'll see a detailed list of the
"    new activity in the conference <B>items</B>, and you can also
"    search the conference for specific text, learn about the other
"    people in the conference, and much more.
"    <P>

"<LI>
"  <TABLE CELLSPACING=0 CELLPADDING=0>
"  <TR VALIGN=top>
"  <TD>
"    <B>Item Pages</B> correspond to the item and response columns in the 
"    diagram.&nbsp;
"    Each item page shows the content of the item and all (or some) of
"    its responses.&nbsp;
"    If you're reading <B>new</B> material, you'll see just the new
"    responses to an item.&nbsp;
"    If you're reading a very long item, it will be broken up into 
"    separate (smaller) pages.&nbsp;
"    In either case you can use the buttons shown on the right
"    to navigate among the pages of an item.
"  </TD>
"  <TD>&nbsp;&nbsp;</TD>
"  <TD><IMG $(src_1stpg)  VSPACE=1><BR>
"      <IMG $(src_prevpg) VSPACE=1><BR>
"      <IMG $(src_nextpg) VSPACE=1><BR>
"      <IMG $(src_lastpg) VSPACE=1>
"  </TD>
"  </TABLE>
"</UL>

"<P>
"<H3><IMG SRC="$(img)/clearpix.gif" WIDTH=1 HEIGHT=15 ALT=""><BR>
"Where Am I?&nbsp; The Caucus Compass Bar</H3>

"<P>
"Near the top of every Caucus page, you will see the <B>Compass Bar</B>,
"which tells you exactly where you are in relation to the
"$(center_name).&nbsp;
"For example, if you were in the middle of a long item in the
"<B>Demonstration</B> conference, you might see:<BR>

"<A NAME="wheream">
"<IMG SRC="$(icon)/clearpix.gif" WIDTH=1 HEIGHT=8></A><BR>
"<TABLE CELLSPACING=0 CELLPADDING=0>
"<TR VALIGN=top>
"<TD>&nbsp;&nbsp;</TD>
"<TD><IMG $(src_compass)></TD>
"<TD>&nbsp;</TD>
"<TD><NOBR>
   "<IMG SRC="$(img)/clearpix.gif" WIDTH=1 HEIGHT=$(dropline) ALT="">
   "<FONT COLOR=blue><U>$(center_name)</U></FONT>
   "</NOBR>
"</TD>

"<TD ALIGN=right>
"  <IMG SRC="$(img)/clearpix.gif" WIDTH=1 HEIGHT=$(dropline) ALT="">
   "$(compass_arrow)
   "<FONT COLOR=blue><U>Demonstration</U></FONT>
   "$(compass_arrow) <FONT COLOR=blue>
   "<U>Item 4: WWWCOOP discussion: Browsers and the HTML Language</U></FONT>
   "<NOBR>$(compass_arrow) <FONT COLOR=blue><U>Resp 27</U></FONT></NOBR>
"</TD>
"</TABLE>

"<P>
"This shows that you are inside the <B>Demonstration</B>
"conference, looking at <B>item 4</B>, titled
""WWWCOOP discussion: Browsers and the HTML Language".&nbsp;
"You are on a page that starts with <B>response 27</B> to that item.&nbsp;
"<P>
"The Compass Bar not only tells you where you are, but provides
"a convenient way to go to any of the intermediate pages.&nbsp;
"In the example, you could click on (and go directly to) the $(center_name),
"the Demonstration conference Home Page, or the beginning
"(first page) of Item 4.

"<P>
"Note that the Compass Bar doesn't tell you exactly how you <B>got</B>
"to the current page.&nbsp;
"(To see the path you took to get here, you can just click on
"your browser's <B>Go</B> button.)&nbsp;
"The Compass Bar just shows where you are in the Caucus
"structure diagram, and the names of the pages between here and the
"$(center_name).

