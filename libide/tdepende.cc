/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TStreamable

#define Uses_TParamList
#include <libtvuti.h>

#include <rhutils.h>

#define Uses_TDepCollection
#define Uses_TDependency
#include <libide.h>

#include <string.h>
#include <stdio.h>

#ifdef OLD_TDEP
#define _InitName(x,y) x = y
#else
#define _InitName(x,y) \
{\
  char *__tmp = y;\
  if (__tmp)\
  {\
    x = new TFileName(__tmp);\
    string_free(__tmp);\
  }\
  else\
    x = NULL;\
}

#endif

static ushort TDependency_Version = 4;
static ushort TDependency_Version_Read;

TStreamable *
TDepCollection::build()
{
  return new TDepCollection(streamableInit);
}

void *
TDepCollection::readItem(ipstream & is)
{
  TDependency *dep;

  is >> dep;
  return dep;
}

void
TDepCollection::writeItem(void *data, opstream & os)
{
  os << (TDependency *) data;
}

void *
TDepCollection::keyOf(void *item)
{
  return ((TDependency *) item)->source_name ?
    (char *) FName(((TDependency *) item)->source_name) :
    (char *) FName(((TDependency *) item)->dest_name);
}

int
TDepCollection::compare(void *key1, void *key2)
{
  int result;

  if (!key1)
    result = -1;
  else if (!key2)
    result = 1;
  else
    result = strcasecmp((char *) key1, (char *) key2);
  return result;
}

void
TDepCollection::write(opstream & os)
{
  TCollection::write(os);
}

void *
TDepCollection::read(ipstream & is)
{
  TCollection::read(is);
  duplicates = False;
  if (count > 0 && TDependency_Version_Read < 2)
  {
    void **olditems;
    int i, _count;
    olditems = new void *[count];
    memcpy(olditems, items, count * sizeof(void *));

    _count = count;
    removeAll();
    for (i = 0; i < _count; i++)
      insert(olditems[i]);
    delete(olditems);
  }
  return this;
}

TStreamable *
TDependency::build()
{
  return new TDependency(streamableInit);
}

TDependency::TDependency()
{
  source_name = NULL;
  dest_name = NULL;
  dependencies = NULL;
  local_options = new TParamList(NULL);
  source_file_type = FILE_UNKNOWN;
  dest_file_type = FILE_UNKNOWN;
  compile_id = COMPILE_UNKNOWN;
  compiler_type = COMPILER_AUTO;
  error_type = ERROR_AUTO;
  compiler = NULL;
  error_check = NULL;
  flags = 0;
  new_project_dep = 1;
}

TDependency::~TDependency()
{
  delete source_name;
  delete dest_name;

  string_free(compiler);
  string_free(error_check);
  destroy(local_options);
  destroy(dependencies);
}

void *
TDependency::read(ipstream & is)
{
  ushort temp;
  char *localoptions;

  is >> TDependency_Version_Read;
  _InitName(source_name, is.readString());
  _InitName(dest_name, is.readString());
  is >> temp;
  source_file_type = (FILE_TYPE) temp;
  is >> temp;
  dest_file_type = (FILE_TYPE) temp;
  is >> temp;
  compile_id = (COMPILE_ID) temp;
  is >> dependencies;
  localoptions = is.readString();
  local_options = new TParamList(localoptions);
  if (localoptions != NULL)
    delete(localoptions);
  if (TDependency_Version_Read < 3)
    flags = 0;
  else
    is >> flags;
  if (TDependency_Version_Read < 4)
  {
    compiler_type = COMPILER_AUTO;
    error_type = ERROR_AUTO;
    compiler = NULL;
    error_check = NULL;
  }
  else
  {
    is >> temp;
    compiler_type = (COMPILER_TYPE) temp;
    is >> temp;
    error_type = (ERROR_TYPE) temp;
    compiler = is.readString();
    error_check = is.readString();
  }
  return this;
}

void
TDependency::write(opstream & os)
{
  ushort temp;
  char *localoptions;

  os << TDependency_Version;
  os.writeString(FName(source_name));
  os.writeString(FName(dest_name));
  temp = (ushort) source_file_type;
  os << temp;
  temp = (ushort) dest_file_type;
  os << temp;
  temp = (ushort) compile_id;
  os << temp;
  os << dependencies;
  local_options->ToString(localoptions);
  if (strlen(localoptions) == 0)
    os.writeString(NULL);
  else
    os.writeString(localoptions);
  delete(localoptions);
  os << flags;
  temp = (ushort) compiler_type;
  os << temp;
  temp = (ushort) error_type;
  os << temp;
  os.writeString(compiler);
  os.writeString(error_check);
}

void
TDepCollection::freeItem(void *item)
{
  destroy((TDependency *) item);
}

TDependency & TDependency::operator = (const TDependency & d)
{
  if (d.source_name)
    InitFName(source_name, FName(d.source_name));
  if (d.dest_name)
    InitFName(dest_name, FName(d.dest_name));
  compiler = string_dup(d.compiler);
  error_check = string_dup(d.compiler);
  source_file_type = d.source_file_type;
  dest_file_type = d.dest_file_type;
  compile_id = d.compile_id;
  error_type = d.error_type;
  compiler_type = d.compiler_type;
  if (dependencies)
  {
    destroy(dependencies);
    dependencies = NULL;
  }
  if (d.dependencies)
  {
    dependencies = new TDepCollection(5, 5);
    *dependencies = *d.dependencies;
  }
  if (d.local_options);
  *local_options = *d.local_options;
  flags = d.flags;
  return *this;
}

TDepCollection & TDepCollection::operator = (const TDepCollection & d)
{
  freeAll();
  shouldDelete = d.shouldDelete;
  duplicates = d.duplicates;
  delta = d.delta;
  setLimit(d.limit);
  int
    i;

  for (i = 0; i < d.count; i++)
  {
    TDependency *
      dep = new TDependency();

    *dep = *(TDependency *) d.items[i];
    insert(dep);
  }
  return *this;
}
