/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdbrh.h>
#include <librhgdb.h>
#include <rhgdbint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char command[1024];

char *EvaluateWatch(const char *watch, int force)
{
  char *ret,*end;
#if 1
  if (!debugger_started && !force) return NULL;
#endif
  sprintf(command,"print %s",watch);
  /* Do not call the select_source_line. This can happen, when
     watch contains a function call */
  reset_command++;
  Command(command,0);
  reset_command--;
  ret = gdb_output_buffer;
  if (*ret != '$') return NULL;
  while (*ret && *ret != '=') ret++;
  if (!*ret) return NULL;
  ret++;
  while (*ret && *ret == ' ') ret++;
  if (!*ret) return NULL;
  end = ret + strlen(ret) - 1;
  if (*end == '\n') *end = 0;
  return ret;
}

char *SetValue(char *var,char *expr)
{
  char *tmp,*ret;
  ret = EvaluateWatch(var, 0);
  if (!ret) return NULL;
  tmp = strdup(ret);
  sprintf(command,"set variable %s=%s",var,expr);
  reset_command++;
  Command(command,0);
  reset_command--;
#if 0
  if (gdb_error)
  {
    ret = strdup(error_start);
    sprintf(command,"set %s=%s",var,tmp);
    reset_command++;
    Command(command,0);
    reset_command--;
    strcpy(command,ret);
    free(ret);
    free(tmp);
    return command;
  }
#endif
  free(tmp);
  return EvaluateWatch(var, 0);
}


