/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TParamList ) && !defined( __TParamList__ )
#define __TParamList__

class TParamList:public TCollection
{
public:
  TParamList(void):TCollection(10, 1)
  {
  }
  TParamList(const char *);
  Boolean FromString(const char *);
  void ToString(char *&);

  TParamList & operator = (const TParamList &);
#if !defined( NO_STREAM )
private:
  virtual void *readItem(ipstream &);
  virtual void writeItem(void *, opstream &);

  STREAMIO(TParamList)
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
STREAMINLINES(TParamList)
#endif // NO_STREAM
#endif
