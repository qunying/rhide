/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_ifpstream
#define Uses_ofpstream
#define Uses_MsgBox
#define Uses_TProject
#define Uses_TDepCollection
#define Uses_TOptions
#define Uses_TDirList
#define Uses_TParamList
#define Uses_TStringCollection
#define Uses_ideFunctions

#include <libide.h>

#define Uses_TSCollection
#include <libtvuti.h>

#include <rhutils.h>

#include <edprint.h>

#include "rhide.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

TProject *project;
char *project_directory = NULL;
char *project_name = NULL;
int recursive_make = 0;

unsigned short ProjectVersion = 1;

TProject *
ReadProject(const char *prjname, bool from_ide)
{
  ifpstream *ifile;
  char *magic;
  link_strm = 1;
  unsigned short version;
  TProject *_project;

  ifile = open_ifpstream(prjname);
  if (ifile && ifile->good())
  {
    magic = ifile->readString();
    if (!ifile->good() ||
        (strcmp(magic, PROJECT_IDENT) && strcmp(magic, OLD_PROJECT_IDENT) &&
         strcmp(magic, VERY_OLD_PROJECT_IDENT)))
    {
      if (from_ide)
      {
        messageBox(mfError | mfOKButton, _("Invalid project-file %s(%s)"),
                   prjname, magic);
      }
      else
      {
        fprintf(stderr, _("Invalid project-file %s(%s)"), prjname, magic);
        fprintf(stderr, "\n");
      }
      close_ifpstream(ifile);
      return NULL;
    }
    if (strcmp(magic, PROJECT_IDENT) == 0)
      *ifile >> version;
    else
      version = 0;
    delete magic;

    if (version > ProjectVersion)
    {
      if (from_ide)
      {
        messageBox(mfError | mfOKButton,
                   _("This project was created with a newer RHIDE version and could not be used."));
      }
      else
      {
        fprintf(stderr,
                _("This project was created with a newer RHIDE version and could not be used."));
        fprintf(stderr, "\n");
      }
      close_ifpstream(ifile);
      return NULL;
    }
    *ifile >> _project;
    if (!_CReservedWords(_project))
      DefaultReservedWords(_project);
    if (!_GPCReservedWords(_project))
      DefaultGPCReservedWords(_project);
    if (!_FPCReservedWords(_project))
      DefaultFPCReservedWords(_project);
    if (!_RHIDEUserWords(_project))
      DefaultUserWords(_project);
    if (version > 0 && !ifile->eof())
      LoadPrintSetUp(ifile);
    if (version < 1 || ifile->eof())
      PrintSetDefaults();
    close_ifpstream(ifile);
    if (project_directory)
      string_free(project_directory);
    project_directory = getcwd(NULL, 512);
    ClearFindCache();
    return _project;
  }
  if (ifile)
    close_ifpstream(ifile);
  return NULL;
}

static TSCollection *targets;
static int depcount;

static Boolean
AddTarget(const char *name)
{
  int i;
  char *tmp;

  if (!*name)
    return True;
  for (i = 0; i < targets->getCount(); i++)
  {
    if (!strcmp(name, (char *) targets->at(i)))
      return False;
  }
  string_dup(tmp, name);
  targets->insert(tmp);
  return True;
}

void
put_breakline(FILE * f, int start_len, int max_len, const char *s)
{
  int len;
  char *lf = NULL;

  while (1)
  {
    lf = strchr(s, '\n');
    if (lf)
      *lf = 0;
    len = strlen(s);
    if (len + start_len <= max_len)
    {
      fprintf(f, "%s\n", s);
      if (lf)
      {
        *lf = '\n';
        s = lf + 1;
        continue;
      }
      return;
    }
    const char *tmp;

    tmp = s + max_len - start_len;
    while (tmp > s && *tmp != ' ')
      tmp--;
    if (tmp == s)
    {
      fprintf(f, "%s\n", s);
      if (lf)
      {
        *lf = '\n';
        s = lf + 1;
        continue;
      }
      return;
    }
    const char *_tmp = s;

    do
    {
      fprintf(f, "%c", *_tmp);
      _tmp++;
    }
    while (_tmp < tmp);
    fprintf(f, "\\\n\t");
    start_len = 8;              // tabsize
    s = tmp + 1;
    if (lf)
    {
      *lf = '\n';
    }
  }
}

