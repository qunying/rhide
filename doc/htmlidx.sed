# Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
s/comment --- change Info menu items to HTML links ---/&/

s|\([^:]*\): *\(.*\)\.$|<li><A HREF="#@w{\2}"> @w{\1} </A>@*|

s,comment --- change | < KEY > to | <kbd>KEY</kbd> ---,&,

s,| <\([^>]*\)>,| <kbd>\1</kbd>,
s,{<\([^>]*\)>,{<kbd>\1</kbd>,
