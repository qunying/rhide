/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#include <rhide.h>
#include <rhutils.h>

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

static int _rhide_load_environment_file(char *fname, int unload);

void rhide_load_environment_file(char *,char *name,int only_current,
                                 int unload)
{
  char *files;
  if (only_current)
    files = string_dup(name);
  else
  {
    char *spec = NULL;
    string_cat(spec,"$(strip $(foreach file,$(addsuffix /",name,
                    ",$(RHIDE_CONFIG_DIRS)),$(wildcard $(file))))"
                    ,NULL);
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
      _rhide_load_environment_file(space+1,unload);
    else
      _rhide_load_environment_file(space,unload);
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
  2) A line, which begins with a dot, means, read
     here the file, with the name, which starts after
     the dot.
  3) A line, which begins with ! tells RHIDE, to put
     this variable also in the environment with
     putenv().
*/

static int
_rhide_load_environment_file(char *fname, int unload)
{
  FILE *f;
  char line[2048];
  char *variable;
  f = fopen(fname,"rt");
  if (!f) return 0;
  while (fgets(line,2048,f))
  {
    if (line[0] == '#') // comment
      continue;
    line[strlen(line)-1] = 0; // remove the last '\n'
    if (line[0] == '.') // include a file
    {
      char *_fname = expand_spec(line+1,NULL);
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
    char *equal = strchr(line,'=');
    if (equal && !unload)
    {
      *equal = 0;
      insert_variable(variable,equal+1);
#ifdef __linux__
/* That's now a special case, when running under linux
  using ncurses. There is this variable used, but only
  at startup, which is already done */
      if (strcmp(variable,"ESCDELAY") == 0)
      {
        extern int ESCDELAY;
        ESCDELAY = atoi(equal+1);
      }
#endif
      if (_putenv)
      {
        *equal = '=';
        /* not all systems allocate the memory for the
           variable, so better waste some memory than a
           sigsegv later :-( */
        putenv(string_dup(variable));
      }
    }
    else
    {
      if (equal)
        *equal = 0;
      insert_variable(variable, NULL);
      if (_putenv)
        putenv(string_dup(variable));
    }
  }
  fclose(f);
  return 1;
}


