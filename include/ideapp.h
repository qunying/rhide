/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifndef __IDEAPP_H__
#define __IDEAPP_H__

class TPalette;

class IDE:public TApplication
{
public:
  IDE();
  virtual TCEditWindow *openEditor(char *fileName, Boolean visible);
  virtual TPalette & getPalette() const;
  static TMenuBar *initMenuBar(TRect);
  static TStatusLine *initStatusLine(TRect);
  virtual void handleEvent(TEvent &);
  virtual void getEvent(TEvent &);
  virtual void update(void);
  virtual void fileOpen();
  virtual void idle();
  virtual void changeBounds(const TRect & bounds);
  void createClipBoard(void);
protected:
    virtual void DosShell();
  virtual void resume();
  void fileNew();
  void ShowClip();
  void Tile();
  void Cascade();
};

unsigned doEditDialog(int dialog, ...);
extern TCEditWindow *clipWindow;

#endif
