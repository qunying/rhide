/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdbrh.h>
#include <librhgdb.h>
#include <rhgdbint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __DJGPP__
#include <dpmi.h>
#endif

int win31 = 0;

#define BREAK win31?"hbreak":"break"

static int hw_break_point_count = 0;

static int
break_available()
{
#ifdef __DJGPP__
  if (!win31)
    return 1;
  if (hw_break_point_count < 3)
    return 1;
  return 0;
#else
  return 1;
#endif
}

char *
Bname(char *name)
{
  char *tmp;

  tmp = strrchr(name, '/');
  if (!tmp)
    tmp = strrchr(name, '\\');
  if (!tmp)
    return name;
  return tmp + 1;
}

static void SetBreakPoint(int);

struct BreakPoint *BreakPoints = NULL;
int BreakCount = 0;
static char command[1024];

void
DisableBreakPoint(int index)
{
  struct BreakPoint *bp;

  if (index > BreakCount)
    return;
  bp = BreakPoints + index;
  if (bp->number > 0)
  {
    sprintf(command, "disable %d", bp->number);
    hw_break_point_count--;
    Command(command, 0);
  }
  bp->type &= ~B_ENABLED;
}

void
EnableBreakPoint(int index)
{
  struct BreakPoint *bp;

  if (index > BreakCount)
    return;
  bp = BreakPoints + index;
  if (bp->number > 0)
  {
    if (!break_available())
    {
      _UserWarning(WARN_NOT_ENABLED, bp);
      return;
    }
    hw_break_point_count++;
    sprintf(command, "enable %d", bp->number);
    Command(command, 0);
  }
  bp->type |= B_ENABLED;
}

void
RemoveBreakPoint(int index)
{
  int i;
  struct BreakPoint *bp;

  if (index > BreakCount)
    return;
  bp = BreakPoints + index;
  if (bp->number > 0)
  {
    hw_break_point_count--;
    sprintf(command, "delete %d", bp->number);
    Command(command, 0);
  }
  if (bp->filename)
    free(bp->filename);
  if (bp->function)
    free(bp->function);
  if (bp->condition)
    free(bp->condition);
  for (i = index; i < BreakCount - 1; i++)
  {
    BreakPoints[i] = BreakPoints[i + 1];
  }
  BreakCount--;
  BreakPoints = (struct BreakPoint *) realloc(BreakPoints,

                                              BreakCount *
                                              sizeof(struct BreakPoint));
}

static void
AddBreakPoint(struct BreakPoint *bp)
{
  struct BreakPoint *_bp;

  BreakCount++;
  BreakPoints = (struct BreakPoint *) realloc(BreakPoints,

                                              BreakCount *
                                              sizeof(struct BreakPoint));
  _bp = BreakPoints + (BreakCount - 1);
  _bp->type = bp->type;
  _bp->address = bp->address;
  _bp->count = bp->count;
  _bp->line_number = bp->line_number;
  if (bp->filename)
    _bp->filename = strdup(bp->filename);
  else
    _bp->filename = NULL;
  if (bp->function)
    _bp->function = strdup(bp->function);
  else
    _bp->function = NULL;
  if (bp->condition)
    _bp->condition = strdup(bp->condition);
  else
    _bp->condition = NULL;
  _bp->number = -1;
  if (_progname != NULL)
    SetBreakPoint(BreakCount - 1);
}

static int
SetLineBreakPoint(struct BreakPoint *bp)
{
  int no_found = 1;

  if (!break_available())
  {
    bp->number = -1;
    _UserWarning(WARN_NO_BREAK_AVAILABLE, bp);
    return 0;
  }
  sprintf(command, "%s %s:%d", BREAK, Bname(bp->filename), bp->line_number);
  Command(command, 0);
  if (invalid_line || (no_found = strncmp(gdb_output_buffer, "No ", 3)) == 0)
  {
    bp->number = -1;
    _UserWarning(WARN_INVALID_BREAK, bp);
    if (no_found == 0)
      return 0;
    sprintf(command, "delete %d", last_breakpoint_number);
    Command(command, 0);
    return 0;
  }
  hw_break_point_count++;
  bp->number = last_breakpoint_number;
  bp->address = last_breakpoint_address;
  return 1;
}

