/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TDepCollection) && !defined( __TDepCollection__)
#define __TDepCollection__

class TDepCollection:public TSortedCollection
{
public:
  TDepCollection(ccIndex aLimit, ccIndex aDelta):TSortedCollection(aLimit,
                                                                   aDelta)
  {
  }
private:
    virtual const char *streamableName() const
  {
    return name;
  }
  virtual void *readItem(ipstream &);
  virtual void writeItem(void *, opstream &);
  virtual void *keyOf(void *);
  virtual int compare(void *, void *);
  virtual void freeItem(void *item);

protected:
TDepCollection(StreamableInit):TSortedCollection(streamableInit)
  {
  }
  virtual void write(opstream &);
  virtual void *read(ipstream &);

public:
  static const char *const name;
  static TStreamable *build();

  TDepCollection & operator = (const TDepCollection &);
};

inline ipstream & operator >> (ipstream & is, TDepCollection & cl)
{
  return is >> (TStreamable &) cl;
}

inline ipstream & operator >> (ipstream & is, TDepCollection * &cl)
{
  return is >> (void *&) cl;
}

inline opstream & operator << (opstream & os, TDepCollection & cl)
{
  return os << (TStreamable &) cl;
}

inline opstream & operator << (opstream & os, TDepCollection * cl)
{
  return os << (TStreamable *) cl;
}

#endif

#if defined( Uses_TDependency ) && !defined( __TDependency__ )
#define __TDependency__

class TDepCollection;
class TParamList;

class TFileName;

class TDependency:public TObject, public TStreamable
{
public:
  TFileName * source_name;
  TFileName *dest_name;
#define FName(x) ((x)?(x)->Name():(const char *)NULL)
#define InitFName(x,y) (x) = new TFileName((y)?(y):"")
  char *compiler;
  char *error_check;
  FILE_TYPE source_file_type;
  FILE_TYPE dest_file_type;
  COMPILE_ID compile_id;
  ERROR_TYPE error_type;
  COMPILER_TYPE compiler_type;
  TDepCollection *dependencies;
  TParamList *local_options;
  union
  {
    uint32 flags;
    struct
    {
      unsigned _exclude_from_link:1;
#     define exclude_from_link _flags._exclude_from_link
      unsigned _rebuild_only_nonexistant:1;
#     define rebuild_only_nonexistant _flags._rebuild_only_nonexistant
      unsigned dummy:29;
      unsigned _new_project_dep:1;
#     define new_project_dep _flags._new_project_dep
    }
    _flags;
  };
    TDependency(void);
   ~TDependency();
private:
    virtual const char *streamableName() const
  {
    return name;
  }
protected:
    TDependency(StreamableInit)
  {
  }
  virtual void write(opstream &);
  virtual void *read(ipstream &);

public:
  static const char *const name;
  static TStreamable *build();

  TDependency & operator = (const TDependency &);
};

inline ipstream & operator >> (ipstream & is, TDependency & cl)
{
  return is >> (TStreamable &) cl;
}

inline ipstream & operator >> (ipstream & is, TDependency * &cl)
{
  return is >> (void *&) cl;
}

inline opstream & operator << (opstream & os, TDependency & cl)
{
  return os << (TStreamable &) cl;
}

inline opstream & operator << (opstream & os, TDependency * cl)
{
  return os << (TStreamable *) cl;
}

#endif
