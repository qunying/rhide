/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdb.h>
#include <librhgdb.h>


int register_count()
{
  return NUM_REGS-NUM_FREGS;
}

static char *register_names[] = REGISTER_NAMES;

const char *register_name(int num)
{
  return register_names[num];
}

unsigned long get_register_value(int num)
{
  if (!debugger_started)
    return 0;
  return read_register(num);
}

void set_register_value(int num, unsigned long value)
{
  if (!debugger_started)
    return;
  write_register(num, value);
}

int get_register_size(int num)
{
  return REGISTER_RAW_SIZE(num);
}


