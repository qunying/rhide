/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#include <tv.h>

__link ( RProject )
#if 0 // I have moved it to ideprj.cc
__link ( RIDEEditWindow )
#endif

#define N(CLASS,S)                        \
class CLASS                               \
{                                         \
public:                                   \
  static const char * const name;         \
};                                        \
                                          \
const char * const CLASS::name = #S;


N(TProject,\x01)
N(TDependency,\x02)
N(TDepCollection,\x03)
N(TFlagEntry,\x04)
N(TFlagCollection,\x05)
N(TOptions,\x06)
N(TFrame,\x07)
N(TScrollBar,\x08)
N(TIndicator,\x09)
N(TIDEEditWindow,\x0A)
N(TDirList,\x0B)
N(TIDEFileEditor,\x0C)
N(TParamList,\x0D)
N(TStringCollection,\x0E)
N(TNoCaseStringCollection,\x0F)
N(TInfViewer,\x10)
// N(TIDEInfWindow,\x11) // defined in idehelp.cc for now

