/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TDirList
#include <libtvuti.h>
#include <string.h>

#include <rhutils.h>

#if !defined( NO_STREAM )
STDSTREAMFUNCS(TDirList, TCollection)
     void TDirList::writeItem(void *item, opstream & os)
{
  os.writeString((char *) item);
}

void *
TDirList::readItem(ipstream & is)
{
  return is.readString();
}
#endif // NO_STREAM

void
TDirList::FromString(const char *liste)
{
  char *tmp, *temp, *_tmp, *__tmp;

  freeAll();
  if (!liste || !*liste)
    return;
  string_dup(temp, liste);
  tmp = temp;
  _tmp = strchr(temp, ';');
  if (!_tmp)
    _tmp = strchr(temp, ' ');
  if (!_tmp)
  {
    insert(temp);
    return;
  }
  do
  {
    __tmp = _tmp;
    *_tmp = 0;
    string_dup(_tmp, tmp);
    insert(_tmp);
    tmp = __tmp + 1;
    _tmp = strchr(tmp, ';');
    if (!_tmp)
      _tmp = strchr(tmp, ' ');
  }
  while (_tmp);
  if (*tmp)
  {
    string_dup(_tmp, tmp);
    insert(_tmp);
  }
  string_free(temp);
}

TDirList::TDirList(const char *liste):
TCollection(10, 1)
{
  FromString(liste);
}

void
TDirList::ToString(char *&result, const char *sep)
{
  int i;

  string_dup(result, "");
  for (i = 0; i < count; i++)
  {
    if (i != 0)
      string_cat(result, sep);
    string_cat(result, (char *) at(i));
  }
}

TDirList & TDirList::operator = (const TDirList & pl)
{
  int
    i;

  freeAll();
  for (i = 0; i < pl.count; i++)
  {
    insert(strdup((char *) pl.items[i]));
  }
  return *this;
}
