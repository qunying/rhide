/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TInspector )
#define __TInspector

class TInspectList;

class TInspector:public TDialog
{
public:
  TInspector(const TRect & bounds, const char *Title);
  void update(const char *expr);
  virtual void changeBounds(const TRect & bounds);
protected:
    TInspectList * list;
};

#endif
