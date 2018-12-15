//---blockBackspaceKey.js    Javascript to prevent the Backspace key
//   from making the user's browser "go back" one page.
//
//   Usage:
//      <script type="text/javascript" src="blockBackspaceKey.js"></script>
//
//   Note: 
//      Normally this script sets itself to handle "onkeydown"
//      events, in order to detect the pressing of the Backspace key.
//      If you need some other script to be "onkeydown" handler, 
//      just call blockBackspaceKey from it, like this:
//
//         if (! blockBackspaceKey(evt)) return false;
//
// Copyright (C) 2007 Charles Roth, www.caucus.com.  All rights reserved.
//
// License: this code is released under the BSD license, to wit: It may be
//    used by anyone for any purpose, as long as this attribution is
//    attached.  It is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// 05/08/2007 Charles Roth.  First version.
//-------------------------------------------------------------------

function blockBackspaceKey(evt) {
   evt = (evt) ? evt : ((window.event) ? event : null);
   if (evt  &&  evt.keyCode == 8) {
      var elem = (evt.target) ? evt.target : 
                   ((evt.srcElement) ? evt.srcElement : null);

      //---Allow normal backspace operation inside form fields!
      if (elem &&  elem.tagName && 
         (elem.tagName.toLowerCase() == "input" ||
          elem.tagName.toLowerCase() == "textarea") ) ;

      //---Otherwise cancel its effects.
      else {
         if (evt.cancelBubble)  evt.cancelBubble = true;
         return false;
      }
   }
   return true;
}

if (document.onkeydown == null) document.onkeydown = blockBackspaceKey;

//-------------------------------------------------------------------
