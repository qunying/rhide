/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TProject ) && !defined( __TProject__ )
#define __TProject__

class TOptions;
class TParamList;

class TProject:public TDependency
{
public:
  TOptions * options;
  char *defaultprojectmask;
  ushort screen_mode;
  uint32 options2;
  uint32 options1;
  union
  {
    uint32 private_options;
    struct
    {
      unsigned pr_VarFilenames:1;
      unsigned pr_dummy:31;
    }
    pr;
  };
  TParamList *info_files;
  char *main_function;
  ushort info_search_mode;
#define INFO_MODE_EXACT 0
#define INFO_MODE_SUBSTRING 1
#define INFO_MODE_FUZZY 2
  union
  {
    ushort info_search_options;
    struct
    {
      unsigned _case:1;
      unsigned dummy:15;
    }
    _info_search_options;
  };
  int fuzzy_value;
#define INFO_OPTIONS_CASE _info_search_options._case
  int max_closed_windows;
    TProject(void);
   ~TProject(void);
private:
    virtual const char *streamableName() const
  {
    return name;
  }
protected:
    TProject(StreamableInit):TDependency(streamableInit)
  {
  }
  virtual void write(opstream &);
  virtual void *read(ipstream &);

public:
  static const char *const name;
  static TStreamable *build();

  TProject & operator = (const TProject &);
};

inline ipstream & operator >> (ipstream & is, TProject & cl)
{
  return is >> (TStreamable &) cl;
}

inline ipstream & operator >> (ipstream & is, TProject * &cl)
{
  return is >> (void *&) cl;
}

inline opstream & operator << (opstream & os, TProject & cl)
{
  return os << (TStreamable &) cl;
}

inline opstream & operator << (opstream & os, TProject * cl)
{
  return os << (TStreamable *) cl;
}

typedef void (*expand_function) (TProject *);
extern expand_function expand_filenames;


#endif
