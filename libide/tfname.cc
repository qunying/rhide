/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TNSSortedCollection
#include <tv.h>

#include <rhutils.h>

#define Uses_TFileName
#include <libide.h>

#include <string.h>
#include <stdio.h>

#undef REMOVE_FILENAMES
#define USE_SLASH

struct FileEntry
{
  char *name;
#ifdef USE_SLASH
  char *slash;
#endif
#ifdef REMOVE_FILENAMES
  int alloc_count;
#endif
    FileEntry():name(NULL)
#ifdef USE_SLASH
   , slash(NULL)
#endif
  {
  }
   ~FileEntry()
  {
    if (name != NULL)
      delete name;
  }
};

class FileNameCollection:TNSSortedCollection
{
public:
  FileNameCollection(ccIndex aLimit,
                     ccIndex aDelta):TNSSortedCollection(aLimit, aDelta)
  {
  }
#ifndef USE_SLASH
  virtual void *keyOf(void *item);
#endif
  char *addName(char *name);

#ifdef USE_SLASH
  Boolean Search(const char *, ccIndex &);
#else
  Boolean Search(const char *name, ccIndex & aIndex)
  {
    return search(name, aIndex);
  }
#endif
#ifdef REMOVE_FILENAMES
  void removeName(char *name);
#endif
private:
  virtual void freeItem(void *item);
  virtual int compare(void *key1, void *key2);
};

#ifndef USE_SLASH
void *
FileNameCollection::keyOf(void *item)
{
  return ((FileEntry *) item)->name;
}
#endif

#ifdef USE_SLASH
Boolean
FileNameCollection::Search(const char *name, ccIndex & index)
{
  FileEntry fe;

  fe.name = (char *) name;
  fe.slash = strrchr(name, '/');
  if (!fe.slash)
    fe.slash = fe.name;
  else
    fe.slash++;
  Boolean ret = search(&fe, index);

  fe.name = NULL;
  return ret;
}
#endif

int
FileNameCollection::compare(void *key1, void *key2)
{
#ifndef USE_SLASH
  return strcmp((char *) key1, (char *) key2);
#else
  int ret;
  FileEntry *k1 = (FileEntry *) key1;
  FileEntry *k2 = (FileEntry *) key2;

  ret = strcmp(k1->slash, k2->slash);
  if (!ret)
    ret = strcmp(k1->name, k2->name);
  return ret;
#endif
}

void
FileNameCollection::freeItem(void *item)
{
  ::delete(FileEntry *) item;
}

char *
FileNameCollection::addName(char *name)
{
  ccIndex index;
  FileEntry *fe;

  if (Search(name, index) == True)
  {
    fe = (FileEntry *) at(index);
  }
  else
  {
    fe = new FileEntry;
    fe->name = string_dup(name);
#ifdef USE_SLASH
    fe->slash = strrchr(fe->name, '/');
    if (!fe->slash)
      fe->slash = fe->name;
    else
      fe->slash++;
#endif
#ifdef REMOVE_FILENAMES
    fe->alloc_count = 0;
#endif
    atInsert(index, fe);
  }
#ifdef REMOVE_FILENAMES
  fe->alloc_count++;
#endif
  return fe->name;
}

#ifdef REMOVE_FILENAMES
void
FileNameCollection::removeName(char *name)
{
  ccIndex index;

  if (Search(name, index) == True)
  {
    FileEntry *fe = (FileEntry *) at(index);

    fe->alloc_count--;
    if (!fe->alloc_count)
    {
      atFree(index);
    }
  }
}
#endif

static FileNameCollection Names(127, 128);

TFileName::TFileName(const char *name)
{
  NAME = Names.addName((char *) name);
}

TFileName::~TFileName()
{
#ifdef REMOVE_FILENAMES
  Names.removeName(NAME);
#endif
}
