/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TEvent
#define Uses_TStreamableClass
#define Uses_MsgBox
#define Uses_TKeys
#define Uses_TIDEEditWindow
#define Uses_TFileDialog
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_MsgBox
#define Uses_editCommands

#define Uses_TSIndicator
#define Uses_TIDEFileEditor
#define Uses_ideCommands
#define Uses_ideFunctions
#include <libide.h>

#define Uses_TCEditor_Commands
#include <ceditor.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <rhutils.h>

extern int SHLSelect(TCEditor & e, char *buffer, int buf_len);

TIDEFileEditor::~TIDEFileEditor()
{
  string_free(bname);
}

int
  TIDEFileEditor::use_syntax = 1;

#if (TCEDITOR_VERSION < 0x000452)
int (*TIDEFileEditor::externalFormatLine) (TCEditor *, void *, unsigned, int,
                                           unsigned short, unsigned, uint32,
                                           unsigned) =
#else
int (*TIDEFileEditor::externalFormatLine) (TCEditor *, void *, unsigned, int,
                                           unsigned short, unsigned, uint32,
                                           unsigned, uint32 *) =
#endif
  NULL;

void
#if (TCEDITOR_VERSION < 0x000452)
TIDEFileEditor::formatLine(void *DrawBuf,
                           unsigned LinePtr, int Width, unsigned short Colors, 
			   unsigned lineLen, uint32 Attr, unsigned lineNo	// needed for RHIDE
#else
TIDEFileEditor::formatLine(void *DrawBuf,
                           unsigned LinePtr, int Width, unsigned short Colors, 
			   unsigned lineLen, uint32 Attr, unsigned lineNo	// needed for RHIDE
			   , uint32 *colMarkers
#endif
  )
{
#if (TCEDITOR_VERSION < 0x000452)
  if (externalFormatLine &&
      externalFormatLine(this, DrawBuf, LinePtr, Width, Colors, lineLen,
                         Attr, lineNo)) return;
  (this->*FormatLinePtr) (DrawBuf, LinePtr, Width, Colors, lineLen, Attr, lineNo);
#else
  if (externalFormatLine &&
      externalFormatLine(this, DrawBuf, LinePtr, Width, Colors, lineLen,
                         Attr, lineNo, colMarkers)) return;
  (this->*FormatLinePtr) (DrawBuf, LinePtr, Width, Colors, lineLen, Attr, lineNo, colMarkers);
#endif
}


void
TIDEFileEditor::setFormatLine()
{
  if (bname)
    string_free(bname);
  BaseName(fileName, bname);
  if (!use_syntax)
    SetHighlightTo(shlNoSyntax);
  else
    SHLSelect(*this, buffer, bufLen);
  FormatLinePtr = formatLinePtr;
#if (TCEDITOR_VERSION < 0x000452)
  formatLinePtr = (void (TCEditor::*)
                   (void *, unsigned, int, unsigned short, unsigned,
                    uint32, unsigned)) &TIDEFileEditor::formatLine;
#else
  formatLinePtr = (void (TCEditor::*)
                   (void *, unsigned, int, unsigned short, unsigned,
                    uint32, unsigned, uint32 *)) &TIDEFileEditor::formatLine;
#endif
  update(ufView);
}

TIDEFileEditor::TIDEFileEditor(const TRect & rect, TScrollBar * ahscrollbar,
                               TScrollBar * avscrollbar,
                               TSIndicator * aindicator,
                               const char *aFileName):
TCFileEditor(rect, ahscrollbar, avscrollbar, aindicator, aFileName)
{
  struct stat st;

  FormatLinePtr = formatLinePtr;	// is set by TCEditor::TCEditor
  if (aFileName == 0)
    fileName[0] = EOS;
  else
  {
    strcpy(fileName, aFileName);
    if (isValid)
      isValid = loadFile();
  }
  if (*fileName && stat(fileName, &st) == 0)
  {
    edittime = st.st_mtime;
#if 0
    if (!(st.st_mode & S_IWUSR))
#else
    if (access(fileName, W_OK) != 0)
#endif
    {
      messageBox(mfWarning | mfOKButton,
                 _("Warning: %s is write protected"), fileName);
    }
  }
  else
  {
    struct timeb tb;

    ftime(&tb);
#ifdef __DJGPP__
    edittime = tb.time & ~(1UL);	//round up to a 2 sec boundary
#endif
  }
  helpCtx = hcIDEFileEditor;
  BaseName(fileName, bname);
  setFormatLine();
}

TStreamable *
TIDEFileEditor::build()
{
  return new TIDEFileEditor(streamableInit);
}

TIDEFileEditor::TIDEFileEditor(StreamableInit):
TCFileEditor(streamableInit), bname(NULL)
{
}

void
TIDEFileEditor::handleEvent(TEvent & event)
{
  int savecmd = 0;
  int save_as = 0;
  Boolean old_modified = modified;

  if (event.what == evCommand &&
      event.message.command == cmSaveEditor && modified == False)
  {
    clearEvent(event);
    return;
  }
  if (event.what == evCommand &&
      (event.message.command == cmSaveEditor ||
       event.message.command == cmcSaveAs))
  {
    savecmd = 1;
    if (event.message.command == cmcSaveAs || !*fileName)
      save_as = 1;
  }
  else
    savecmd = 0;
//  lock();
  if (savecmd && event.message.command == cmSaveEditor)
    event.message.command = cmcSave;
  if (!savecmd)
    modified = False;           // when not saving, check for new modification
  TCFileEditor::handleEvent(event);
  if (event.what == evCommand)  // the event was not handled
  {
    if (!savecmd)
    {
      if (old_modified != modified)
      {
        modified = old_modified;
        indicator->setValue(curPos, modified);
      }
    }
    return;
  }
  switch (event.what)
  {
    case evBroadcast:
      switch (event.message.command)
      {
        case cmTurnSyntaxOff:
          use_syntax = 0;
          setFormatLine();
          break;                // do not clear the event
        case cmTurnSyntaxOn:
          use_syntax = 1;
          setFormatLine();
          break;                // do not clear the event
        default:
          break;
      }
      break;
    default:
      break;
  }
  if (savecmd)
    setFormatLine();            // may be the name has changed
  if (savecmd && modified == False)	// saving was successfull
  {
    struct utimbuf ut;

    ut.actime = ut.modtime = edittime;
    utime(fileName, &ut);
    if (save_as)                // send a message, that the filename has changed
      message(TProgram::application, evBroadcast, cmEditorFilenameChanged,
              this);
    // clear all undo information ???
  }
  if (modified == True && !savecmd)	// it was a real modification
  {
    struct timeb tb;

    ftime(&tb);
    edittime = tb.time & ~(1UL);	//round up to a 2 sec boundary
    message(TProgram::application, evBroadcast, cmEditorModified,
            (void *) edittime);
  }
  if (!savecmd && old_modified == True && modified == False)
  {
    modified = old_modified;
    indicator->setValue(curPos, modified);
  }
  if (event.what == evBroadcast && event.message.command == cmRedraw)
  {
    if (exposed())
      update(ufView);
  }
//  unlock();
}

Boolean
TIDEFileEditor::valid(ushort command)
{
  Boolean retval;
  struct utimbuf ut;

  if (command == cmValid)
    return isValid;
  if (command != cmClose)
    return True;
  if (modified == True)
  {
    int d;

    if (*fileName == EOS)
      d = edSaveUntitled;
    else
      d = edSaveModify;

    switch (editorDialog(d, fileName))
    {
      case cmYes:
        retval = save();
        if (retval == True)
        {
          ut.actime = ut.modtime = edittime;
          utime(fileName, &ut);
        }
        return retval;
      case cmNo:
        modified = False;
        return True;
      case cmCancel:
        return False;
    }
  }
  return True;
}

void
goto_line(TIDEFileEditor * editor, int line, int column)
{
  if (line < 1)
    return;
  if (column < 1)
    column = 1;
  editor->MoveCursorTo(column - 1, line - 1);
  editor->trackCursor(True);
  editor->GoAndSelectLine(line);
  editor->MoveCursorTo(column - 1, line - 1);
}

static ushort TIDEFileEditor_Version = 0;

void
TIDEFileEditor::write(opstream & os)
{
  TCFileEditor::write(os);
  os << TIDEFileEditor_Version;
}

void *
TIDEFileEditor::read(ipstream & is)
{
  struct stat st;
  ushort version;

  TCFileEditor::read(is);
  is >> version;
  if (stat(fileName, &st) == 0)
  {
    edittime = st.st_mtime;
#if 0
    if (!(st.st_mode & S_IWUSR))
#else
    if (access(fileName, W_OK) != 0)
#endif
    {
      messageBox(mfWarning | mfOKButton,
                 _("Warning: %s is write protected"), fileName);
    }
  }
  else
  {
    struct timeb tb;

    ftime(&tb);
    edittime = tb.time & ~(1UL);	//round up to a 2 sec boundary
  }
  BaseName(fileName, bname);
  FormatLinePtr = formatLinePtr;	// is set by TCFileEditor::read
  setFormatLine();
  helpCtx = hcIDEFileEditor;
  return this;
}