void
_AbsToRelPath(char *&dname, TStringCollection * vars, bool use_rel)
{
  int i, count;
  char *dir, *_dir;

  if (!dname || !*dname)
    return;
  if (use_rel)
  {
    FILE_TYPE t = get_file_type(dname);

    if (t == FILE_HEADER)
    {
      char *rel;

      if (FindFile(dname, rel, Options.include_path))
      {
        string_free(dname);
        dname = rel;
        return;
      }
    }
    /*
      projects live normaly relative to the build
      directory
    */
    if ((t == FILE_PROJECT) || (t == FILE_LIBRARY))
    {
      if (AbsToRelPath(project_directory, dname, NULL, 1))
        return;
    }
  }
  if (AbsToRelPath(project_directory, dname, NULL))
    return;
  count = vars->getCount();
  char *_dname = string_dup(dname);
  char *last_found = NULL;
  int last_dir_len = 0;

  for (i = 0; i < count; i++)
  {
    int retval, dir_len;

    dir = string_dup("$(");
    string_cat(dir, (char *) vars->at(i), ")", NULL);
    _dir = expand_rhide_spec(dir);
    dir_len = strlen(_dir);
    retval = AbsToRelPath(_dir, dname, dir);
    string_free(_dir);
    string_free(dir);
    if (retval)
    {
      if (*dname != '.')
      {
        if (dir_len > last_dir_len)
        {
          string_free(last_found);
          last_found = dname;
          last_dir_len = dir_len;
        }
        else
        {
          string_free(dname);
        }
        dname = string_dup(_dname);
      }
      else
      {
        string_free(dname);
        dname = string_dup(_dname);
      }
    }
  }
  string_free(dname);
  if (!last_found)
  {
    dname = _dname;
  }
  else
  {
    dname = last_found;
    string_free(_dname);
  }
}

