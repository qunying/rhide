/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TDataWindow )
#define __TDataWindow

class TRect;
class TEvent;
class TDataViewer;
class TPalette;

class TDataWindow:public TDialog
{
  TDataViewer *viewer;
  TDataWindow *next;
  static TDataWindow *windowlist;

public:
    TDataWindow(const TRect &, const char *aTitle);
   ~TDataWindow();
  virtual TPalette & getPalette() const;
  virtual void sizeLimits(TPoint & min, TPoint & max);

  static TDataWindow *createNew(const char *naddr = NULL);
  static TDataWindow *stackWindow();
  static void updateAll();
};

/* Force update all TDataWindow windows  */
void UpdateDataWindows(void);

#endif
