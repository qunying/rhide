/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TProject
#define Uses_TOptions
#define Uses_TDependency
#define Uses_TDepCollection
#include <libide.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#ifdef __DJGPP__
#include <crt0.h>
#endif

#include <rhutils.h>
#include <rhide.h>

void
AddToStack()
{
}

void
RemoveFromStack()
{
}

static void
init_gpr2mak()
{
#ifdef __DJGPP__
  __crt0_load_environment_file("rhide");
#endif
  rhide_load_environment_file("rhide", "rhide.env");
  expand_filenames = ExpandFileNames;
}

static void
_WriteMake(int all_deps, int argc, char **argv, bool default_vars)
{
  if (recursive_make && Project.dependencies)
  {
    int i;

    for (i = 0; i < Project.dependencies->getCount(); i++)
    {
      TDependency *_dep = (TDependency *) Project.dependencies->at(i);

      if (_dep->compile_id == COMPILE_PROJECT)
      {
        if (_PushProject(_dep) == True)
        {
          AllDeps = all_deps;
          char *outname = string_dup(project_name);

          BaseName(outname, 0);
          string_cat(outname, ".mak");
          FExpand(outname);
          fprintf(stdout, _("Writing Makefile : %s\n"), outname);
          string_free(outname);
          WriteMake(NULL, argc, argv, default_vars);
          _WriteMake(all_deps, argc, argv, default_vars);
          _PopProject();
        }
      }
    }
  }
}

int
main(int argc, char *argv[])
{
  char *tmp;
  char *orig_dir;
  bool default_vars = false;

  orig_dir = getcwd(NULL, PATH_MAX);
  string_dup(tmp, argv[0]);
  if (!__file_exists(tmp))
  {
    // it was found on PATH
    char *spec = NULL;

    string_cat(spec, "\
$(word 1,$(foreach path,$(subst $(RHIDE_PATH_SEPARATOR), ,$(PATH)),\
$(wildcard $(path)/$(notdir ", tmp, "))))", NULL);
    char *path = expand_rhide_spec(spec);

    string_free(spec);
    if (*path)
    {
      string_free(tmp);
      tmp = path;
    }
    else
      string_free(path);
  }
  FExpand(tmp);
  split_fname(tmp, RHIDE_DIR, RHIDE_NAME, RHIDE_EXT);
  string_free(tmp);
  init_gpr2mak();
  char *outname = NULL;
  char *pname = NULL;
  int i, all_deps = 1;
  char *locale_dir = expand_spec("$(LOCALEDIR)", NULL);

#ifndef __DJGPP__
  if (!*locale_dir)
  {
    string_free(locale_dir);
    // get the system default localedir
    char *_locale_dir = BINDTEXTDOMAIN("rhide", NULL);

    if (_locale_dir)
      locale_dir = string_dup(_locale_dir);
    else
      locale_dir = string_dup("");
  }
#endif
  if (!*locale_dir)
  {
    string_free(locale_dir);
    locale_dir = expand_spec("$(DJDIR)", NULL);
    if (*locale_dir)
    {
      string_cat(locale_dir, "/share/locale");
    }
    else
    {
      string_free(locale_dir);
      locale_dir = string_dup("/usr/local/share/locale");
    }
  }
  setlocale(LC_ALL, "");
  BINDTEXTDOMAIN("rhide", locale_dir);
  TEXTDOMAIN("rhide");
  recursive_make = 0;
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-V") == 0)
    {
      default_vars = true;
    }
    if (strcmp(argv[i], "-V-") == 0)
    {
      default_vars = false;
    }
    if (strcmp(argv[i], "-r") == 0)
    {
      recursive_make = 1;
    }
    if (strcmp(argv[i], "-r-") == 0)
    {
      recursive_make = 0;
    }
    else if (strcmp(argv[i], "-d") == 0)
    {
      all_deps = 1;
    }
    else if (strcmp(argv[i], "-d-") == 0)
    {
      all_deps = 0;
    }
    else if (strcmp(argv[i], "-o") == 0)
    {
      i++;
      if (i >= argc)
      {
        fprintf(stderr, _("-o needs an argument\n"));
        return -1;
      }
      outname = string_dup(argv[i]);
    }
    else if (argv[i][0] != '-')
    {
      pname = string_dup(argv[i]);
      break;
    }
  }
  if (!pname)
  {
    fprintf(stderr, _("a projectfile must be given\n"));
    return -2;
  }
  if (outname)
  {
    if (recursive_make)
    {
      fprintf(stderr, "%s%s%s: %s\n", RHIDE_DIR, RHIDE_NAME, RHIDE_EXT,
              _("-o and -r are not allowed together"));
      return -4;
    }
    if (strcmp(outname, "-") != 0)
      FExpand(outname);
  }
  FExpand(pname);
  {
    char *ext, *pdir;

    split_fname(pname, pdir, project_name, ext);
    string_cat(project_name, ext);
    string_free(ext);
    chdir(pdir);
    string_free(pdir);
  }
  if ((project = ReadProject(project_name, false)) == NULL)
  {
    fprintf(stderr, _("error reading projectfile %s\n"), project_name);
    chdir(orig_dir);
    return -3;
  }
  push_environment();
  AllDeps = all_deps;
  WriteMake(outname, argc, argv, default_vars);
  _WriteMake(all_deps, argc, argv, default_vars);
  chdir(orig_dir);
  return 0;
}

char *
WUC()
{
  return NULL;
}

/* The following functions are provided here only to make the
   linker happy, since they are refernced by libset */
void
show_message(const char *, const char *, int, int, int)
{
}

int
RunProgram(const char *, bool, bool, bool)
{
  return -1;
}
