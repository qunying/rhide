# Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
s/comment --- extract Info indices to temporary files ---/&/

/^[0-9 .]*Index of key commands/,/^Tag Table:/ {
   s/^\* Menu://
   s/^\(\* \)\(.*\)/\2/w kindex.tmp
}
/^[0-9 .]*Index/,/^File:/ {
   s/^\* Menu://
   s/^\(\* \)\(.*\)/\2/w cindex.tmp
}
