/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TMemInfo )
#define __TMemInfo

class TIDEMemInfo:public TView
{
public:
  TIDEMemInfo(const TRect & r, int _long_format =
              0):TView(r), p_mem(0), v_mem(0), long_format(_long_format)
  {
  };
  void update(int force = 0);
  virtual void draw();
  virtual void get_mem_info(unsigned long &virt_mem, unsigned long &phys_mem);

private:
  unsigned long p_mem, v_mem;
  int long_format;
};

void get_mem_info(unsigned long &, unsigned long &);

#endif
