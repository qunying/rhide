# Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details 
# This file is part of RHIDE. 
s/@chapter \([0-9][0-9.]*\) \(.*$\)/chapentry{\2}{\1}/p
s/@unnumbered \([0-9][0-9.]*\) \(.*$\)/chapentry{\2}{\1}/p
s/@section \([0-9][0-9.]*\) \(.*$\)/secentry{\2}{\1}/p
s/@subsection \([0-9][0-9.]*\) \(.*$\)/subsecentry{\2}{\1}/p
s/@subsubsection \([0-9][0-9.]*\) \(.*$\)/subsubsecentry{\2}{\1}/p
