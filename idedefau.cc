/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TOptions
#define Uses_TProject
#define Uses_TStringCollection
#define Uses_TFlagEntry
#define Uses_TFlagCollection
#include <libide.h>
#include "rhide.h"

#define F(x) x,
static char *c_flags[] = {
#include "flags/c_opt.h"
  NULL
};

static char *cxx_flags[] = {
#include "flags/cxx_opt.h"
  NULL
};

static char *warn_flags[] = {
#include "flags/warn_opt.h"
  NULL
};

static char *opt_flags[] = {
#include "flags/opt_opt.h"
  NULL
};

static char *debug_flags[] = {
#include "flags/deb_opt.h"
  NULL
};

static char *reserved[] = {
#include "flags/reserved.h"
  NULL
};

static char *GPCreserved[] = {
#include "flags/gpcreser.h"
  NULL
};

static char *FPCreserved[] = {
#include "flags/fpcreser.h"
  NULL
};

static char *pascal_flags[] = {
#include "flags/pas_opt.h"
  NULL
};

static char *fpc_flags[] = {
#include "flags/fpc_opt.h"
  NULL
};

static char *fortran_flags[] = {
#include "flags/f_opt.h"
  NULL
};

static char *ada_flags[] = {
#include "flags/ada_opt.h"
  NULL
};

#undef F

static void
InitOptions(char *opts[], TFlagCollection * &coll)
{
  int i;

  if (!coll)
    coll = new TFlagCollection();
  i = 0;
  while (opts[i])
  {
    coll->insert(new TFlagEntry(opts[i]));
    i++;
  }
}

#define GETFLAGS(lower,upper)\
void Default##upper##Flags()\
{\
  if (Options.lower##_flags) Options.lower##_flags->freeAll();\
  InitOptions(lower##_flags,Options.lower##_flags);\
}

GETFLAGS(opt, Opt)
GETFLAGS(warn, Warn)
GETFLAGS(debug, Debug)
GETFLAGS(cxx, CXX)
GETFLAGS(c, C)
GETFLAGS(pascal, Pascal)
GETFLAGS(fpc, Fpc) GETFLAGS(fortran, Fortran) GETFLAGS(ada, Ada)
     void
     DefaultReservedWords(TProject * p)
{
  int i;
  ccIndex index;

  if (!p)
    p = project;
  if (_CReservedWords(p))
    _CReservedWords(p)->freeAll();
  else
    _CReservedWords(p) = new TStringCollection(15, 16);
  i = 0;
  while (reserved[i])
  {
    if (_CReservedWords(p)->search(reserved[i], index) == False)
      _CReservedWords(p)->insert(newStr(reserved[i]));
    i++;
  }
  c_words_changed = 1;
}

void
DefaultGPCReservedWords(TProject * p)
{
  int i;
  ccIndex index;

  if (!p)
    p = project;
  if (_GPCReservedWords(p))
    _GPCReservedWords(p)->freeAll();
  else
    _GPCReservedWords(p) = new TStringCollection(15, 16);
  i = 0;
  while (GPCreserved[i])
  {
    if (_GPCReservedWords(p)->search(GPCreserved[i], index) == False)
      _GPCReservedWords(p)->insert(newStr(GPCreserved[i]));
    i++;
  }
  gpc_words_changed = 1;
}

void
DefaultFPCReservedWords(TProject * p)
{
  int i;
  ccIndex index;

  if (!p)
    p = project;
  if (_FPCReservedWords(p))
    _FPCReservedWords(p)->freeAll();
  else
    _FPCReservedWords(p) = new TStringCollection(15, 16);
  i = 0;
  while (FPCreserved[i])
  {
    if (_FPCReservedWords(p)->search(FPCreserved[i], index) == False)
      _FPCReservedWords(p)->insert(newStr(FPCreserved[i]));
    i++;
  }
  fpc_words_changed = 1;
}

void
DefaultUserWords(TProject * p)
{
  if (!p)
    p = project;
  if (_RHIDEUserWords(p))
    _RHIDEUserWords(p)->freeAll();
  else
    _RHIDEUserWords(p) = new TStringCollection(15, 16);
  user_words_changed = 1;
}
