/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TInspectList )
#define __TInspectList

class TInspectList:public TListBox
{
public:
  TInspectList(const TRect & bounds);
  void update(const char *expr);
};

#endif
