/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TProjectWindow ) && !defined( __TProjectWindow__ )
#define __TProjectWindow__

class TProjectListBox;
class TDepCollection;

class TProjectWindow:public TDialog
{
public:
  TProjectListBox * liste;
  TProjectWindow(const TRect &, TDepCollection *, const char *);
};

#endif
