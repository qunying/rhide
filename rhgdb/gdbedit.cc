/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#define Uses_TEvent
#define Uses_TWindow
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TKeys
#define Uses_TPalette
#define Uses_TDeskTop
#define Uses_TFileDialog
#define Uses_MsgBox
#define Uses_TStringCollection
#include <tv.h>

#define Uses_TCEditor
#define Uses_TCEditWindow
#include <ceditor.h>

#define Uses_TDirList
#define Uses_tvutilFunctions
#define Uses_TWindowList
#define Uses_TSCollection
#include <libtvuti.h>

#include <rhutils.h>

#include <librhgdb.h>

#include <rhgdb.h>

#include <unistd.h>
#include <string.h>

#define cNormal   1
#define cMarked   2
#define cComment  3
#define cReserved 4
#define cIdent    5
#define cSymbol   6
#define cString   7
#define cInteger  8
#define cFloat    9
#define cOctal    10
#define cHex      11
#define cChar     12
#define cPre      13
#define cIllegal  14
#define cUser     15
#define cCPU      16
#define cBreak    17
#define cSymbol2  18

uint32 CPULine;
TCEditor *current_editor;

TStringCollection *ReservedWords;
TStringCollection *UserWords;
TStringCollection *PascalRWords;
TStringCollection *ClipperRWords;

class TGDBEditor : public TCEditor
{
public:
  TGDBEditor(const TRect &,TScrollBar *,TScrollBar *,TIndicator *,const char *);
  virtual void handleEvent(TEvent &event);
  static int (*externalFormatLine)(TCEditor *,void *, unsigned, int,
                                   unsigned short, unsigned, unsigned short,
                                   unsigned );
  void formatLine(void *, unsigned, int, unsigned short, unsigned, unsigned short, unsigned );
  void setFormatLine();
  void (TCEditor::*FormatLinePtr)(void *, unsigned, int, unsigned short, unsigned,
                                  unsigned short, unsigned );
  ~TGDBEditor();
  char *bname;
};

class TGDBEditWindow : public TCEditWindow
{
public:
  TGDBEditWindow(const TRect &,const char *,int);
  virtual TPalette & getPalette() const;
};

#define cpBlueEditWindow "\x40\x41\x42\x43\x44\x45\x46\x47"\
                         "\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F"\
                         "\x50\x51\x52\x53\x54\x55\x56\x57"\
                         "\x58\x59\x5A"
#define cpCyanEditWindow "\x60\x61\x62\x63\x64\x65\x66\x67"\
                         "\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F"\
                         "\x70\x71\x72\x73\x74\x75\x76\x77"\
                         "\x78\x79\x7A"
#define cpGrayEditWindow "\x80\x81\x82\x83\x84\x85\x86\x87"\
                         "\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F"\
                         "\x90\x91\x92\x93\x94\x95\x96\x97"\
                         "\x98\x99\x9A"

TPalette & TGDBEditWindow::getPalette() const
{
  static TPalette blue(cpBlueEditWindow,sizeof(cpBlueEditWindow)-1);
  static TPalette cyan(cpCyanEditWindow,sizeof(cpCyanEditWindow)-1);
  static TPalette gray(cpGrayEditWindow,sizeof(cpGrayEditWindow)-1);
  static TPalette * palettes[] =
  {
    &blue,
    &cyan,
    &gray
  };
  return *(palettes[palette]);
}

TGDBEditWindow::TGDBEditWindow( const TRect& bounds,
                          const char *fileName,
                          int aNumber
                        ) :
    TCEditWindow( bounds, (const char *)NULL, aNumber ),
    TWindowInit( &TGDBEditWindow::initFrame )
{
  TRect r(editor->origin.x,editor->origin.y,
  editor->origin.x+editor->size.x,editor->origin.y+editor->size.y);
  TScrollBar * hScrollBar = editor->hScrollBar;
  TScrollBar * vScrollBar = editor->vScrollBar;
  TIndicator * indicator = editor->indicator;
  editor->hScrollBar = NULL;
  editor->vScrollBar = NULL;
  editor->indicator = NULL;
  remove(editor);
  destroy(editor);
//  hScrollBar->options |= ofPreProcess;
//  vScrollBar->options |= ofPreProcess;
  editor = new TGDBEditor( r, hScrollBar, vScrollBar, indicator, fileName );
  insert(editor);
//  helpCtx = hcIDEEditWindow;
  options |= ofFirstClick;
}

TGDBEditor::TGDBEditor(const TRect & rect,TScrollBar *ahscrollbar,
				TScrollBar *avscrollbar,TIndicator *aindicator,
				const char *aFileName) :
  TCEditor(rect,ahscrollbar,avscrollbar,aindicator,aFileName)
{
  BaseName(fileName,bname);
  setFormatLine();
}

TGDBEditor::~TGDBEditor()
{
  string_free(bname);
}

