/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TDirList ) && !defined( __TDirList__ )
#define __TDirList__

class TDirList:public TCollection
{
public:
  TDirList(void):TCollection(10, 1)
  {
  }
  TDirList(const char *);
  void FromString(const char *);
  void ToString(char *&, const char *sep = " ");

  TDirList & operator = (const TDirList &);
#if !defined( NO_STREAM )
private:
  virtual void *readItem(ipstream &);
  virtual void writeItem(void *, opstream &);

  STREAMIO(TDirList)
#endif // NO_STREAM
};

#if !defined( NO_STREAM )
STREAMINLINES(TDirList)
#endif // NO_STREAM
#endif
