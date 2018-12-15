#
#---mathsymvals.i    Definition of actual list of "math symbols".
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 05/04/2004 Add new (gif) symbols.
#-----------------------------------------------------------------

#---Symbol names and character entities (or images or whatever!)
#   in pop-up symbol table.  Each entry is a pair of the form:
#      name  [ptext,]rtext
#   where
#      name  name used to identify symbol in %sym() macro
#      rtext  HTML to appear in response
#      ptext  HTML to appear in pop-up window (if different from rtext)
#   if an rtext or ptext contains any actual blanks, that part must
#   be $quoted() -- for example:
#      someimg $quote(<img src="xyz1.gif">),$quote(<img src="xyz2.gif">)

set symnam
set symval
for sn sv in \
      left &larr;   harr &harr;  right &rarr;  uarr &uarr;  darr &darr; \
      ne   &ne;     asymp &asymp;   ge &ge;  le &le; lt &lt; gt &gt; \
      equiv &equiv; \
      divide &divide;  times &times;  dot &middot; plusmn &plusmn; \
      union  &cap; \
      not &not; \
      prod &prod;  sum &sum;  radic &radic;   \
      int &int;   int2  &int;&int;  int3 &int;&int;&int; \
      alpha &alpha;  Alpha &Alpha;           beta &beta;  Beta &Beta; \
      gamma &gamma;  Gamma &Gamma;           delta &delta;  Delta &Delta; \
      epsilon &epsilon;  Epsilon &Epsilon;   zeta &zeta;  Zeta &Zeta; \
      eta &eta;  Eta &Eta;                   theta &theta;  Theta &Theta; \
      iota &iota;  Iota &Iota;               kappa &kappa;  Kappa &Kappa; \
      mu &mu;  Mu &Mu;                       nu &nu;  Nu &Nu; \
      omicron &omicron;  Omicron &Omicron;   xi &xi;  Xi &Xi; \
      pi &pi;  Pi &Pi;                       rho &rho;  Rho &Rho; \
      sigma &sigma;  Sigma &Sigma;           tau &tau;  Tau &Tau; \
      upsilon &upsilon;  Upsilon &Upsilon;   phi &phi;  Phi &Phi; \
      chi &chi;  Chi &Chi;                   psi &psi;  Psi &Psi; \
      omega &omega;  Omega &Omega; \
      exist %mathimg(exists.gif) \
      forall %mathimg(forall.gif) \
      angle %mathimg(angle1.gif angle0.gif)  \
      contains %mathimg(contains1.gif contains0.gif)  \
      containsproperly %mathimg(containsproperly1.gif containsproperly0.gif)  \
      dbarrow %mathimg(doublebigarrow1.gif doublebigarrow0.gif)  \
      dlarrow %mathimg(doubleleftarrow1.gif doubleleftarrow0.gif)  \
      drarrow %mathimg(doublerightarrow1.gif doublerightarrow0.gif)  \
      infinity %mathimg(infinity1.gif infinity0.gif)  \
      intersect %mathimg(intersection1.gif intersection0.gif)  \
      ismemberof %mathimg(ismemberof1.gif ismemberof0.gif)  \
      isnotmemberof %mathimg(isnotmemberof1.gif isnotmemberof0.gif)  \
      isomorphism %mathimg(isomorphism1.gif isomorphism0.gif)  \
      notin %mathimg(notincludedin1.gif notincludedin0.gif)  \
      perpend %mathimg(perpendicular1.gif perpendicular0.gif)  \
      plusminus %mathimg(plusminus1.gif plusminus0.gif)  \
      propersubset %mathimg(propersubsetof1.gif propersubsetof0.gif)  \
      subset %mathimg(subsetof1.gif subsetof0.gif)  \
      therefore %mathimg(therefore1.gif therefore0.gif)  \
      sup2 x<sup>2</sup>,<sup>2</sup> \
      subi x<sub>i</sub>,<sub>i</sub>
   set symnam $(symnam) $(sn)
   set symval $(symval) $(sv)
end

