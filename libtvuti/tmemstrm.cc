/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TMemoryStream
#include <libtvuti.h>

#include "config.h"
#include <strstream>

#define strbase std::strstreambuf

class rh_sstream : public strbase
{
public:
  rh_sstream() : strbase()
  {
  }
  rh_sstream(char *buf, int len) : strbase(buf, len)
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