static void
WriteTarget(FILE * f, TDependency * dep, int depth, TStringCollection * vars)
{
  int i;
  char tmp[10];

  if (AddTarget(FName(dep->dest_name)) == False)
    return;
  if (depcount > 0 && dep->compile_id == COMPILE_NONE)
    return;
  if (!dep->dest_name)
  {
    if (!dep->dependencies || dep->dependencies->getCount() == 0)
      return;
  }
  depcount++;
  char *deps = string_dup("DEPS_");

  sprintf(tmp, "%d", depcount - 1);
  string_cat(deps, tmp, "=", NULL);
  if ((dep->source_file_type != FILE_PROJECT) && dep->source_name)
  {
    string_cat(deps, FName(dep->source_name));
  }
  if ((AllDeps || depth < 2) && dep->dependencies &&
      dep->dependencies->getCount() > 0)
  {
    if (depcount == 1)
    {
      char *_targets = expand_rhide_spec("$(targets 0)");

      string_cat(deps, " ", _targets, NULL);
      string_free(_targets);
    }
    else
    {
      for (i = 0; i < dep->dependencies->getCount(); i++)
      {
        TDependency *_dep = (TDependency *) dep->dependencies->at(i);

        if (_dep->source_file_type != FILE_PROJECT)
        {
          char *dname;

          FindFile(FName(_dep->dest_name), dname);
          _AbsToRelPath(dname, vars);
          string_cat(deps, " ", dname, NULL);
          string_free(dname);
        }
        else
        {
          if (_dep->new_project_dep)
            string_cat(deps, " ", FName(_dep->source_name), ".force", NULL);
          else
          {
            char dir[PATH_MAX];

            split_fname_fmt(FName(_dep->dest_name), "%D", dir);
            string_cat(deps, " ", dir, FName(_dep->source_name), ".force",
                       NULL);
          }
        }
      }
    }
  }
  put_breakline(f, 0, 75, deps);
  string_free(deps);
  if (depcount == 1)
  {
    char *__targets = string_dup("NO_LINK=");

    string_cat(__targets, expand_rhide_spec("$(targets 2)"));
    put_breakline(f, 0, 75, __targets);
    fprintf(f, "LINK_FILES=$(filter-out $(NO_LINK),$(DEPS_%d))\n",
            depcount - 1);
  }
  if (dep->source_file_type == FILE_PROJECT)
  {
    char *dir, *name, *ext;

    if (dep->new_project_dep)
    {
      split_fname(FName(dep->source_name), dir, name, ext);
    }
    else
    {
      char *_dir;

      split_fname(FName(dep->dest_name), dir, name, ext);
      string_free(name);
      split_fname(FName(dep->source_name), _dir, name, ext);
      string_free(_dir);
    }
//    if (*dir && dir[strlen(dir)-1] == '/')
//      dir[strlen(dir)-1] = 0;
    fprintf(f, ".PHONY: %s%s%s.force\n", dir, name, ext);
    fprintf(f, "all:: %s%s%s.force\n", dir, name, ext);
    fprintf(f, "%s%s%s.force:\n", dir, name, ext);
    fprintf(f, "\t$(MAKE)%s%s -f %s.mak $(FLAGS_FOR_SUBPROJECTS)\n",
            *dir ? " -C " : "", *dir ? dir : "", name);
    fprintf(f, "clean::\n\t");
    if (DeleteRecursive)
      fprintf(f, "$(MAKE)%s%s -f %s.mak $(FLAGS_FOR_SUBPROJECTS) clean\n",
              *dir ? " -C " : "", *dir ? dir : "", name);
    if (recursive_make && (_PushProject(dep) == True))
    {
      int _depcount = depcount;
      TSCollection *_targets = targets;

      WriteMake();
      _PopProject();
      depcount = _depcount;
      targets = _targets;
    }
    fprintf(f, "\n");
    string_free(dir);
    string_free(name);
  }
  else
  {
    char *dname;

    FindFile(FName(dep->dest_name), dname);
    _AbsToRelPath(dname, vars);
    if (!*dname)
    {
      fprintf(f, "TARGET_%d:: ", depcount - 1);
    }
    else
    {
      fprintf(f, "%s:: ", dname);
    }
    fprintf(f, "$(DEPS_%d)\n", depcount - 1);
    string_free(dname);
    Boolean is_user;
    char *spec = GetCompilerSpec(dep, is_user);

    if (spec)
      fprintf(f, "\t%s\n", spec);
    string_free(spec);
  }
  {
    if ((AllDeps || depth < 2) && dep->dependencies &&
        dep->dependencies->getCount() > 0)
    {
      for (i = 0; i < dep->dependencies->getCount(); i++)
      {
        WriteTarget(f, (TDependency *) dep->dependencies->at(i), depth + 1,
                    vars);
      }
    }
  }
  if (dep->local_options->getCount() > 0)
  {
    fprintf(f, "LOCAL_%d=", (depcount - 1));
    for (i = 0; i < dep->local_options->getCount(); i++)
    {
      char buffer[1000], *_buffer = (char *) (dep->local_options->at(i));
      char *tmp = buffer;
      int has_spaces = 0;

      if (strchr(_buffer, ' ') != NULL)
        has_spaces = 1;
      strcpy(tmp, FName(dep->source_name));
      strcat(tmp, "___");
      while (*tmp)
        tmp++;
      if (has_spaces)
      {
        *tmp = '"';
        tmp++;
      }
      while (*_buffer)
      {
        if (*_buffer == ' ')
        {
          *tmp = 0;
          strcat(tmp, "___~~~___");
          while (*tmp)
            tmp++;
        }
        else
        {
          *tmp = *_buffer;
          tmp++;
        }
        _buffer++;
      }
      if (has_spaces)
      {
        *tmp = '"';
        tmp++;
      }
      *tmp = 0;
      fprintf(f, "\\\n\t%s", buffer);
    }
    fprintf(f, "\nLOCAL_OPTIONS += $(LOCAL_%d)\n", (depcount - 1));
  }
}

