/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TIDEEditWindow ) && !defined( __TIDEEditWindow__ )
#define __TIDEEditWindow__

class TPalette;

class TIDEEditWindow:public TCEditWindow
{
public:
  TIDEEditWindow(const TRect &, const char *, int);
   ~TIDEEditWindow();
  static TStreamable *build();
  static const char *const name;
  virtual TPalette & getPalette() const;
  virtual void handleEvent(TEvent &);
  const char *getTitle(short);
private:
    virtual const char *streamableName() const
  {
    return name;
  }
  char *wtitle;
protected:
    TIDEEditWindow(StreamableInit);
};

inline ipstream & operator >> (ipstream & is, TIDEEditWindow & cl)
{
  return is >> (TStreamable &) cl;
}

inline ipstream & operator >> (ipstream & is, TIDEEditWindow * &cl)
{
  return is >> (void *&) cl;
}

inline opstream & operator << (opstream & os, TIDEEditWindow & cl)
{
  return os << (TStreamable &) cl;
}

inline opstream & operator << (opstream & os, TIDEEditWindow * cl)
{
  return os << (TStreamable *) cl;
}

#endif
