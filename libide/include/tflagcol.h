/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TFlagCollection ) && !defined( __TFlagCollection__ )
#define __TFlagCollection__

class TFlagCollection:public TCollection
{
public:
  TFlagCollection():TCollection(10, 10)
  {
  }
  static const char *const name;
  static TStreamable *build();
  void ToString(char *&, const char *sep = " ");

private:
  virtual const char *streamableName() const
  {
    return name;
  }
  virtual void *readItem(ipstream &);
  virtual void writeItem(void *, opstream &);
  virtual void freeItem(void *item);

protected:
TFlagCollection(StreamableInit):TCollection(streamableInit)
  {
  }
public:
  TFlagCollection & operator = (const TFlagCollection &);
};

inline ipstream & operator >> (ipstream & is, TFlagCollection & cl)
{
  return is >> (TStreamable &) cl;
}

inline ipstream & operator >> (ipstream & is, TFlagCollection * &cl)
{
  return is >> (void *&) cl;
}

inline opstream & operator << (opstream & os, TFlagCollection & cl)
{
  return os << (TStreamable &) cl;
}

inline opstream & operator << (opstream & os, TFlagCollection * cl)
{
  return os << (TStreamable *) cl;
}

#endif