void TGDBEditor::handleEvent(TEvent &event)
{
  switch (event.what)
  {
    case evMouseDown:
    case evMouseUp:
    case evMouseAuto:
    case evMouseMove:
      TCEditor::handleEvent(event);
      return;
    case evKeyDown:
      switch (event.keyDown.keyCode)
      {
        case kbUp:
        case kbDown:
        case kbLeft:
        case kbRight:
        case kbPgUp:
        case kbPgDn:
        case kbEnd:
        case kbHome:
        case kbCtrlLeft:
        case kbCtrlRight:
        case kbCtrlPgDn:
        case kbCtrlPgUp:
        case kbCtrlEnd:
        case kbCtrlHome:
          break;
        default:
          return;
      }
      break;
    case evBroadcast:
      switch (event.message.command)
      {
        case cmEditorAnswer:
          clearEvent(event);
          return;
        default:
          break;
      }
    default:
      break;
  }
  TCEditor::handleEvent(event);
}

static
int DebuggerFormatLine(TCEditor *editor,
                       void *DrawBuf,
                       unsigned LinePtr,
                       int Width,
                       unsigned short Colors,
                       unsigned lineLen,
                       unsigned short Attr,
                       unsigned LineNo)
{
#define drawbuf ((ushort *)DrawBuf)
  uint32 offset = 0;
  ushort color;
  if (debugger_started && editor == current_editor &&
      CPULine == LineNo)
  {
     editor->formatLine(DrawBuf,LinePtr,Width,Colors,lineLen,Attr,LineNo);
     color = editor->getColor(cCPU) << 8;
     while (Width--)
     {
       drawbuf[offset] &= 0x00FF;
       drawbuf[offset++] |= color;
     }
     return 1;
  }
  if (IsBreakPointLine(((TGDBEditor *)editor)->bname,LineNo+1) >= 0)
  {
     editor->formatLine(DrawBuf,LinePtr,Width,Colors,lineLen,Attr,LineNo);
     color = editor->getColor(cBreak) << 8;
     while (Width--)
     {
       drawbuf[offset] &= 0x00FF;
       drawbuf[offset++] |= color;
     }
     return 1;
  }
  return 0;
}

int (*TGDBEditor::externalFormatLine)(TCEditor *,void *, unsigned, int,
                                   unsigned short, unsigned, unsigned short,
                                   unsigned ) = NULL;

void TGDBEditor::formatLine( void *DrawBuf,
			  unsigned LinePtr,
			  int Width,
			  unsigned short Colors,
                          unsigned lineLen,
                          unsigned short Attr,
                          unsigned lineNo // needed for RHIDE
			)
{
  if (DebuggerFormatLine(this,DrawBuf,LinePtr,Width,Colors,lineLen,
                         Attr,lineNo)) return;
  FormatLinePtr(DrawBuf,LinePtr,Width,Colors,lineLen,Attr,lineNo);
}


void TGDBEditor::setFormatLine()
{
  extern int SHLSelectByExtention(TCEditor &e);
  SHLSelectByExtention(*this);
  FormatLinePtr = formatLinePtr;
  formatLinePtr = (void (TCEditor::*)
                  (void *, unsigned, int, unsigned short, unsigned,
                   unsigned short, unsigned ))&formatLine;
  update(ufView);
}

void open_editor(const char *fname)
{
  TGDBEditWindow *window = new TGDBEditWindow(TRect(1,1,79,20),fname,0);
  AddWindow(window);
}


char *ExpandFileNameToThePointWhereTheProgramWasLoaded(const char *s)
{
  static char buffer[PATH_MAX];
  if (__file_exists(s))
  {
    strcpy(buffer,s);
    return buffer;
  }
#ifndef __DJGPP__
  // now try the home directory
  char *home = getenv("HOME");
  if (home)
  {
    strcpy(buffer,home);
    strcat(buffer,"/");
    strcat(buffer,s);
    if (__file_exists(buffer)) return buffer;
  }
  // now try in some standard places
  strcpy(buffer,"/usr/local/share/rhide/");
  strcat(buffer,s);
  if (__file_exists(buffer)) return buffer;
  strcpy(buffer,"/usr/share/rhide/");
  strcat(buffer,s);
  if (__file_exists(buffer)) return buffer;
  strcpy(buffer,"/local/share/rhide/");
  strcat(buffer,s);
  if (__file_exists(buffer)) return buffer;
  strcpy(buffer,"/share/rhide/");
  strcat(buffer,s);
  if (__file_exists(buffer)) return buffer;
#else
  char *dj = getenv("DJDIR");
  if (dj)
  {
    strcpy(buffer,dj);
    strcat(buffer,"/share/rhide/");
    strcat(buffer,s);
    if (__file_exists(buffer)) return buffer;
  }
#endif
#if 0
  strcpy(buffer,RHIDE_DIR);
  strcat(buffer,s);
#else
  strcpy(buffer,s);
#endif
  return buffer;
}

static char *find_buffer = NULL;

