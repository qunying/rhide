/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TIDEEditWindow
#define Uses_TIDEFileEditor
#define Uses_TScrollBar
#define Uses_TIndicator
#define Uses_TStreamableClass
#define Uses_TPalette
#define Uses_TEvent
#define Uses_TFrame
#define Uses_ideCommands
#include <libide.h>

#include <rhutils.h>

#define cpBlueEditWindow "\x40\x41\x42\x43\x44\x45\x46\x47"\
                         "\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F"\
                         "\x50\x51\x52\x53\x54\x55\x56\x57"\
                         "\x58\x59\x5A\x5B\x5C\x5D\x5E\x5F"
#define cpCyanEditWindow "\x60\x61\x62\x63\x64\x65\x66\x67"\
                         "\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F"\
                         "\x70\x71\x72\x73\x74\x75\x76\x77"\
                         "\x78\x79\x7A\x7B\x7C\x7D\x7E\x7F"
#define cpGrayEditWindow "\x80\x81\x82\x83\x84\x85\x86\x87"\
                         "\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F"\
                         "\x90\x91\x92\x93\x94\x95\x96\x97"\
                         "\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F"

TPalette & TIDEEditWindow::getPalette() const
{
  static TPalette
  blue(cpBlueEditWindow, sizeof(cpBlueEditWindow) - 1);
  static TPalette
  cyan(cpCyanEditWindow, sizeof(cpCyanEditWindow) - 1);
  static TPalette
  gray(cpGrayEditWindow, sizeof(cpGrayEditWindow) - 1);
  static TPalette *
    palettes[] = {
    &blue,
    &cyan,
    &gray
  };

  return *(palettes[palette]);
}

TIDEEditWindow::~TIDEEditWindow()
{
  string_free(wtitle);
}

const char *
TIDEEditWindow::getTitle(short maxval)
{
  string_free(wtitle);
  if (editor->modified)
    string_cat(wtitle, "*");
  string_cat(wtitle, TCEditWindow::getTitle(maxval));
  return wtitle;
}

TIDEEditWindow::TIDEEditWindow(const TRect & bounds,
const char *fileName, int aNumber):
TCEditWindow(bounds, (const char *) NULL, aNumber),
TWindowInit(&TIDEEditWindow::initFrame), wtitle(NULL)
{
  TRect r(editor->origin.x, editor->origin.y,
          editor->origin.x + editor->size.x,

          editor->origin.y + editor->size.y);
  TScrollBar *hScrollBar = editor->hScrollBar;
  TScrollBar *vScrollBar = editor->vScrollBar;
  TSIndicator *indicator = editor->indicator;

  editor->hScrollBar = NULL;
  editor->vScrollBar = NULL;
  editor->indicator = NULL;
  remove(editor);
  destroy(editor);
  editor = new TIDEFileEditor(r, hScrollBar, vScrollBar, indicator, fileName);
  insert(editor);
  helpCtx = hcIDEEditWindow;
  options |= ofFirstClick;
}

TStreamable *
TIDEEditWindow::build()
{
  return new TIDEEditWindow(streamableInit);
}

TIDEEditWindow::TIDEEditWindow(StreamableInit):
TCEditWindow(streamableInit), TWindowInit(NULL), wtitle(NULL)
{
}

void
TIDEEditWindow::handleEvent(TEvent & event)
{
  switch (event.what)
  {
    case evBroadcast:
      switch (event.message.command)
      {
        case cmEditorAnswer:
          clearEvent(event);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  if (event.what != evNothing)
    TCEditWindow::handleEvent(event);
}
