/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TParamList
#define Uses_TStreamable
#include <libtvuti.h>
#include <string.h>
#include <stdio.h>

#include <rhutils.h>

#if !defined( NO_STREAM )
STDSTREAMFUNCS(TParamList, TCollection)
     void TParamList::writeItem(void *item, opstream & os)
{
  os.writeString((char *) item);
}

void *
TParamList::readItem(ipstream & is)
{
  return is.readString();
}

#endif // NO_STREAM

Boolean
TParamList::FromString(const char *liste)
{
  char quote_char = '"';
  int in_quote = 0;
  char *start, *end;
  char *tmp, *temp, c;

  freeAll();
  if (!liste || !*liste)
    return True;
  string_dup(temp, liste);
  start = temp;
  while (*start)
  {
    while (*start == ' ')
      start++;
    if (!*start)
      return True;
    end = start;
    while (1)
    {
      if (*end == '\\' && (end[1] == '\'' || end[1] == '"'))
      {
        end += 2;
        continue;
      }
      if (in_quote)
      {
        if (quote_char == *end)
        {
          in_quote = 0;
          strcpy(end, end + 1);
        }
        else
          end++;
        continue;
      }
      if (*end == '\'' || *end == '"')
      {
        in_quote = 1;
        quote_char = *end;
        strcpy(end, end + 1);
        continue;
      }
      if (!*end || *end == ' ')
        break;
      end++;
    }
    c = *end;
    *end = 0;
    string_dup(tmp, start);
    insert(tmp);
    *end = c;
    while (*end && *end == ' ')
      end++;
    start = end;
  }
  string_free(temp);
  if (in_quote)
    return False;
  return True;
}

TParamList::TParamList(const char *liste):TCollection(10, 1)
{
  FromString(liste);
}

void
TParamList::ToString(char *&result)
{
  int i, need_quote;
  char *tmp;

  result = NULL;
  for (i = 0; i < count; i++)
  {
    if (result)
      string_cat(result, " ");
    tmp = (char *) at(i);
    if (strchr(tmp, ' ') != 0)
      need_quote = 1;
    else
      need_quote = 0;
    if (need_quote)
    {
      string_cat(result, "\"");
      string_cat(result, tmp);
      string_cat(result, "\"");
    }
    else
    {
      string_cat(result, tmp);
    }
  }
  if (result == NULL)
    string_dup(result, "");
}

TParamList & TParamList::operator = (const TParamList & pl)
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
