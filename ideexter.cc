/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TParamList
#define Uses_TProject
#define Uses_TOptions
#define Uses_TDirList
#define Uses_MsgBox
#include <libide.h>

#define Uses_TSCollection
#define Uses_TMsgCollection
#define Uses_MsgRec
#include <libtvuti.h>

#include <rhutils.h>

#include "rhide.h"
#include "rhidehis.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void
FSDB()
{
  char *cmd;

  if (DEBUGGER_STARTED())
    RESET();
  CLEARSYMBOLS();
  cmd = BuildCompiler(project, "$(RHIDE_FSDB)");
  RunProgram(cmd, False, False, True);
  string_free(cmd);
}

void
GDB()
{
  char *cmd;

  if (DEBUGGER_STARTED())
    RESET();
  CLEARSYMBOLS();
  cmd = BuildCompiler(project, "$(RHIDE_GDB)");
  RunProgram(cmd, False, False, True);
  string_free(cmd);
}

static char *_read_buffer = NULL;
static int _read_buffer_size = 0;
static int _read_buffer_len = 0;
static int _read_buffer_char;

#define RESIZE() \
do {\
  if (_read_buffer_len >= _read_buffer_size)\
  {\
    _read_buffer_size += 1024;\
    _read_buffer = (char *)realloc(_read_buffer,_read_buffer_size);\
  }\
} while (0)


static char *
GetLine(FILE * f)
{
  _read_buffer_len = 0;
  while ((_read_buffer_char = fgetc(f)) != EOF)
  {
    RESIZE();
    if (_read_buffer_char == '\n')
    {
      _read_buffer[_read_buffer_len] = 0;
      return _read_buffer;
    }
    if (_read_buffer_char)
      _read_buffer[_read_buffer_len++] = _read_buffer_char;
    else
    {
      _read_buffer[_read_buffer_len++] = '\\';
      RESIZE();
      _read_buffer[_read_buffer_len++] = '0';
      RESIZE();
      _read_buffer[_read_buffer_len++] = '0';
      RESIZE();
      _read_buffer[_read_buffer_len++] = '0';
    }
  }
  if (_read_buffer_len == 0)
    return NULL;
  RESIZE();
  _read_buffer[_read_buffer_len] = 0;
  return _read_buffer;
}

static void
CheckGrepMsg(const char *fname, const char *errname, TMsgCollection * msgs)
{
  char *buffer;
  int lineno;
  char *line, *message;

  msgs->insert(new MsgRec(NULL, _("ready")));
  FILE *f = fopen(fname, "r");

  while ((buffer = GetLine(f)) != NULL)
  {
    line = strchr(buffer, ':');
    if (!line)
    {
      msgs->insert(new MsgRec(NULL, buffer));
      continue;
    }
    if (!rh_isdigit(line[1]))
      line = strchr(line + 1, ':');
    if (!line)
    {
      msgs->insert(new MsgRec(NULL, buffer));
      continue;
    }
    *line = 0;
    line++;
    message = strchr(line, ':');
    if (!message)
    {
      line[-1] = ':';
      msgs->insert(new MsgRec(NULL, buffer));
      continue;
    }
    *message = 0;
    message++;
    sscanf(line, "%d", &lineno);
    msgs->insert(new MsgRec(buffer, message, msgMessage, lineno));
  }
  fclose(f);
  f = fopen(errname, "r");
  while ((buffer = GetLine(f)) != NULL)
  {
    msgs->insert(new MsgRec(NULL, buffer));
  }
  fclose(f);
}

void
GREP()
{
  char *spec, *cmd;
  TMsgCollection *msgs;

#if 0
  TParamList *params;

  params = new TParamList();
  if (EditParamList(params, _("arguments for GREP"),
                    RHIDE_History_prompt) != cmOK)
  {
    destroy(params);
    return;
  }
  if (params->getCount() < 2)
  {
    if (messageBox(mfWarning | mfYesButton | mfNoButton,
                   _("There are only %d parameters for GREP. Continue ?"),
                   params->getCount()) != cmYes)
    {
      destroy(params);
      return;
    }
  }
  string_dup(spec, "grep -n ");
  params->ToString(cmd);
  string_cat(spec, cmd);
  string_free(cmd);
  destroy(params);
#else
  string_dup(spec, "$(RHIDE_GREP)");
#endif
  msgs = new TMsgCollection();

  /*
     A message, that GREP is running 
   */
  msgs->insert(new MsgRec(NULL, _("running GREP:")));
  cmd = BuildCompiler(project, spec);
  msgs->insert(new MsgRec(NULL, cmd));
  ShowMessages(msgs, True);
  string_free(spec);
  /*
     This is now a little trick to force grep to print out
     the filename. If grep is called with only one filename
     it prints only the linennumber 
   */
  string_cat(cmd, " /dev/null");	// /dev/null is now also nul on MS-DOS
  string_cat(cmd, " /dev/null");	// when the user has specified no arguments
  RunProgram(cmd, True, True, False);
  string_free(cmd);
  msgs = new TMsgCollection();
  CheckGrepMsg(cpp_outname, cpp_errname, msgs);
  if (!debug_tempfiles)
  {
    remove(cpp_outname);
    remove(cpp_errname);
  }
  ShowMessages(msgs, False);
}

void
GPROF()
{
  char *cmd = BuildCompiler(project, "$(RHIDE_GPROF)");

  RunProgram(cmd, False, True, True);
  string_free(cmd);
}

void
RunExternalProgram(const char *program, int show_stderr, int show_stdout)
{
  TMsgCollection *msgs = NULL;
  char *tmp = string_dup(_("executing: "));

  string_cat(tmp, program);
  show_message(tmp, NULL, 0, 0, 2);
  RunProgram(program, True, True, False);
  if (show_stderr)
  {
    if (!msgs)
      msgs = new TMsgCollection();
    msgs->insert(new MsgRec(NULL, _("Output to stderr:")));
    char *buffer;
    FILE *f;

    f = fopen(cpp_errname, "r");
    while ((buffer = GetLine(f)) != NULL)
    {
      msgs->insert(new MsgRec(NULL, buffer));
    }
    fclose(f);
  }
  if (show_stdout)
  {
    if (!msgs)
      msgs = new TMsgCollection();
    msgs->insert(new MsgRec(NULL, _("Output to stdout:")));
    char *buffer;
    FILE *f;

    f = fopen(cpp_outname, "r");
    while ((buffer = GetLine(f)) != NULL)
    {
      msgs->insert(new MsgRec(NULL, buffer));
    }
    fclose(f);
  }
  if (!debug_tempfiles)
  {
    remove(cpp_outname);
    remove(cpp_errname);
  }
  if (msgs)
    ShowMessages(msgs, False);
}