static char *find_file(const char *fname,char * & bname,Boolean & found)
{
  int i,count;
  string_free(find_buffer);
  string_dup(find_buffer,fname);
  found = True;
  BaseName(fname,bname);
  if (__file_exists(find_buffer)) return find_buffer;
  count = src_dirs->getCount();
  for (i=0;i<count;i++)
  {
    string_free(find_buffer);
    find_buffer = expand_spec((const char *)src_dirs->at(i),NULL);
    string_cat(find_buffer,"/");
    string_cat(find_buffer,bname);
    if (__file_exists(find_buffer)) return find_buffer;
  }
  string_free(find_buffer);
  string_dup(find_buffer,fname);
  found = False;
  return find_buffer;
}    
  
int FindFile(const char *name,char *&retval)
{
  Boolean dummy;
  find_file(name,retval,dummy);
  return dummy == True;
}

void OpenViewer(char *_fname,int line,Boolean from_debugger,
                Boolean only_focus)
{
  int i,count;
  Boolean found;
  char *_bname,*bname;
  char *fname = find_file(_fname,_bname,found);
  char full_name[512];
  TCEditor *editor;
  bname = (char *)alloca(strlen(_bname)+1);
  strcpy(bname,_bname);
  string_free(_bname);
  TWindow *window = NULL;
  TView *v = TProgram::deskTop->current;
  if (windows)
  {
    count = windows->getCount();
    for (i=0;i<count;i++)
    {
      DeskTopWindow *w = (DeskTopWindow *)windows->at(i);
      if (strcmp(w->full_name,fname) == 0 ||
          strcmp(w->base_name,bname) == 0)
      {
        window = DESKTOPWINDOW(i);
        found = True;
        break;
      }
    }
  }
  if (found == False)
  {
    ushort result;
    if (only_focus == True) return;
    messageBox(mfError | mfOKButton,_("Could not find the source "
                                      "file %s."),bname);
    TFileDialog *dialog;
    dialog = new TFileDialog(bname,_("Open a file"),
                           _("~N~ame"),fdOpenButton,0);
    result = TProgram::deskTop->execView(dialog);
    if (result != cmCancel)
    {
      dialog->getData(full_name);
      fname = full_name;
      FExpand(fname,False);
      char *dir,*name,*ext;
      split_fname(fname,dir,name,ext);
      string_free(name);
      string_free(ext);
      if (*dir) dir[strlen(dir)-1] = 0;
      if (messageBox(mfConfirmation | mfYesNoCancel,
                     _("Add '%s' to the search path for source files"),
                     dir) == cmYes)
      {
        src_dirs->insert(strdup(dir));
      }
      string_free(dir);
    }
    else
    {
      return;
    }
  }
  if (!window)
  {
    TRect r = TProgram::deskTop->getExtent();
    r.b.y -= 7;
    window = new TGDBEditWindow(r,fname,0);
    AddWindow(window);
  }
  editor = ((TGDBEditWindow *)window)->editor;
  if (from_debugger == True)
  {
    current_editor = editor;
    CPULine = line-1;
  }
  TProgram::deskTop->lock();
  editor->MoveCursorTo(0,line-1);
  editor->GoAndSelectLine(line);
  editor->trackCursor(False);
  if (editor->cursor.y == 0 ||
      editor->cursor.y == editor->size.y-1)
    editor->trackCursor(True);
  editor->update(ufView);
  window->select();
  if (only_focus == True)
    if (v) v->select();
  TProgram::deskTop->unlock();
}

void CenterCursor()
{
  TEvent event;
  if (!TProgram::deskTop->current) return;
  event.what = evBroadcast;
  event.message.command = cmEditorAnswer;
  TProgram::deskTop->current->handleEvent(event);
  if (event.what != evNothing) return;
#define E ((TCEditWindow *)TProgram::deskTop->current)->editor
  E->trackCursor(True);
#undef E
}

char *RHGDBWordUnderCursor(void)
{
  char *word;
  TEvent event;
  if (!TProgram::deskTop->current) return NULL;
  event.what = evBroadcast;
  event.message.command = cmEditorAnswer;
  TProgram::deskTop->current->handleEvent(event);
  if (event.what != evNothing) return NULL;
#define E ((TCEditWindow *)TProgram::deskTop->current)->editor
  word = E->WordUnderCursor();
  if (!word) // try to get the word before the cursor
  {
    int x = E->curPos.x;
    if (x > 0)
    {
      E->MoveCursorTo(x-1,E->curPos.y);
      word = E->WordUnderCursor();
      E->MoveCursorTo(x,E->curPos.y);
    }
  }
#undef E
  return word;
}

char *WhereIsCursor(int &line,int &column,char *&bname)
{
  TEvent event;
  if (!TProgram::deskTop->current) return NULL;
  event.what = evBroadcast;
  event.message.command = cmEditorAnswer;
  TProgram::deskTop->current->handleEvent(event);
  if (event.what != evNothing) return NULL;
#define E ((TCEditWindow *)TProgram::deskTop->current)->editor
  line = E->curPos.y+1;
  column = E->curPos.x+1;
  bname = ((TGDBEditor *)E)->bname;
  return bname;
#undef E
}

void OpenFileFromEditor(char *fullName)
{
  open_editor(fullName);
}

