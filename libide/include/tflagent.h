/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TFlagEntry ) && !defined( __TFlagEntry__ )
#define __TFlagEntry__

typedef enum
{
  FLAG_TYPE,
  OPTIONAL_INT_TYPE,
  INT_TYPE,
  OPTIONAL_STRING_TYPE,
  STRING_TYPE,
  OPTIONAL_CHAR_TYPE,
  CHAR_TYPE
}
FlagType;

class TFlagEntry:public TObject, public TStreamable
{
public:
  char *value;
  Boolean activated;
  Boolean hasoption;
  FlagType type;
  union
  {
    int ivalue;
    struct
    {
      char *allowed;
      char *value;
    }
    svalue;
    char *cvalue;
  }
  data;
    TFlagEntry(char *flag);
    TFlagEntry(const class TFlagEntry *);
   ~TFlagEntry();
  char *GetText();
  char *GetValue();
  char *GetControlString();
  void ToggleActive();
  static const char *const name;
  static TStreamable *build();
private:
    virtual const char *streamableName() const
  {
    return name;
  }
protected:
    TFlagEntry(StreamableInit);
  virtual void write(opstream &);
  virtual void *read(ipstream &);
};

inline ipstream & operator >> (ipstream & is, TFlagEntry & cl)
{
  return is >> (TStreamable &) cl;
}

inline ipstream & operator >> (ipstream & is, TFlagEntry * &cl)
{
  return is >> (void *&) cl;
}

inline opstream & operator << (opstream & os, TFlagEntry & cl)
{
  return os << (TStreamable &) cl;
}

inline opstream & operator << (opstream & os, TFlagEntry * cl)
{
  return os << (TStreamable *) cl;
}

#endif
