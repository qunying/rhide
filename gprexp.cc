/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#ifdef __DJGPP__
#include <crt0.h>
#endif

#define Uses_TStringCollection
#define Uses_ofpstream
#include <tv.h>

#define Uses_TProject
#define Uses_TOptions
#define Uses_TDependency
#define Uses_TDepCollection
#include <libide.h>

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

static void WriteGPR(char *outname);


static void
init_gprexp()
{
#ifdef __DJGPP__
  __crt0_load_environment_file("rhide");
#endif
  rhide_load_environment_file("rhide", "rhide.env");
  expand_filenames = ExpandFileNames;
}

static void
ConvertName(TFileName * &_name, TStringCollection * vars)
{
  if (!_name)
    return;
  char *name = NULL;
  const char *Name = FName(_name);

  if (*Name && ((*Name == '/') || (rh_isalpha(*Name) && (Name[1] == ':'))))
  {
    name = string_dup(Name);
    _AbsToRelPath(name, vars, true);
    InitFName(_name, name);
  }
  string_free(name);
}

static void
ConvertDep(TDependency * dep, TStringCollection * vars)
{
  ConvertName(dep->source_name, vars);
  ConvertName(dep->dest_name, vars);
  int i;

  if (dep->dependencies)
  {
    for (i = 0; i < dep->dependencies->getCount(); i++)
    {
      ConvertDep((TDependency *) dep->dependencies->at(i), vars);
    }
  }
}

static TProject *
ConvertProject(TProject * prj, TStringCollection * vars)
{
  TProject *ret = new TProject();

  *ret = *prj;
  *(TDependency *) ret = *(TDependency *) prj;
  ConvertDep(ret, vars);
  ret->pr.pr_VarFilenames = 1;
  return ret;
}

static void
_WriteGPR(int argc, char **argv)
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
          char *outname = string_dup(project_name);

          FExpand(outname);
          fprintf(stdout, _("Converting Projectfile : %s\n"), outname);
          string_free(outname);
          WriteGPR(NULL);
          _WriteGPR(argc, argv);
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
  init_gprexp();
  char *outname = NULL;
  char *pname = NULL;
  int i;
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
    if (strcmp(argv[i], "-r") == 0)
    {
      recursive_make = 1;
    }
    if (strcmp(argv[i], "-r-") == 0)
    {
      recursive_make = 0;
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
  if (!outname)
    outname = string_dup(pname);
  if ((project = ReadProject(project_name, False)) == NULL)
  {
    fprintf(stderr, _("error reading projectfile %s\n"), project_name);
    chdir(orig_dir);
    return -3;
  }
  push_environment();
  WriteGPR(outname);
  _WriteGPR(argc, argv);
  chdir(orig_dir);
  return 0;
}

char *
WUC()
{
  return NULL;
}

static void
WriteGPR(char *outname)
{
  char *name = NULL;
  TStringCollection *vars = new TStringCollection(10, 10);

  if (!outname)
  {
    name = string_dup("__!!!!__.gpr");
  }
  else
  {
    string_dup(name, outname);
  }

  check_vars(vars, Options.SrcDirs);
  check_vars(vars, Options.ObjDirs);
  check_vars(vars, Options.include_path);
  check_vars(vars, Options.library_path);
  TProject *tmp = ConvertProject(project, vars);

  if (strcmp(name, "-") == 0)
    SaveProject(tmp, "");
  else
    SaveProject(tmp, name);
  delete tmp;

  string_free(name);
  destroy(vars);
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

