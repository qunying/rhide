/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TMemoryStream
#include <libtvuti.h>

#include "config.h"
#ifdef HAVE_STREAMBUF
#include <streambuf>
#else
#include <streambuf.h>
#endif

class rh_sstream : public std::streambuf
{
public:
  rh_sstream() : std::streambuf()
  {
  }
  const void *Buffer()
  {
    return eback();
  }
  void set_buf(void *buf, int size)
  {
    setbuf((char *)buf, size);
  }
};

TMemoryStream::TMemoryStream():
iopstream(new rh_sstream())
{
}

TMemoryStream::TMemoryStream(void *b, int len):
iopstream(new rh_sstream())
{
  ((rh_sstream *) bp)->set_buf(b, len);
}

unsigned long
TMemoryStream::getSize()
{
  return ((rh_sstream *) bp)->in_avail();
}

const void *
TMemoryStream::getBuffer()
{
  return ((rh_sstream *) bp)->Buffer();
}