static void
WriteTargets(FILE * f, TStringCollection * vars)
{
  depcount = 0;
  targets = new TSCollection();
  WriteTarget(f, project, 1, vars);
  destroy(targets);
}

void
check_vars(TStringCollection * vars, TDirList * dirs)
{
  int i;
  ccIndex index;
  char *tmp, *temp, *temp_end, c;

  for (i = 0; i < dirs->getCount(); i++)
  {
    tmp = (char *) dirs->at(i);
    while ((temp = strchr(tmp, '$')) != NULL)
    {
      temp += 2;
      tmp = temp;
      temp_end = strchr(temp, ')');
      if (!temp_end)
        continue;
      c = *temp_end;
      *temp_end = 0;
      if (vars->search(temp, index) == False)
      {
        vars->insert(strdup(temp));
      }
      *temp_end = c;
      tmp = temp_end + 1;
    }
  }
}

void
WriteMake(char *outname, int argc, char *argv[], bool default_vars)
{
  char *dir = NULL, *fname = NULL, *ext = NULL, *name = NULL;
  FILE *f;
  const char *target0;
  TStringCollection *vars = new TStringCollection(10, 10);
  int i;

  if (!outname)
  {
    split_fname(project_name, dir, fname, ext);
    string_dup(name, dir);
    string_cat(name, fname);
    string_cat(name, ".mak");
  }
  else
  {
    string_dup(name, outname);
  }
  if (strcmp(name, "-") == 0)
    f = stdout;
  else
    f = fopen(name, "w+t");
  fprintf(f, "# This file is automatically generated by %s\n", IDEVersion);
  if (argc > 0)
  {
    int i;

    fprintf(f, "# created with the command:\n#");
    char *prog;
    BaseName(argv[0], prog, 0);
    fprintf(f, " %s", prog);
    string_free(prog);
    for (i = 1; i < argc; i++)
    {
      fprintf(f, " %s", argv[i]);
    }
    fprintf(f, "\n");
  }
  else
  {
    fprintf(f, "# created from within RHIDE\n");
  }
  check_vars(vars, Options.SrcDirs);
  check_vars(vars, Options.ObjDirs);
  check_vars(vars, Options.include_path);
  check_vars(vars, Options.library_path);
  if (default_vars)
  {
    for (i = 0; i < vars->getCount(); i++)
    {
      char *var, *env;
  
      var = (char *) vars->at(i);
      env = getenv(var);
      fprintf(f, "ifeq ($(strip $(%s)),)\n", var);
      fprintf(f, "%s=", var);
      if (env)
        fprintf(f, "%s", env);
      fprintf(f, "\nendif\n");
    }
  }
  if (Options.SrcDirs->getCount() > 0)
  {
    fprintf(f, "vpath_src=");
    for (i = 0; i < Options.SrcDirs->getCount(); i++)
    {
      if (i > 0)
        fprintf(f, " ");
      fprintf(f, "%s", ((char *) Options.SrcDirs->at(i)));
    }
    fprintf(f, "\n");
#define VS(suffix) fprintf(f,"vpath %c"#suffix" $(vpath_src)\n",'%')
    VS(.c);
    VS(.cc);
    VS(.cpp);
    VS(.C);
    VS(.cxx);
    VS(.s);
    VS(.S);
    VS(.p);
    VS(.pas);
    VS(.f);
    VS(.for);
    VS(.F);
    VS(.fpp);
    VS(.i);
    VS(.ii);
    VS(.m);
    VS(.asm);
    VS(.nsm);
#undef VS
  }
  if (Options.include_path->getCount() > 0)
  {
    char *vp = string_dup("vpath_header=");

    for (i = 0; i < Options.include_path->getCount(); i++)
    {
      if (i > 0)
        string_cat(vp, " ");
      string_cat(vp, ((char *) Options.include_path->at(i)));
    }
    put_breakline(f, 0, 75, vp);
    string_free(vp);
#define VS(suffix) fprintf(f,"vpath %c"#suffix" $(vpath_header)\n",'%')
    VS(.h);
    VS(.hpp);
    VS(.ha);
    VS(.hd);
#undef VS
  }
  if (Options.ObjDirs->getCount() > 0)
  {
    fprintf(f, "vpath_obj=");
    for (i = 0; i < Options.ObjDirs->getCount(); i++)
    {
      if (i > 0)
        fprintf(f, " ");
      fprintf(f, "%s", ((char *) Options.ObjDirs->at(i)));
    }
    fprintf(f, "\n");
    fprintf(f, "vpath %c.o $(vpath_obj)\n", '%');
  }
  WriteSpecData(f);
  if (__file_exists("rhide.env"))
    fprintf(f, "include rhide.env\n");
  target0 = FName(Project.dest_name);
  // make it the default rule
  fprintf(f, "all::\n");
  fprintf(f, "clean::\n\t");
  put_breakline(f, 0, 75,
                "rm -f $(CLEAN_FILES)");
  WriteTargets(f, vars);
  if (!target0 || !*target0)
    target0 = "TARGET_0";
  fprintf(f, "all:: %s\n", target0);
  fclose(f);
  string_free(dir);
  string_free(fname);
  string_free(ext);
  string_free(name);
  destroy(vars);
}

