/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TFileName ) && !defined( __TFileName__ )
#define __TFileName__

class TFileName
{
public:
  TFileName(const char *name);
    virtual ~ TFileName();
  const char *const Name()
  {
    return NAME;
  }
protected:
  char *NAME;
};

#endif
