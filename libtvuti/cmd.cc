/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#include <stdlib.h>

struct const_rec
{
  char *name;
  unsigned short val;
};

static const_rec *commands = NULL;
static unsigned short commands_count = 0;
static const_rec *helps = NULL;
static unsigned short helps_count = 0;

static void
add_const_rec(const_rec * &cmds, unsigned short &count,
              const char *name, unsigned short constant)
{
  count++;
  cmds = (const_rec *) realloc(cmds, count * sizeof(const_rec));
  cmds[count - 1].name = string_dup(name);
  cmds[count - 1].val = constant;
}

void
add_command_constant(const char *name, unsigned short value)
{
  add_const_rec(commands, commands_count, name, value);
}

void
add_help_constant(const char *name, unsigned short value)
{
  add_const_rec(helps, helps_count, name, value);
}
