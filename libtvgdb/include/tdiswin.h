/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TDisassemblerWindow )
#define __TDisassemblerWindow

class TRect;
class TEvent;
class TDisassemblerViewer;
class TRegisters;
class TPalette;

typedef int (*check_func) (unsigned long);

class TDisassemblerWindow:public TDialog
{
public:
  TDisassemblerWindow(const TRect &, const char *aTitle,
                      check_func is_bp = NULL);
  void update(unsigned long address);
  virtual TPalette & getPalette() const;
  virtual void sizeLimits(TPoint & min, TPoint & max);
  unsigned long focused_address();

  TDisassemblerViewer *viewer;
  TRegisters *registers;
};

#endif
