/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#if !defined( __TMemInfo )
#define __TMemInfo

class TIDEMemInfo : public TView
{
public:
  TIDEMemInfo(const TRect & r) : TView(r), p_mem(0), v_mem(0) {};
  void update(int force = 0);
  virtual void draw();
private:
  unsigned long p_mem,v_mem;
};

void get_mem_info(unsigned long &,unsigned long &);

#endif
