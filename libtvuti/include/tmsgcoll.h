/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TMsgCollection ) && !defined( __TMsgCollection__ )
#define __TMsgCollection__

class TMsgCollection:public TNSCollection
{
public:
  TMsgCollection(void):TNSCollection(5, 5)
  {
  }
  virtual void freeItem(void *item);
};

#endif
