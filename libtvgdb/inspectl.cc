/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <stdlib.h>
#include <string.h>

#define Uses_TInspectList
#include <libtvgdb.h>

#define Uses_TSCollection
#include <libtvuti.h>

#include <librhgdb.h>
#include <rhutils.h>

TInspectList::TInspectList(const TRect & bounds):
TListBox(bounds, 1, NULL)
{
}

static char *
left_adjust(int left_space, const char *val)
{
  int len = strlen(val);
  char *ret = (char *) malloc(left_space + len + 1);

  memset(ret, ' ', left_space);
  memcpy(ret + left_space, val, len + 1);
  return ret;
}

void
TInspectList::update(const char *expr)
{
  newList(NULL);
  char *res = string_dup(EvaluateWatch(expr, 0));

  if (*res)
  {
    int left = 0;
    char *start = res;
    char *end = start;
    int in_string1 = 0, in_string2 = 0, in_brace = 0;
    TSCollection *coll = new TSCollection();

    while (*end)
    {
      switch (*end)
      {
        default:
          break;
        case '\\':
          end++;
          break;
        case '"':
          in_string1 ^= 1;
          break;
        case '\'':
          in_string2 ^= 1;
          break;
        case '(':
          in_brace++;
          break;
        case ')':
          in_brace--;
          break;
        case '}':
          if (!in_brace && !in_string1 && !in_string2)
          {
            char c = *end;

            *end = 0;
            char *val = left_adjust(left, start);

            coll->insert(val);
            *end = c;
            left--;
            start = end;
          }
          break;
        case ',':
        case '{':
          if (!in_brace && !in_string1 && !in_string2)
          {
            char c = end[1];

            end[1] = 0;
            char *val = left_adjust(left, start);

            coll->insert(val);
            end[1] = c;
            if (*end == '{')
              left++;
            end++;
            while (*end == ' ')
              end++;
            end--;
            start = end + 1;
          }
          break;
      }
      end++;
    }
    if (*start)
      coll->insert(string_dup(start));
    newList(coll);
  }
}
