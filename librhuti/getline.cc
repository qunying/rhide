/* Copyright (C) 2001-2001 Robert H”hne, see COPYING.RHU for details */
#include "rhutils.h"
#include <cstdlib>

#define chunk_size 128

static
void check_bufsize(char *&line, int &buf_count, int needed)
{
  if (needed >= buf_count)
  {
    while (needed >= buf_count)
      buf_count += chunk_size;
    line = (char*)realloc(line, buf_count);
  }
}

int getline(char *&line, int &buf_count, FILE *f)
{
  int chars_read = 0;
  char c;
  if (!line)
  {
    buf_count = chunk_size;
    line = (char*)malloc(buf_count);
  }
  if (feof(f))
  {
    line[0] = 0;
    return -1;
  }
  while (!feof(f))
  {
    if (fread(&c, 1, 1, f) != 1)
      break;
    if (c == '\r')
      continue;
    if (c == '\n')
      break;
    if (c == -1)
      break;
    check_bufsize(line, buf_count, chars_read);
    line[chars_read++] = c;
  }
  check_bufsize(line, buf_count, chars_read);
  line[chars_read] = 0;
  return chars_read;
}