project_stack *PROJECT_STACK = NULL;
int stack_count = 0;

Boolean
_PushProject(const char *directory, const char *prjname)
{
  stack_count++;
  PROJECT_STACK = (project_stack *) realloc(PROJECT_STACK,
                                            stack_count *
                                            sizeof(project_stack));
  project_stack *ps = PROJECT_STACK + (stack_count - 1);

  ps->pname = project_name;
  ps->dname = getcwd(NULL, 512);
  ps->project = project;
  chdir(directory);
  project = NULL;
  project_name = NULL;
  if ((project = ReadProject(prjname)) != NULL)
  {
    string_dup(project_name, prjname);
    push_environment();
    return True;
  }
  stack_count--;
  project_name = ps->pname;
  project = ps->project;
  chdir(ps->dname);
  string_free(ps->dname);
  PROJECT_STACK = (project_stack *) realloc(PROJECT_STACK,
                                            stack_count *
                                            sizeof(project_stack));
  if (stack_count == 0)
    PROJECT_STACK = NULL;
  return False;
}

Boolean
_PushProject(TDependency * _dep)
{
  char *dname, *pname;
  const char *_pname;

  if (!_dep->new_project_dep)
  {
    dname = (char *) alloca(strlen(FName(_dep->dest_name)) + 1);
    strcpy(dname, FName(_dep->dest_name));
    _pname = FName(_dep->source_name);
    pname = (char *) alloca(strlen(_pname) + 1);
    strcpy(pname, _pname);
    BaseName(pname);
  }
  else
  {
    dname = (char *) alloca(strlen(FName(_dep->source_name)) + 1);
    pname = (char *) alloca(strlen(FName(_dep->source_name)) + 1);
    split_fname_fmt(FName(_dep->source_name), "%D%f", dname, pname);
    if (*dname && dname[strlen(dname) - 1] == '/')
      dname[strlen(dname) - 1] = 0;
  }
  return _PushProject(dname, pname);
}

void
_PopProject()
{
  if (stack_count == 0)
    return;
  stack_count--;
  project_stack *ps = PROJECT_STACK + stack_count;

  destroy(project);
  pop_environment();
  chdir(ps->dname);
  string_free(project_name);
  project_name = ps->pname;
  if (project_directory)
    string_free(project_directory);
  project_directory = ps->dname;
  project = ps->project;
  PROJECT_STACK = (project_stack *) realloc(PROJECT_STACK,
                                            stack_count *
                                            sizeof(project_stack));
  if (stack_count == 0)
    PROJECT_STACK = NULL;
  ClearFindCache();
}

void
SaveProject(TProject * _project, const char *_project_name)
{
  ofpstream *file = new ofpstream(_project_name);

  file->writeString(PROJECT_IDENT);
  *file << ProjectVersion;
  *file << _project;
  SavePrintSetUp(file);
  delete(file);
}

