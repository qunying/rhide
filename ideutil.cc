/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define Uses_TNSSortedCollection
#define Uses_ifpstream
#define Uses_fpstream

#define Uses_TDirList
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#define Uses_TProject
#define Uses_TOptions
#define Uses_ideFunctions
#define Uses_TDepCollection
#include <libide.h>
#include "rhide.h"

int debug_dependencies = 0;
int debug_commands = 0;
int debug_tempfiles = 0;
int debug_files = 0;
int c_words_changed = 1;
int gpc_words_changed = 1;
int fpc_words_changed = 1;
int user_words_changed = 1;

char *RHIDE_DIR = NULL;
char *RHIDE_NAME = NULL;
char *RHIDE_EXT = NULL;

int
FindFile(char *name, char *&full_name)
{
  return (Boolean) FindFile((const char *) name, full_name) == True ? 1 : 0;
}

Boolean
FindFile(const char *name, char *&full_name)
{
  FILE_TYPE t;
  Boolean retval = False;

  if (!name || !*name)
  {
    string_dup(full_name, "");
    return False;
  }
  if (debug_files)
  {
    fprintf(stderr, _("Search for file '%s'\n"), name);
  }
  if (strchr(name, '/') != NULL && __file_exists(name))
  {
    string_dup(full_name, name);
    FExpand(full_name);
    retval = True;
  }
  else
  {
    t = get_file_type(name);
    switch (t)
    {
      case FILE_C_SOURCE:
      case FILE_C_SOURCE_I:
      case FILE_CC_SOURCE:
      case FILE_CC_SOURCE_II:
      case FILE_ASM_SOURCE:
      case FILE_FLEX_SOURCE:
      case FILE_BISON_SOURCE:
      case FILE_OBJC_SOURCE:
      case FILE_PASCAL_SOURCE:
      case FILE_FPC_SOURCE:
      case FILE_FORTRAN_SOURCE:
      case FILE_FORTRAN_SOURCE_PRE:
      case FILE_ADA_SOURCE:
      case FILE_ADA_SPEC:
        retval = FindFile(name, Options.SrcDirs, full_name, False);
        break;
      case FILE_LIBRARY:
      case FILE_DLL:
        retval = FindFile(name, Options.library_path, full_name, False);
        break;
      case FILE_OBJECT:
        retval = FindFile(name, Options.ObjDirs, full_name);
        break;
      case FILE_HEADER:
        retval = FindFile(name, Options.include_path, full_name, False);
        break;
      case FILE_EXE:
      /*
        EXE files are searched only in the current directory.
        Maybe I add a separate option for that in the future.
      */
        string_dup(full_name, name);
        FExpand(full_name);
        retval = __file_exists(full_name);
        break;
      default:
/* What is with files, which have no known source suffixes?
   I think this is the better way to find it. As an example
   I have in the doc directory a project doc.gpr where I have
   inserted the file rhide.tx which should be searched also
   in the source directories. May be I will add sometimes a
   path for so called 'other files'.
*/
        retval = FindFile(name, Options.SrcDirs, full_name, False);
        break;
    }
  }
  if (debug_files)
  {
    if (retval == True)
      fprintf(stderr, _("found '%s'\n"), full_name);
    else
      fprintf(stderr, _("not found: take '%s'\n"), full_name);
  }
  return retval;
}

class FindCacheCollection:public TNSSortedCollection
{
public:
  FindCacheCollection(ccIndex alimit,
                      ccIndex adelta):TNSSortedCollection(alimit, adelta)
  {
  }
private:
    virtual int compare(void *, void *);
  virtual void freeItem(void *);
};

struct FileFind
{
  char *name;
  char *full_name;
    FileFind(const char *_name, const char *_full_name)
  {
    string_dup(name, _name);
    if (_full_name)
      string_dup(full_name, _full_name);
    else
      full_name = NULL;
  }
   ~FileFind()
  {
    string_free(name);
    string_free(full_name);
  }
};

int Fcompcount;

int
FindCacheCollection::compare(void *key1, void *key2)
{
  return strcmp(((FileFind *) key1)->name, ((FileFind *) key2)->name);
}

void
FindCacheCollection::freeItem(void *item)
{
  delete((FileFind *) item);
}

static FindCacheCollection *FindCache = NULL;

void
ClearFindCache()
{
  if (debug_files)
  {
    fprintf(stderr, _("clearing the FindCache\n"));
  }
  destroy(FindCache);
  FindCache = NULL;
}

Boolean
FindFile(const char *name, char *&rel_name, TDirList * list)
{
  rel_name = NULL;
  for (int i = 0; i < list->getCount(); i++)
  {
    string_dup(rel_name, name);
    char *dir = expand_rhide_spec((const char *) list->at(i));
    FExpand(dir);
    Boolean found = AbsToRelPath(dir, rel_name, NULL, 0);

    string_free(dir);
    if (found)
      return True;
    string_free(rel_name);
  }
  return False;
}

