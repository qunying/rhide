/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TMemoriStream )
#define __TMemoryStream

class TMemoryStream:public iopstream
{
public:
  TMemoryStream();
  TMemoryStream(void *, int);
  unsigned long getSize();
  const void *getBuffer();
};

#endif
