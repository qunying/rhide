/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TRangeValidator

#define Uses_TIntInputLine
#include <libtvuti.h>

#include <limits.h>
#include <string.h>
#include <stdio.h>

TIntInputLine::TIntInputLine(const TRect & bounds, int aMaxlen,
                             int _unsigned):
TEnterInputLine(bounds, aMaxlen)
{
  if (_unsigned)
    SetValidator(new TRangeValidator(0, UINT_MAX));
  else
    SetValidator(new TRangeValidator(INT_MIN, INT_MAX));
}

void
TIntInputLine::setData(void *rec)
{
  int i = *((int *) rec);
  char tmp[256];

  sprintf(tmp, "%d", i);
  strncpy(data, tmp, maxLen - 1);
  data[maxLen - 1] = 0;
}

void
TIntInputLine::getData(void *rec)
{
  sscanf(data, "%d", (int *) rec);
}

uint32
TIntInputLine::dataSize()
{
  return sizeof(int);
}
