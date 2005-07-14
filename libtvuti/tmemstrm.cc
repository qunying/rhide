/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TMemoryStream
#include <libtvuti.h>

#include "config.h"
#define Uses_strstream_simple
#include <compatlayer.h>

class rh_sstream : public CLY_strstreambuf
{
public:
  rh_sstream() : CLY_strstreambuf()
  {
  }
  rh_sstream(char *buf, int len) : CLY_strstreambuf(buf, len)
  {
  }
  const void *Buffer()
  {
    return pbase();
  }
};

TMemoryStream::TMemoryStream():
iopstream(new rh_sstream())
{
}

TMemoryStream::TMemoryStream(void *b, int len):
iopstream(new rh_sstream((char*)b, len))
{
}

unsigned long
TMemoryStream::getSize()
{
  return ((rh_sstream *) bp)->pcount();
}

const void *
TMemoryStream::getBuffer()
{
  return ((rh_sstream *) bp)->Buffer();
}
