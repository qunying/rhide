/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TMemoryStream
#include <libtvuti.h>

#include <strstream.h>

class _S : public strstreambuf
{
public:
  _S() : strstreambuf() {}
  _S(const void *b, int len) : strstreambuf((const char *)b, len) {}
  void * Buffer() { return base(); }
};

TMemoryStream::TMemoryStream() :
    iopstream(new _S())
{
}

TMemoryStream::TMemoryStream(const void *b, int len) :
    iopstream(new _S(b, len))
{
}

unsigned long TMemoryStream::getSize()
{
  return ((_S *)bp)->pcount();
}

void *TMemoryStream::getBuffer()
{
  return ((_S *)bp)->Buffer();
}


