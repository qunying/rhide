/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <librhgdb.h>
#include <rhgdbint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char command[1024];

void
annotate_field (num)
     int num;
{
_DEBUG("a_field(%d)\n",num);
}

void
annotate_field_begin (type)
     struct type *type;
{
_DEBUG("a_field_begin\n");
}

void
annotate_field_name_end ()
{
_DEBUG("a_field_name_end\n");
}

void
annotate_field_value ()
{
_DEBUG("a_field_value\n");
}

void
annotate_field_end ()
{
_DEBUG("a_field_end\n");
}

void
annotate_value_history_begin (histindex, type)
     int histindex;
     struct type *type;
{
_DEBUG("a_value_history_begin(%d)\n",histindex);
}

void
annotate_value_begin (type)
     struct type *type;
{
_DEBUG("a_value_begin\n");
}

void
annotate_value_history_value ()
{
_DEBUG("a_value_history_value\n");
}

void
annotate_value_history_end ()
{
_DEBUG("a_value_history_end\n");
}

void
annotate_value_end ()
{
_DEBUG("a_value_end\n");
}

char *EvaluateWatch(char *watch)
{
  char *ret,*end;
#if 1
  if (!debugger_started) return NULL;
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
  ret = EvaluateWatch(var);
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
  return EvaluateWatch(var);
}


