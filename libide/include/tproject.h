/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#if defined( Uses_TProject ) && !defined( __TProject__ )
#define __TProject__

class TOptions;
class TParamList;

class TProject : public TDependency
{
public:
  TOptions *options;
  char *defaultprojectmask;
  ushort screen_mode;
  uint32 options2;
  uint32 options1;
  TParamList *info_files;
  char *main_function;
  ushort info_search_mode;
#define INFO_MODE_EXACT 0
#define INFO_MODE_SUBSTRING 1
#define INFO_MODE_FUZZY 2
  union {
    ushort info_search_options;
    struct {
      unsigned _case:1;
      unsigned dummy:15;
    } _info_search_options;
  };
  int fuzzy_value;
#define INFO_OPTIONS_CASE _info_search_options._case
  int max_closed_windows;
  TProject(void);
  ~TProject(void);
private:
  virtual const char *streamableName() const
    { return name; }
protected:
  TProject( StreamableInit ) : TDependency(streamableInit) {}
  virtual void write( opstream& );
  virtual void *read( ipstream& );
public:
  static const char * const near name;
  static TStreamable *build();
  TProject & operator = ( const TProject& );
};

inline ipstream& operator >> ( ipstream& is, TProject& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TProject*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TProject& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TProject* cl )
    { return os << (TStreamable *)cl; }

#endif
