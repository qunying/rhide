/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TWindowList )
#define __TWindowList

class TWindow;
class TSCollection;

typedef struct
{
  TWindow *window;
  TWindow **ref;
  char *full_name;
  char *base_name;
  dev_t dev;
  ino_t inode;
  Boolean remember_closed;
  Boolean is_removable;
}
DeskTopWindow;

struct closed_window
{
  char *name;
  unsigned long size;
  void *saved_window;
  Boolean is_removable;
};

class TWindowList:public TEnterListBox
{
public:
  TWindowList(const TRect & bounds, ushort aNumCols, TScrollBar * aScrollBar,
              TSCollection * _open_windows, TSCollection * _closed_windows);
  virtual void handleEvent(TEvent & event);
  virtual void getText(char *, ccIndex, short);
  virtual void selectItem(ccIndex item);
  void update(TSCollection * _open_windows, TSCollection * _closed_windows);

  TSCollection *ow, *cw;
  ccIndex open_count, closed_count;
};

#endif

