/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TIDEFileEditor ) && !defined( __TIDEFileEditor__ )
#define __TIDEFileEditor__

class TSIndicator;

class TIDEFileEditor:public TCFileEditor
{
public:
  TIDEFileEditor(const TRect &, TScrollBar *, TScrollBar *, TSIndicator *,
                 const char *);
  ~TIDEFileEditor();
  static TStreamable *build();
  static const char *const name;
  virtual void handleEvent(TEvent &);
  virtual Boolean valid(ushort);
#if (TCEDITOR_VERSION < 0x000452)
  static int (*externalFormatLine) (TCEditor *, void *, unsigned, int,
                                    unsigned short, unsigned, uint32,
                                    unsigned);
  void formatLine(void *, unsigned, int, unsigned short, unsigned, uint32,
                  unsigned);
  void (TCEditor::*FormatLinePtr) (void *, unsigned, int, unsigned short,
                                   unsigned, uint32, unsigned);
#else
  static int (*externalFormatLine) (TCEditor *, void *, unsigned, int,
                                    unsigned short, unsigned, uint32,
                                    unsigned, uint32 *);
  void formatLine(void *, unsigned, int, unsigned short, unsigned, uint32,
                  unsigned, uint32 *);
  void (TCEditor::*FormatLinePtr) (void *, unsigned, int, unsigned short,
                                   unsigned, uint32, unsigned, uint32 *);
#endif
  void setFormatLine();
private:
    virtual const char *streamableName() const
  {
    return name;
  }

protected:

    TIDEFileEditor(StreamableInit);
  virtual void *read(ipstream &);
  virtual void write(opstream &);

public:
  unsigned long edittime;
  char *bname;
  static int use_syntax;
};

inline ipstream & operator >> (ipstream & is, TIDEFileEditor & cl)
{
  return is >> (TStreamable &) cl;
}

inline ipstream & operator >> (ipstream & is, TIDEFileEditor * &cl)
{
  return is >> (void *&) cl;
}

inline opstream & operator << (opstream & os, TIDEFileEditor & cl)
{
  return os << (TStreamable &) cl;
}

inline opstream & operator << (opstream & os, TIDEFileEditor * cl)
{
  return os << (TStreamable *) cl;
}

#endif