static int
SetFunctionBreakPoint(struct BreakPoint *bp)
{
  int no_found = 1;

  if (!break_available())
  {
    bp->number = -1;
    _UserWarning(WARN_NO_BREAK_AVAILABLE, bp);
    return 0;
  }
  strcpy(command, BREAK);
  strcat(command, " ");
  if (bp->filename)
  {
    strcat(command, Bname(bp->filename));
    strcat(command, ":");
  }
  strcat(command, bp->function);
  Command(command, 0);
  if (invalid_line || (no_found = strncmp(gdb_output_buffer, "No ", 3)) == 0)
  {
    bp->number = -1;
    _UserWarning(WARN_INVALID_BREAK, bp);
    if (no_found == 0)
      return 0;
    sprintf(command, "delete %d", last_breakpoint_number);
    Command(command, 0);
    return 0;
  }
  hw_break_point_count++;
  if (last_breakpoint_file)
  {
    if (!bp->filename || strcmp(bp->filename, last_breakpoint_file))
    {
      if (bp->filename)
        free(bp->filename);
      bp->filename = strdup(last_breakpoint_file);
    }
    bp->line_number = last_breakpoint_line;
  }
  bp->number = last_breakpoint_number;
  bp->address = last_breakpoint_address;
  return 1;
}

static int
SetCondition(struct BreakPoint *bp)
{
  sprintf(command, "condition %d %s", bp->number, bp->condition);
  Command(command, 0);
  return 1;
}

static int
SetCount(struct BreakPoint *bp)
{
  sprintf(command, "ignore %d %d", bp->number, bp->count);
  Command(command, 0);
  return 1;
}

static void
SetBreakPoint(int index)
{
  struct BreakPoint *bp;
  int result = 1;

  if (index > BreakCount)
    return;
  bp = BreakPoints + index;
  if (bp->number > 0)
    return;                     /*
                                   already set 
                                 */
  if (BREAK_FUNCTION(bp))
    result = SetFunctionBreakPoint(bp);
  if (!result)
    return;
  if (BREAK_LINE(bp))
    result = SetLineBreakPoint(bp);
  if (!result)
    return;
  if (BREAK_CONDITION(bp))
    result = SetCondition(bp);
  if (!result)
    return;
  if (BREAK_COUNT(bp))
    result = SetCount(bp);
  if (!result)
    return;
  if (BREAK_DISABLED(bp))
    DisableBreakPoint(index);
}

void
SetBreakPoints()
{
  int i;

  for (i = 0; i < BreakCount; i++)
  {
    SetBreakPoint(i);
  }
}

void
DeleteBreakPoints()
{
  int i;

#if 0                           /*
                                   I don't know why, but this causes a crash, when the program
                                   exits with an exit code > 63 
                                 */
  Command("delete", 0);
#endif
  hw_break_point_count = 0;
  for (i = 0; i < BreakCount; i++)
  {
    sprintf(command, "delete %d", BreakPoints[i].number);
    Command(command, 0);
    BreakPoints[i].number = -1;
  }
}

int
AddBreakPointLine(char *fname, int line)
{
  struct BreakPoint bp;

  bp.type = B_ENABLED | B_LINE;
  bp.count = 0;
  bp.condition = NULL;
  bp.address = 0;
  bp.filename = fname;
  bp.line_number = line;
  bp.function = NULL;
  bp.number = -1;
  AddBreakPoint(&bp);
  return BreakCount - 1;
}

int
IsBreakPointLine(char *fname, int line)
{
  int i;
  struct BreakPoint *bp;

  for (i = 0, bp = BreakPoints; i < BreakCount; i++, bp++)
  {
    if (bp->line_number == line &&
        (strcmp(Bname(bp->filename), fname) == 0 ||
         strcmp(bp->filename, fname) == 0))
      return i;
  }
  return -1;
}

int
BreakPointCount()
{
  return BreakCount;
}

struct BreakPoint *
GetBreakPoint(int index)
{
  return BreakPoints + index;
}

void
EditBreakPoint(struct BreakPoint *bp, int index)
{
  if (index > BreakCount)
    return;
  if (bp->count > 0)
    bp->type |= B_COUNT;
  else
    bp->type &= ~B_COUNT;
  if (bp->function)
  {
    bp->type |= B_FUNCTION;
    bp->type &= ~B_LINE;
  }
  else
  {
    bp->type &= ~B_FUNCTION;
    bp->type |= B_LINE;
  }
  if (bp->condition)
  {
    bp->type |= B_CONDITION;
  }
  else
  {
    bp->type &= ~B_CONDITION;
  }
  if (index < 0)
  {
    AddBreakPoint(bp);
    return;
  }
  BreakPoints[index] = *bp;
  if (bp->number > 0)
  {
    sprintf(command, "delete %d", bp->number);
    Command(command, 0);
    bp->number = -1;
  }
  if (init_count > 0)
    SetBreakPoint(index);
}
