/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include "rhide.h"
#include <rhutils.h>

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

static int _rhide_load_environment_file(char *fname, int unload);

void
rhide_load_environment_file(char *, char *name, int only_current, int unload)
{
  char *files;

  if (only_current)
    files = string_dup(name);
  else
  {
    char *spec = NULL;

    string_cat(spec, "$(strip $(foreach file,$(addsuffix /", name,
               ",$(RHIDE_CONFIG_DIRS)),$(wildcard $(file))))", NULL);
    files = expand_rhide_spec(spec);
    string_free(spec);
  }
  // load the files now in reverse order
  while (*files)
  {
    char *space = files + strlen(files) - 1;

    while ((space > files) && (*space != ' '))
      space--;
    if (space > files)
      _rhide_load_environment_file(space + 1, unload);
    else
      _rhide_load_environment_file(space, unload);
    *space = 0;
  }
  string_free(files);
}

/*
  This reads in an environment file. The syntax of that file
  is very simple. Normally it contains only lines like

VARIABLE=CONTENTS

  which means, that VARIABLE should become the contents
  CONTENTS.
  In addition to this, the file can have some special
  things.
  1) A line, which begins with # is a comment
  2) A line, which begins with the word include, read
     here the file, with the name, which starts after
     the include separated by a space.
     (For compatibility I still support the old dot
      syntax).
     If the line starts with "-include" is treated the
     same way.
  3) A line, which starts with the word export, tells
     RHIDE, to put this variable also in the environment
     with putenv(). (The old ! syntax is still working)
*/

static int
_rhide_load_environment_file(char *fname, int unload)
{
  FILE *f;
  char *line = NULL;
  int line_size = 0;
  char *variable;

  f = fopen(fname, "rt");
  if (!f)
    return 0;
  while (getline(line, line_size, f) != -1)
  {
    if (line[0] == '#')         // comment
      continue;
    if (line[0] == '.')         // include a file
      /*
         This syntax should not longer be used. Use instead
         the "include" keyword like in GNU make 
       */
    {
      char *_fname = expand_spec(line + 1, NULL);

      _rhide_load_environment_file(_fname, unload);
      string_free(_fname);
      continue;
    }
    if (strncmp(line, "-include ", 9) == 0)
      strcpy(line, line+1);
    if (strncmp(line, "include ", 8) == 0)
    {
      char *_fname = expand_spec(line + 8, NULL);

      _rhide_load_environment_file(_fname, unload);
      string_free(_fname);
      continue;
    }
    int _putenv = 0;

    variable = line;
    if (line[0] == '!')
    {
      _putenv = 1;
      variable++;
    }
    if (strncmp(variable, "export ", 7) == 0)
    {
      _putenv = 1;
      variable += 7;
    }
    char *equal = strchr(line, '=');

    if (equal)
    {
      *equal = 0;
      bool add = false;
      if ((equal > line) && (equal[-1] == '+'))
      {
        add = true;
        equal[-1] = 0;
      }
      char *_variable = expand_rhide_spec(variable);
      if (add)
      {
        char *content = string_dup(GetVariable(_variable));
        string_cat(content, " ", equal+1, NULL);
        insert_user_variable(variable, unload ? NULL : content);
        if (strcmp(variable, _variable) != 0)
        {
          insert_user_variable(_variable, unload ? NULL : content);
        }
        string_free(content);
      }
      else
      {
        insert_user_variable(variable, unload ? NULL : equal + 1);
        if (strcmp(variable, _variable) != 0)
        {
          insert_user_variable(_variable, unload ? NULL : equal + 1);
        }
      }
#ifdef __linux__
/* That's now a special case, when running under linux
  using ncurses. There is this variable used, but only
  at startup, which is already done */
      if (strcmp(_variable, "ESCDELAY") == 0)
      {
        extern int ESCDELAY;

        ESCDELAY = atoi(equal + 1);
      }
#endif
      if (_putenv)
      {
        if (!unload)
          *equal = '=';
        /*
           not all systems allocate the memory for the
           variable, so better waste some memory than a
           sigsegv later :-( 
         */
        putenv(string_dup(_variable));
      }
      string_free(_variable);
    }
  }
  fclose(f);
  if (line != NULL)
    free(line);
  return 1;
}

void
push_environment()
{
  _rhide_load_environment_file("rhide.env", 0);
}

void
pop_environment()
{
  _rhide_load_environment_file("rhide.env", 1);
}
