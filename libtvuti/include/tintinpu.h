/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TIntInputLine )
#define __TIntInputLine

class TIntInputLine:public TEnterInputLine
{
public:
  TIntInputLine(const TRect &, int, int _unsigned = 0);
  virtual void setData(void *);
  virtual void getData(void *);
  virtual uint32 dataSize();
};

#endif