Boolean
FindFile(const char *name, TDirList * list, char *&full_name, Boolean uselist)
{
  int i;
  FileFind *ff = NULL;
  ccIndex findIndex;
  char bname[PATH_MAX];

  if (!name)
  {
    string_dup(full_name, "");
    return False;
  }
  strcpy(bname, name);
  // That's now a try for allowing directories in 'name'
  if (!NoFileCache)
  {
    ff = new FileFind(bname, NULL);
    if (!FindCache)
      FindCache = new FindCacheCollection(100, 100);
    if (FindCache->search(ff, findIndex) == True)
    {
      string_dup(full_name,
                 ((FileFind *) FindCache->at(findIndex))->full_name);
      delete(ff);
      if (debug_files)
      {
        fprintf(stderr, _("found '%s' in FindCache\n"), name);
      }
      return True;
    }
  }
  if (__file_exists(bname))
  {
    string_dup(full_name, bname);
    FExpand(full_name);
    if (debug_files)
    {
      fprintf(stderr, _("found '%s' in current directory\n"), name);
    }
    if (!NoFileCache)
    {
      string_dup(ff->full_name, full_name);
      FindCache->atInsert(findIndex, ff);
    }
    return True;
  }
  for (i = 0; i < list->getCount(); i++)
  {
    full_name = expand_rhide_spec((const char *) list->at(i));
    string_cat(full_name, "/");
    string_cat(full_name, bname);
    if (__file_exists(full_name))
    {
      if (debug_files)
      {
        fprintf(stderr, _("found '%s' using the search directory '%s'\n"),
                name, (char *) list->at(i));
      }
      FExpand(full_name);
      if (!NoFileCache)
      {
        string_dup(ff->full_name, full_name);
        FindCache->atInsert(findIndex, ff);
      }
      return True;
    }
    string_free(full_name);
  }
  {
    FILE_TYPE t;

    if ((t = get_file_type(name)) == FILE_ADA_SOURCE || t == FILE_ADA_SPEC)
    {
      full_name = expand_rhide_spec("$(ADA_INCLUDE_PATH)");
      if (*full_name)
      {
        string_cat(full_name, "/", bname, NULL);
        if (__file_exists(full_name))
        {
          if (debug_files)
          {
            fprintf(stderr, _("found '%s' using the search directory '%s'\n"),
                    name, "$(ADA_INCLUDE_PATH)");
          }
          FExpand(full_name);
          if (!NoFileCache)
          {
            string_dup(ff->full_name, full_name);
            FindCache->atInsert(findIndex, ff);
          }
          return True;
        }
      }
      string_free(full_name);
    }
  }
  if (uselist == True && list && list->getCount() > 0)
  {
    full_name = expand_rhide_spec((const char *) list->at(0));
    string_cat(full_name, "/");
    string_cat(full_name, bname);
  }
  else
  {
    string_dup(full_name, name);
  }
  FExpand(full_name);
  if (!NoFileCache)
    delete(ff);
  return False;
}

#ifdef FILEFINDDEBUG
static __attribute__ ((__destructor__))
     void print_findfiles()
{
  if (!FindCache)
    return;
  int i, count = FindCache->getCount();

  fprintf(stderr, "FindFiles(%d)\n", count);
  for (i = 0; i < count; i++)
  {
    FileFind *ff = (FileFind *) FindCache->at(i);

    fprintf(stderr, "%s[%s]\n", ff->full_name, ff->name);
  }
}
#endif

//#define USE_OPEN

ifpstream *
open_ifpstream(const char *name)
{
#ifdef USE_OPEN
  int handle;

#ifdef O_BINARY
  handle = open(name, O_RDONLY | O_BINARY);
#else
  handle = open(name, O_RDONLY);
#endif
  if (handle < 0)
  {
    return NULL;
  }
  return new ifpstream(handle);
#else
  ifpstream *ret = new ifpstream();
  ret->open(name);
  return ret;
#endif
}

void
close_ifpstream(ifpstream * is)
{
#ifdef USE_OPEN
  is->close();
#endif
  delete is;
}

static void
ConvertName(TFileName * &_name)
{
  if (!_name)
    return;
  char *name = expand_rhide_spec(FName(_name));
  delete _name;

  InitFName(_name, name);
  string_free(name);
}

static void
ConvertDep(TDependency * dep)
{
  ConvertName(dep->source_name);
  ConvertName(dep->dest_name);
  int i;

  if (dep->dependencies)
  {
    for (i = 0; i < dep->dependencies->getCount(); i++)
    {
      ConvertDep((TDependency *) dep->dependencies->at(i));
    }
  }
}

void
ExpandFileNames(TProject * prj)
{
  TProject *old = project;

  project = prj;
  ConvertDep(project);
  project = old;
}


