/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TOptions ) && !defined( __TOptions__ )
#define __TOptions__

class TDirList;
class TFlagCollection;
class TParamList;
class TStringCollection;

class TOptions:public TObject, public TStreamable
{
public:
  TDirList * include_path;
  TDirList *library_path;
  TDirList *user_libs;
  TFlagCollection *opt_flags;
  TFlagCollection *warn_flags;
  TFlagCollection *debug_flags;
  TFlagCollection *cxx_flags;
  TFlagCollection *c_flags;
  TStringCollection *reserved;
  TStringCollection *user_words;
  ushort libs;
  ushort debug_level;
  TParamList *link_opt;
  TParamList *comp_opt;
  TDirList *ObjDirs;
  TDirList *SrcDirs;
  TParamList *ProgArgs;
  TFlagCollection *pascal_flags;
  TStringCollection *gpc_reserved;
  TFlagCollection *fpc_flags;
  TStringCollection *fpc_reserved;
  TFlagCollection *fortran_flags;
  TDirList *StdInc;
  TFlagCollection *ada_flags;

    TOptions();
   ~TOptions();
private:
    virtual const char *streamableName() const
  {
    return name;
  }
protected:
    TOptions(StreamableInit)
  {
  }
  virtual void write(opstream &);
  virtual void *read(ipstream &);

public:
  static const char *const name;
  static TStreamable *build();

  TOptions & operator = (const TOptions &);
};

inline ipstream & operator >> (ipstream & is, TOptions & cl)
{
  return is >> (TStreamable &) cl;
}

inline ipstream & operator >> (ipstream & is, TOptions * &cl)
{
  return is >> (void *&) cl;
}

inline opstream & operator << (opstream & os, TOptions & cl)
{
  return os << (TStreamable &) cl;
}

inline opstream & operator << (opstream & os, TOptions * cl)
{
  return os << (TStreamable *) cl;
}

#endif
