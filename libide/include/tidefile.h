/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TIDEFileEditor ) && !defined( __TIDEFileEditor__ )
#define __TIDEFileEditor__

class TIDEFileEditor : public TCFileEditor
{
public:
  TIDEFileEditor(const TRect &,TScrollBar *,TScrollBar *,TIndicator *,const char *);
  ~TIDEFileEditor();
  static TStreamable * build();
  static const char * const name;
  virtual void handleEvent(TEvent &);
  virtual Boolean valid(ushort);
  static int (*externalFormatLine)(TCEditor *,void *, unsigned, int,
                                   unsigned short, unsigned, unsigned short,
                                   unsigned );
  void formatLine(void *, unsigned, int, unsigned short, unsigned, unsigned short, unsigned );
  void setFormatLine();
  void (TCEditor::*FormatLinePtr)(void *, unsigned, int, unsigned short, unsigned,
                                  unsigned short, unsigned );
private:
  virtual const char * streamableName() const { return name; }

protected:

  TIDEFileEditor(StreamableInit);
  virtual void * read(ipstream &);
  virtual void write( opstream& );

public:
  unsigned long edittime;
  char *bname;
  static int use_syntax;
};

inline ipstream& operator >> ( ipstream& is, TIDEFileEditor& cl )
    { return is >> (TStreamable&)cl; }
inline ipstream& operator >> ( ipstream& is, TIDEFileEditor*& cl )
    { return is >> (void *&)cl; }

inline opstream& operator << ( opstream& os, TIDEFileEditor& cl )
    { return os << (TStreamable&)cl; }
inline opstream& operator << ( opstream& os, TIDEFileEditor* cl )
    { return os << (TStreamable *)cl; }

#endif
