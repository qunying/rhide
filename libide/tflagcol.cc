/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TFlagCollection
#define Uses_TFlagEntry
#define Uses_TStreamable
#include <libide.h>

#include <rhutils.h>

void *
TFlagCollection::readItem(ipstream & is)
{
  TFlagEntry *flag;

  is >> flag;
  return flag;
}

void
TFlagCollection::writeItem(void *flag, opstream & os)
{
  os << ((TFlagEntry *) flag);
}

TStreamable *
TFlagCollection::build()
{
  return new TFlagCollection(streamableInit);
}

void
TFlagCollection::ToString(char *&retval, const char *sep)
{
  int i;

  retval = NULL;
  for (i = 0; i < count; i++)
  {
    char *flag;

    flag = ((TFlagEntry *) at(i))->GetValue();
    if (flag)
    {
      if (retval)
        string_cat(retval, sep);
      string_cat(retval, flag);
      string_free(flag);
    }
  }
}

TFlagCollection & TFlagCollection::operator = (const TFlagCollection & fc)
{
  int
    i;

  freeAll();
  for (i = 0; i < fc.count; i++)
  {
    insert(new TFlagEntry((TFlagEntry *) fc.items[i]));
  }
  return *this;
}

void
TFlagCollection::freeItem(void *item)
{
  destroy((TFlagEntry *) item);
}
