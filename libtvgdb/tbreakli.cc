/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TBreakListBox
#include <libtvgdb.h>

#include <librhgdb.h>

#include <string.h>
#include <stdio.h>

TBreakListBox::TBreakListBox(const TRect & r, TScrollBar * s):
TEnterListBox(r, 1, s), delemiter(179)
{
  setRange(BreakPointCount());
}

void
TBreakListBox::update()
{
  setRange(BreakPointCount());
  drawView();
}

void
TBreakListBox::getText(char *dest, ccIndex item, short maxLen)
{
  char buffer[256], *temp;
  char tmp[512];
  BreakPoint *bp = GetBreakPoint(item);

  memset(buffer, ' ', maxLen);
  if (BREAK_ENABLED(bp))
    buffer[0] = 251;
  temp = buffer + 1;
  if (bp->filename)
    memcpy(temp, bp->filename, strlen(bp->filename));
  else
    memset(temp, ' ', filesize);
  temp += filesize;
  *temp++ = delemiter;
  memset(temp, ' ', linesize);
  if (BREAK_FUNCTION(bp))
  {
    memcpy(temp, bp->function, strlen(bp->function));
  }
  else
  {
    sprintf(tmp, "%d", bp->line_number);
    int len = strlen(tmp);

    memcpy(temp + linesize - len, tmp, len);
  }
  temp += linesize;
  *temp++ = delemiter;
  if (BREAK_CONDITION(bp))
    memcpy(temp, bp->condition, strlen(bp->condition));
  else
    memset(temp, ' ', condsize);
  temp += condsize;
  *temp++ = delemiter;
  memset(temp, ' ', countsize);
  if (BREAK_COUNT(bp))
  {
    sprintf(tmp, "%d", bp->count);
    int len = strlen(tmp);

    memcpy(temp + countsize - len, tmp, len);
  }
  temp[countsize] = 0;
  strncpy(dest, buffer, maxLen);
}
