/* Copyright (C) 1996-2003 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include "rhide.h"

#define Uses_TDependency
#include <libide.h>

#define Uses_TMsgCollection
#define Uses_MsgRec
#include <libtvuti.h>

#include <rhutils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/*
  What should the external error checking program do:
  1) The program must take a filename as argument, which contain the
     output to stderr from the compiler (Error and warning messages)
  2) The program have to write to stdout the Warnings, Errors or
     messages in a special format.
     Errors:
       A line, which starts with the string "!Error" (without the
       double quotes) optionally follwed by "!File" and a filename
       in single quotes and "!Line" and a line number and
       !Column" and a column number
       The next line contain the error text.
       Example:
!Error!File'test.c'!Line17!Column18
This is a sample error message
       This means, that in file 'test.c' at line 17 column 18 has an error
       occoured with the description
         'This is a sample error message'
  3) Warnings have the same format, except that !Error is replaced by
     !Warning.
  4) Any other line is treated as a simple message optionally preceeded
     by !File and !Line.
*/

static char *buffer = NULL;
static int bufsize = 0;
static int bufpos;
static int line_no;
static int column_no;
static int in_error;
static int in_warning;
static char *fname = NULL;

static char *
ScanBuffer()
{
  char *tmp = buffer;

  string_free(fname);
  while (*tmp && *tmp == '!')
  {
    if (strncmp(tmp, "!Error", 6) == 0)
    {
      in_error = 1;
      tmp += 6;
      continue;
    }
    if (strncmp(tmp, "!Warning", 8) == 0)
    {
      in_warning = 1;
      tmp += 8;
      continue;
    }
    if (strncmp(tmp, "!File", 5) == 0)
    {
      tmp += 5;
      if (*tmp != '\'')
        continue;
      tmp++;
      char *end = strchr(tmp, '\'');

      if (!end)
        continue;
      *end = 0;
      fname = string_dup(tmp);
      tmp = end + 1;
      continue;
    }
    if (strncmp(tmp, "!Line", 5) == 0)
    {
      tmp += 6;
      char *end = tmp, c;

      while (rh_isdigit(*end))
        end++;
      if (end == tmp)
        continue;
      c = *end;
      *end = 0;
      sscanf(tmp, "%d", &line_no);
      *end = c;
      tmp = end;
      continue;
    }
    if (strncmp(tmp, "!Column", 7) == 0)
    {
      tmp += 8;
      char *end = tmp, c;

      while (rh_isdigit(*end))
        end++;
      if (end == tmp)
        continue;
      c = *end;
      *end = 0;
      sscanf(tmp, "%d", &column_no);
      *end = c;
      tmp = end;
      continue;
    }
    return tmp;
  }
  return tmp;
}

Boolean
user_check_errors(TDependency * dep, TMsgCollection & args)
{
  FILE *inf;
  char *tmp;
  int c;
  Boolean retval = True;

  string_dup(tmp, dep->error_check);
  string_cat(tmp, " ");
  string_cat(tmp, cpp_errname);
  inf = popen(tmp, "r");
  if (!inf)
  {
    string_dup(tmp, _("An error has occoured when running the program "));
    string_cat(tmp, dep->error_check);
    args.insert(new MsgRec(NULL, tmp));
    string_free(tmp);
    string_dup(tmp, _("The error was : "));
    string_cat(tmp, strerror(errno));
    args.insert(new MsgRec(NULL, tmp));
    string_free(tmp);
    return False;
  }
  while (!feof(inf))
  {
    bufpos = 0;
    do
    {
      c = fgetc(inf);
      if (c != EOF && c != '\n')
      {
        if (bufpos >= bufsize)
        {
          bufsize += 1024;
          buffer = (char *) realloc(buffer, bufsize);
        }
        buffer[bufpos++] = c;
      }
      else
        break;
    }
    while (1);
    if (bufpos >= bufsize)
    {
      bufsize += 1024;
      buffer = (char *) realloc(buffer, bufsize);
    }
    buffer[bufpos] = 0;
    if (in_error)
    {
      args.insert(new MsgRec(fname, buffer, msgError, line_no, column_no));
      in_error = 0;
    }
    else if (in_warning)
    {
      args.insert(new MsgRec(fname, buffer, msgWarning, line_no, column_no));
      in_error = 0;
    }
    else
    {
      line_no = 0;
      column_no = 0;
      char *msg = ScanBuffer();

      if (!in_error && !in_warning)
        args.insert(new MsgRec(fname, msg, msgMessage, line_no, column_no));
      if (in_error)
        retval = False;
    }
  }
  pclose(inf);
  return retval;
}
