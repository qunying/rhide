/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TStreamable
#define Uses_TStringCollection

#define Uses_TDirList
#define Uses_TParamList
#include <libtvuti.h>

#define Uses_TOptions
#define Uses_TFlagCollection
#include <libide.h>

static ushort TOptions_Version = 8;

void *
TOptions::read(ipstream & is)
{
  ushort version;

  is >> version;
  is >> include_path;
  is >> library_path;
  is >> libs;
  is >> user_libs;
  is >> debug_level;
  is >> link_opt;
  is >> comp_opt;
  is >> ObjDirs;
  is >> SrcDirs;
  is >> opt_flags;
  is >> warn_flags;
  is >> debug_flags;
  is >> cxx_flags;
  is >> c_flags;
  is >> ProgArgs;
  if (version < 2)
    reserved = NULL;
  else
    is >> reserved;
  if (version < 3)
    user_words = new TStringCollection(4, 4);
  else
    is >> user_words;
  if (version < 4)
  {
    pascal_flags = new TFlagCollection();
    gpc_reserved = new TStringCollection(4, 4);
  }
  else
  {
    is >> pascal_flags;
    is >> gpc_reserved;
  }
  if (version < 5)
  {
    fortran_flags = new TFlagCollection();
  }
  else
  {
    is >> fortran_flags;
  }
  if (version < 6)
  {
    StdInc = new TDirList();
  }
  else
  {
    is >> StdInc;
  }
  if (version < 7)
  {
    ada_flags = new TFlagCollection();
  }
  else
  {
    is >> ada_flags;
  }
  if (version < 8)
  {
    fpc_flags = new TFlagCollection();
    fpc_reserved = new TStringCollection(4, 4);
  }
  else
  {
    is >> fpc_flags;
    is >> fpc_reserved;
  }
  return this;
}

void
TOptions::write(opstream & os)
{
  os << TOptions_Version;
  os << include_path;
  os << library_path;
  os << libs;
  os << user_libs;
  os << debug_level;
  os << link_opt;
  os << comp_opt;
  os << ObjDirs;
  os << SrcDirs;
  os << opt_flags;
  os << warn_flags;
  os << debug_flags;
  os << cxx_flags;
  os << c_flags;
  os << ProgArgs;
  os << reserved;
  os << user_words;
  os << pascal_flags;
  os << gpc_reserved;
  os << fortran_flags;
  os << StdInc;
  os << ada_flags;
  os << fpc_flags;
  os << fpc_reserved;
}

TOptions::TOptions()
{
  include_path = new TDirList();
  library_path = new TDirList();
  user_libs = new TDirList();
  libs = 0;
  debug_level = 0;
  link_opt = new TParamList();
  comp_opt = new TParamList();
  ObjDirs = new TDirList();
  SrcDirs = new TDirList();
  opt_flags = new TFlagCollection();
  warn_flags = new TFlagCollection();
  debug_flags = new TFlagCollection();
  cxx_flags = new TFlagCollection();
  c_flags = new TFlagCollection();
  ProgArgs = new TParamList();
  reserved = new TStringCollection(4, 4);
  user_words = new TStringCollection(4, 4);
  pascal_flags = new TFlagCollection();
  gpc_reserved = new TStringCollection(4, 4);
  fpc_flags = new TFlagCollection();
  fpc_reserved = new TStringCollection(4, 4);
  fortran_flags = new TFlagCollection();
  StdInc = new TDirList();
  ada_flags = new TFlagCollection();
}

TOptions & TOptions::operator = (const TOptions & opt)
{
#define D(x) *x = *opt.x
  D(include_path);
  D(library_path);
  D(user_libs);
  libs = opt.libs;
  debug_level = opt.debug_level;
  D(link_opt);
  D(comp_opt);
  D(ObjDirs);
  D(SrcDirs);
  D(opt_flags);
  D(warn_flags);
  D(debug_flags);
  D(cxx_flags);
  D(c_flags);
  D(ProgArgs);
  D(reserved);
  D(user_words);
  D(pascal_flags);
  D(gpc_reserved);
  D(fortran_flags);
  D(StdInc);
  D(ada_flags);
  D(fpc_flags);
  D(fpc_reserved);
  return *this;
}

TStreamable *
TOptions::build()
{
  return new TOptions(streamableInit);
}

TOptions::~TOptions()
{
  destroy(include_path);
  destroy(library_path);
  destroy(user_libs);
  destroy(link_opt);
  destroy(comp_opt);
  destroy(ObjDirs);
  destroy(SrcDirs);
  destroy(opt_flags);
  destroy(warn_flags);
  destroy(debug_flags);
  destroy(cxx_flags);
  destroy(c_flags);
  destroy(ProgArgs);
  destroy(reserved);
  destroy(user_words);
  destroy(pascal_flags);
  destroy(gpc_reserved);
  destroy(fortran_flags);
  destroy(StdInc);
  destroy(ada_flags);
  destroy(fpc_flags);
  destroy(fpc_reserved);
}
