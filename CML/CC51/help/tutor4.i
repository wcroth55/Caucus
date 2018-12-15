#
#---TUTOR4.I   Text of tutorial page 4.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  4/18/00 15:09 Substitute "attach" wording for "upload".
#----------------------------------------------------------------------

"<A NAME="top">
"<H3>Writing Your Own Responses</H3>
"</A>

"<P>
"Once you've seen an item and its responses, you'll be given
"a choice.&nbsp;  You can either

"<P>
"<UL>
"<LI>press&nbsp; <IMG $(src_pass) ALIGN=absmiddle>
"    (i.e. don't respond), and go on to the next item
"   <P>
"<LI>or add a response of your own.
"</UL>


"<P>
"You can respond as many times, or as often, as you like.&nbsp;
"Think of an item as a <B>conversation</B>, or a round-table 
"discussion, about a particular topic.&nbsp;
"At the end of an item, 
"you'll see a "response box"
"that looks like this:

"<P>
"<FORM METHOD=POST ACTION="help/tutor4.cml?0+0+x+x+x+x+x+x">
"<TABLE BORDER CELLPADDING=10>
"<TR>
"<TD>
   "<TABLE CELLSPACING=0 CELLPADDING=0>
   "<TR>
   "<TD>Enter your </TD>
   "<TD><FONT SIZE=-1>
        include $(main)/resptype.i   tp  1 $arg(10)
      "</FONT>
   "</TD>
   "<TD>response in the box,</TD>

   "<TD></TD>
   "<TD>&nbsp;<B>or</B> </TD>
   "<TD><IMG $(src_upload) HSPACE=3></TD>
   "<TD> a file.</TD>
   "</TABLE>

   "<TEXTAREA WRAP=physical NAME="resp" ROWS=4 COLS=40></TEXTAREA>

   #---All the buttons below the textbox.
   "<TABLE CELLSPACING=0 CELLPADDING=0>
   "<TR >
   "<TD><INPUT TYPE="submit" NAME="submit" VALUE="Post+View"></TD>
   "<TD><IMG SRC="$(icon)/clearpix.gif" WIDTH=4>it now.</TD>

   "<TR>
   "<TD><INPUT TYPE="submit" NAME="submit" VALUE="Post + Go"></TD>
   "<TD><IMG SRC="$(icon)/clearpix.gif" WIDTH=4>to next item</TD>
   "</TABLE>
"</TD>
"</TABLE>
"</FORM>

"To add a response, just <B>click in the bordered rectangle</B>
"(the "text box" with the little scroll bars)
"and type your response.&nbsp;
"This box will act like a very simple word-processor.&nbsp;
"You can use the backspace and delete keys to correct mistakes,
"and the mouse and the browser <B>Edit</B> menu bar to cut
"and paste text.&nbsp;

"<P>
"Just like a word processor, your text will "wrap" in the box as 
"you type.&nbsp;
"Anywhere that you press <B>Return</B> or <B>Enter</B>
"will be treated as the start of a new paragraph.&nbsp;
"(It's always a good idea to press <B>Return</B> twice
"to start a new paragraph.)

"<P>
"Once you are finished with your response, press one of the
"<B>Post</B> buttons.&nbsp;
"This saves your response, and adds it to the item.
"<UL>
"<LI><B>Post + View</B> saves your response and displays
"    the item again, with your response added at the end.&nbsp;
"    This is a good idea if you're writing a long or complicated 
"    response; if it doesn't look right, you can edit it.
"    <P>
"<LI><B>Post + Go</B> saves your response, and immediately
"    takes you on to the next item or page.
"</UL>

"<P>
"<H3><IMG SRC="$(img)/clearpix.gif" WIDTH=1 HEIGHT=15 ALT=""><BR>
"Advanced Response Options</H3>

"<P>
"The <B>word-wrapped</B> button is actually a pull-down menu;
"it controls how your text is treated.&nbsp;
"Click on it to see your choices.&nbsp;
"The "literal" choice will display your text exactly as you typed
"it, in a fixed-pitch font.&nbsp;
"The "HTML" choice interprets your text as HTML code, 
"and the "CML" choice is a special, expanded version of HTML.

"<P>
"The <B>Attach</B> button lets you attach files on your
"PC directly to your response.&nbsp;
"You can attach any kind of file, including plain text,
"graphic images, binary files, and so forth.

"<P>
"<H3><IMG SRC="$(img)/clearpix.gif" WIDTH=1 HEIGHT=15 ALT=""><BR>
"Try Caucus!</H3>

"<P>
"This is the end of the Caucus tutorial.&nbsp;
"While there are many more capabilities and features in Caucus,
"the point of the tutorial is to give you enough background
"and context to make sense of the features.&nbsp;
"From here you should proceed to the <B>$(center_name)</B>, 
"and join the <B>Demonstration</B> or some other conference,
"and try out the various features.

"<P>
"Each page has a <IMG $(src_help)> button near the top; click on
"it whenever you need assistance, or want to know more
"about the page you're on.&nbsp;
"From the Help button, you can also see this tutorial again,
"at any time.

