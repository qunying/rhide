/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_ifpstream
#define Uses_ofpstream
#define Uses_MsgBox
#define Uses_TScreen
#define Uses_TDialog
#define Uses_TListBox
#define Uses_TDeskTop
#define Uses_TProgram
#define Uses_TScrollBar
#define Uses_TFileDialog
#define Uses_TFileInputLine
#define Uses_TFileList
#define Uses_TKeys
#define Uses_TProject
#define Uses_TDepCollection
#define Uses_TWindowList
#define Uses_TOptions
#define Uses_TProjectWindow
#define Uses_TProjectListBox
#define Uses_TDirList
#define Uses_TParamList
#define Uses_IDEConst
#define Uses_TIDEEditWindow
#define Uses_editCommands
#define Uses_TIDEFileEditor
#define Uses_TStringCollection
#define Uses_TPalette
#define Uses_TEventQueue
#define Uses_TStreamableClass

/*
  For old desktop files I need special TSTreamableClass
  Variables.
*/
#define Uses_TSIndicator
#define Uses_TNoCaseStringCollection
#define Uses_TStringCollectionW

#define Uses_ideCommands
#define Uses_ideFunctions
#include <libide.h>

#define Uses_TSCollection
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#include <edprint.h>

#ifdef INTERNAL_DEBUGGER
#define Uses_tvgdbFunctions
#include <libtvgdb.h>
#include <librhgdb.h>
#endif

#include "rhide.h"
#include "rhidehis.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>

/* I have moved it now here, to reduce the size of
  gpr2mak, beause it is needed only, when reading
  the desktop file and not when only reading the
  project file */
__link(RIDEEditWindow)
/*
  For old desktop files I need special TSTreamableClass
  Variables.
*/
     TStreamableClass _RSIndicator("TSIndicator",

                                   TSIndicator::build, __DELTA(TSIndicator));
     TStreamableClass _RNoCaseStringCollection("TNoCaseStringCollection",
                                               TNoCaseStringCollection::build,
                                               __DELTA

                                               (TNoCaseStringCollection));
     TStreamableClass _RStringCollectionW("TStringCollectionW",
                                          TStringCollectionW::build,

                                          __DELTA(TStringCollectionW));

     TProjectWindow *project_window;
     static char *dskname;

     static ushort DeskTop_Version = 27;
     static ushort BreakPoint_Version = 0;

     static void SetGlobalOptions();

     TRect ProjectWindowRect(-1, -1, -1, -1);
     extern TRect MsgWindowRect;

     char *default_directory;

     static void writeView(TView * p, void *strm)
{
  TEvent event;
  opstream *os = (opstream *) strm;

  event.what = evBroadcast;
  event.message.command = cmEditorAnswer;
  p->handleEvent(event);
  if (event.what == evNothing)
  {
    if (((TIDEEditWindow *) p)->editor->isClipboard() == False)
    {
      *os << p;
      short number = ((TWindow *) p)->number;

      *os << number;
    }
    return;
  }
  event.what = evBroadcast;
  event.message.command = cmInfoAnswer;
  p->handleEvent(event);
  if (event.what == evNothing)
  {
    *os << p;
    short number = ((TWindow *) p)->number;

    *os << number;
    return;
  }
}

/* Now I'm using an other desktop-file format. I indicate it by putting
   0x00 at first byte, which could not be happen with the old format.
*/

static void
SaveDesktop(opstream & os, Boolean save_windows = True)
{
  int n, i;
  ushort w;
  TEvent event;
  uchar len = 0x00;

  os << len;
  os.writeString(DESKTOP_IDENT);
  os << DeskTop_Version;
  int save_pal = TProgram::appPalette;

  TProgram::appPalette = apColor;
  TPalette *palette = &TProgram::application->getPalette();

  len = palette->data[0];
  os << len;
  os.writeBytes(&palette->data[1], len);
  if (project_window)
    len = 1;
  else
    len = 0;
  os << len;
  if (windows)
    n = windows->getCount();
  else
    n = 0;
  w = n;
  if (save_windows == True)
  {
    // count the editor windows on the desktop
    for (i = 0; i < n; i++)
    {
      event.what = evBroadcast;
      event.message.command = cmEditorAnswer;
      DESKTOPWINDOW(i)->handleEvent(event);
      if (event.what == evNothing)
        continue;
      else
      {
        event.what = evBroadcast;
        event.message.command = cmInfoAnswer;
        DESKTOPWINDOW(i)->handleEvent(event);
        if (event.what == evNothing)
          continue;
      }
      w--;
    }
  }
  else
  {
    w = 0;
  }
#ifdef INTERNAL_DEBUGGER
  {
    int tmp_len;
    char *tmp;

    if (save_windows == False)
      n = 0;
    else
      n = Watches.getCount();
    os << n;
    for (i = 0; i < n; i++)
    {
      tmp = (char *) Watches.at(i);
      tmp_len = strlen(tmp);
      os << tmp_len;
      os.writeBytes(tmp, tmp_len);
    }
  }
#else
  n = 0;
  os << n;
#endif
  os << InfRec;
  if (!project_window)
  {
    ccIndex tmp = -1;

    os << tmp;
  }
  else
  {
    ccIndex tmp;

    if (project_window->liste->list()->getCount() == 0)
      tmp = -1;
    else
      tmp = project_window->liste->focused;
    os << tmp;
  }
  os << BreakPoint_Version;
#ifdef INTERNAL_DEBUGGER
  int breakcount;

  if (save_windows == False)
    breakcount = 0;
  else
    breakcount = BreakPointCount();
  os << breakcount;
  for (i = 0; i < breakcount; i++)
  {
    struct BreakPoint *bp = GetBreakPoint(i);

    os << bp->type;
    os << bp->address;
    os.writeString(bp->filename);
    os.writeString(bp->condition);
    os.writeString(bp->function);
    os << bp->count;
    os << bp->line_number;
  }
#else
  int breakcount = 0;

  os << breakcount;
#endif

  SaveHistoryIDs(os);

  os << w;
  if (w > 0)
    TProgram::deskTop->forEach(::writeView, &os);

  TProgram::appPalette = apMonochrome;
  palette = &TProgram::application->getPalette();
  len = palette->data[0];
  os << len;
  os.writeBytes(&palette->data[1], len);
  TProgram::appPalette = apBlackWhite;
  palette = &TProgram::application->getPalette();
  len = palette->data[0];
  os << len;
  os.writeBytes(&palette->data[1], len);
  {
    GlobalOptionsRect temp;
    unsigned u = sizeof(temp);
    os << u;
    TCEditor::CompactGlobalOptions(&temp);
    os.writeBytes(&temp, sizeof(temp));
  }
  TProgram::appPalette = save_pal;

#ifdef INTERNAL_DEBUGGER
  os << WatchWindowRect;
  os << CallStackWindowRect;
#else
  {
    TRect r(-1, -1, -1, -1);

    os << r;
    os << r;
  }
#endif

  {
    unsigned char us = TEventQueue::mouseReverse == True ? 1 : 0;

    os << us;
    os << TEventQueue::doubleDelay;
  }

  {
    os << ProjectWindowRect;
    os << MsgWindowRect;
  }

  os.writeString(default_directory);

  SaveHelpDesktop(os);

  SaveClosedWindows(os);

}

/* They are in tscreen.cc */
void setBlinkState(void);
void setIntenseState(void);

void
SetProjectScreenMode()
{
  if (TScreen::screenMode != Project.screen_mode)
    TProgram::application->setScreenMode(Project.screen_mode);
  if (IntenseMode)
    setIntenseState();
  else
    setBlinkState();
}

static void
LoadDesktop(ipstream & is, Boolean load_windows = True)
{
  int i;
  ushort n;
  uchar len;
  char *pal;
  char *magic;
  ushort version;

#ifdef INTERNAL_DEBUGGER
  if (load_windows == True)
  {
    DeleteAllWatches();
    DeleteAllBreakPoints();
  }
#endif
  ProjectWindowRect = TRect(-1, -1, -1, -1);
  MsgWindowRect = TRect(-1, -1, -1, -1);
#ifdef INTERNAL_DEBUGGER
  WatchWindowRect = TRect(-1, -1, -1, -1);
  CallStackWindowRect = TRect(-1, -1, -1, -1);
#endif
  is >> len;
  if (len != 0)                 // it is an old desktop-file. Simply ignore it
  {
    messageBox(_("The desktop file cannot be used, because of a new format"),
               mfWarning | mfOKButton);
    // but show the project window, if it is a real project
    if (project_name && load_windows == True)
      ShowProject();
    return;
  }
  magic = is.readString();
  if (!is.good())
    return;
  if (strcmp(magic, DESKTOP_IDENT))
    return;
  is >> version;
  if (version < 19)             // applied editor v 0.34
  {
    messageBox(_("The desktop file cannot be used, because of a new format"),
               mfWarning | mfOKButton);
    // but show the project window, if it is a real project
    if (project_name && load_windows == True)
      ShowProject();
    return;
  }
  if (version > DeskTop_Version)
  {
    messageBox(_("The desktop file cannot be used, because it was created "
                 "by a newer version of RHIDE"), mfWarning | mfOKButton);
    // but show the project window, if it is a real project
    if (project_name && load_windows == True)
      ShowProject();
    return;
  }
  int save_pal = TProgram::appPalette;

  TProgram::appPalette = apColor;
  is >> len;
  pal = new char[len + 1];

  is.readBytes(pal, len);
  if (len < TProgram::application->getPalette()[0])
  {
    uchar _len = TProgram::application->getPalette()[0];
    char *_pal = new char[_len + 1];

    memcpy(_pal, pal, len);
    memcpy(_pal + len, &(TProgram::application->getPalette()[len + 1]),
           _len - len);
    delete(pal);
    len = _len;
    pal = _pal;
  }
  TPalette pa(pal, len);

  TProgram::application->getPalette() = pa;
  delete(pal);
  TProgram::appPalette = save_pal;
  SetProjectScreenMode();
  is >> len;
  {
    char *tmp = NULL;
    int tmp_len;
    int tmp_size = 0;
    int count;

    is >> count;
    for (i = 0; i < count; i++)
    {
      is >> tmp_len;
      if (tmp_len > 0)
      {
        if (tmp_len >= tmp_size)
          tmp = (char *) realloc(tmp, (tmp_size = tmp_len + 1));
        is.readBytes(tmp, tmp_len);
        tmp[tmp_len] = 0;
#ifdef INTERNAL_DEBUGGER
        if (load_windows == True)
          AddWatch(tmp, False);
#endif
      }
    }
    if (tmp)
      free(tmp);
  }
  is >> InfRec;
  ccIndex tmp;

  is >> tmp;

  {
    int breakcount;
    ushort b_version;

    is >> b_version;
    is >> breakcount;
    for (i = 0; i < breakcount; i++)
    {
#ifdef INTERNAL_DEBUGGER
      struct BreakPoint bp;

      is >> bp.type;
      is >> bp.address;
      bp.filename = is.readString();
      bp.condition = is.readString();
      bp.function = is.readString();
      is >> bp.count;
      is >> bp.line_number;
      if (load_windows)
        EditBreakPoint(&bp, -1);
      if (bp.filename)
        free(bp.filename);
      if (bp.condition)
        free(bp.condition);
      if (bp.function)
        free(bp.function);
#else
      ushort _ushort;
      int _int;
      unsigned _ulong;
      char *tmp;

      is >> _ushort;
      is >> _ulong;
      tmp = is.readString();
      if (tmp)
        free(tmp);
      tmp = is.readString();
      if (tmp)
        free(tmp);
      tmp = is.readString();
      if (tmp)
        free(tmp);
      is >> _int;
      is >> _int;
#endif
    }
  }

  LoadHistoryIDs(is);

  if (len != 0 && project_name && load_windows == True)
    ShowProject();              // do not show without a project
  if (tmp >= 0 && project_window && load_windows == True)
    project_window->liste->focusItem(tmp);

  {
    is >> n;
    struct
    {
      short number;
      TView *view;
    }
    V[n];

    for (i = 0; i < n; i++)
    {
      is >> V[i].view;
      V[i].number = 0;
      if (version >= 24)
        is >> V[i].number;
    }
    if ((load_windows == True) && (n > 0))
    {
      for (i = n - 1; i >= 0; i--)
      {
        AddWindow((TWindow *) V[i].view, NULL, False, True, True,
                  V[i].number);
        {
          TEvent event;

          event.what = evBroadcast;
          event.message.command = cmEditorAnswer;
          V[i].view->handleEvent(event);
          if (event.what == evNothing)
            /*
               Remove the file from the filetime hash 
             */
            TimeOfFile(((TCEditWindow *) V[i].view)->editor->fileName, True);
        }
      }
    }
  }

  {
    TProgram::appPalette = apMonochrome;
    is >> len;
    pal = new char[len + 1];

    is.readBytes(pal, len);
    if (len < TProgram::application->getPalette()[0])
    {
      uchar _len = TProgram::application->getPalette()[0];
      char *_pal = new char[_len + 1];

      memcpy(_pal, pal, len);
      memcpy(_pal + len, &(TProgram::application->getPalette()[len + 1]),
             _len - len);
      delete(pal);
      len = _len;
      pal = _pal;
    }
    pa = TPalette(pal, len);
    TProgram::application->getPalette() = pa;
    delete(pal);

    TProgram::appPalette = apBlackWhite;
    is >> len;
    pal = new char[len + 1];

    is.readBytes(pal, len);
    if (len < TProgram::application->getPalette()[0])
    {
      uchar _len = TProgram::application->getPalette()[0];
      char *_pal = new char[_len + 1];

      memcpy(_pal, pal, len);
      memcpy(_pal + len, &(TProgram::application->getPalette()[len + 1]),
             _len - len);
      delete(pal);
      len = _len;
      pal = _pal;
    }
    pa = TPalette(pal, len);
    TProgram::application->getPalette() = pa;
    delete(pal);

    TProgram::appPalette = save_pal;
  }

  // force to reread the chached colors for the editor
  TCEditor::colorsCached = 0;

  {
    union
    {
      struct
      {
        uint16 t1;
        char tab[3];
      }
      before_0413;
      struct
      {
        uint16 t1;
        char tab[3];
        char wcol[3];
      } before_0417;
      struct
      {
        uint16 t1;
        char tab[3];
        char wcol[4];
      } before_0445;
      GlobalOptionsRect opt;
    }
    temp;

    if (version < 22)           // applied editor 0.4.13
    {
      is.readBytes(&temp, sizeof(temp.before_0413));
      temp.opt.wcol[0] = '6';
      temp.opt.wcol[1] = '0';
      temp.opt.wcol[2] = 0;
      temp.opt.wcol[3] = 0;
    }
    else if (version < 23)      // applied editor 0.4.17
    {
      is.readBytes(&temp, sizeof(temp.before_0417));
      temp.opt.wcol[3] = 0;
    }
    else if (version < 26)
    {
      is.readBytes(&temp, sizeof(temp.before_0445));
    }
    else
    {
      if (version < 27)
      {
        is >> len;
        if (len == sizeof(GlobalOptionsRect))
        {
          is.readBytes(&temp, len);
          TCEditor::ExpandGlobalOptions(&temp.opt);
        }
        else
        {
          char buf[len];
          is.readBytes(buf, len);
        }
      }
      else
      {
        unsigned l;
        is >> l;
        if (l == sizeof(GlobalOptionsRect))
        {
          is.readBytes(&temp, l);
          TCEditor::ExpandGlobalOptions(&temp.opt);
        }
        else
        {
          char buf[l];
          is.readBytes(buf, l);
        }
      }
    }
  }

  TRect dsk_rect = TProgram::deskTop->getExtent();

#define RectVisible(r) (dsk_rect.a.x < r.b.x && dsk_rect.b.x > r.a.x && \
                        dsk_rect.a.y < r.b.y && dsk_rect.b.y > r.a.y)
#define CheckRect(r) if (!RectVisible(r)) r = TRect(-1,-1,-1,-1)

  {
#ifdef INTERNAL_DEBUGGER
    is >> WatchWindowRect;
    CheckRect(WatchWindowRect);
    is >> CallStackWindowRect;
    CheckRect(CallStackWindowRect);
    if (watchwindow)
      watchwindow->locate(WatchWindowRect);
    if (CallStackWindow)
      CallStackWindow->locate(CallStackWindowRect);
#else
    TRect r;

    is >> r;
    is >> r;
#endif
  }

  {
    unsigned char us;

    is >> us;
    is >> TEventQueue::doubleDelay;
    TEventQueue::mouseReverse = us ? True : False;
  }

  {
    is >> ProjectWindowRect;
    CheckRect(ProjectWindowRect);
    is >> MsgWindowRect;
    CheckRect(MsgWindowRect);
    if (project_window)
      project_window->locate(ProjectWindowRect);
    if (msg_window)
      msg_window->locate(MsgWindowRect);
  }

  string_free(default_directory);
  default_directory = is.readString();

  if (version > 19)
    LoadHelpDesktop(is);

  if (version > 20)
    LoadClosedWindows(is);
}

static void
LoadOptions(char *_name)
{
  char *dir, *name, *ext, *pname, *dname, *__name;
  ifpstream *ifile;
  TProject *_project;

  split_fname(_name, dir, name, ext);
  string_free(ext);
  string_dup(__name, dir);
  string_cat(__name, name);
  string_cat(__name, PROJECT_EXT);
  pname = (char *) alloca(strlen(__name) + 1);
  strcpy(pname, __name);
  string_free(__name);
  string_dup(__name, dir);
  string_cat(__name, name);
  string_cat(__name, DESKTOP_EXT);
  dname = (char *) alloca(strlen(__name) + 1);
  strcpy(dname, __name);
  string_free(__name);
  string_free(dir);
  string_free(name);
  _project = ReadProject(pname);
  if (!_project)
  {
    messageBox(mfError | mfOKButton, _("Invalid project-file %s(%s)"),
               pname, " :-( ");
    return;
  }
  *project = *_project;
  destroy(_project);
  ClearFindCache();
  ifile = open_ifpstream(dname);
  if (ifile && ifile->good())
    LoadDesktop(*ifile, False);
  if (ifile)
    close_ifpstream(ifile);
}

static void
SaveOptions(char *_name)
{
  char *dir, *name, *ext, *pname, *dname, *__name;
  ofpstream *ofile;
  TProject *_project;

  split_fname(_name, dir, name, ext);
  string_free(ext);
  string_dup(__name, dir);
  string_cat(__name, name);
  string_cat(__name, PROJECT_EXT);
  pname = (char *) alloca(strlen(__name) + 1);
  strcpy(pname, __name);
  string_free(__name);
  string_dup(__name, dir);
  string_cat(__name, name);
  string_cat(__name, DESKTOP_EXT);
  dname = (char *) alloca(strlen(__name) + 1);
  strcpy(dname, __name);
  string_free(__name);
  string_free(dir);
  string_free(name);
  _project = new TProject();
  *_project = *project;
  SetMainTargetName("a.exe", _project);
  SaveProject(_project, pname);
  destroy(_project);
  ofile = new ofpstream(dname);
  if (ofile->good())
    SaveDesktop(*ofile, False);
  delete(ofile);
}

char *
select_project(char *title)
{
  ushort result;
  TFileDialog *dialog;
  char *fileName = NULL;

  InitHistoryID(RHIDE_History_project_file);
  dialog = new TFileDialog("*" PROJECT_EXT, title,
                           _("~N~ame"), fdOpenButton,
                           RHIDE_History_project_file);
  TProgram::deskTop->insert(dialog);
  dialog->setState(sfModal, True);
  result = dialog->execute();
  if (result != cmCancel)
  {
    char fname[512];

    dialog->getData(fname);
    string_dup(fileName, fname);
  }
  TProgram::deskTop->remove(dialog);
  destroy(dialog);
  return fileName;
}

void
SaveOptions()
{
  char *name = select_project(_("Save options to a project"));

  if (name)
  {
    SaveOptions(name);
    string_free(name);
  }
}

void
LoadOptions()
{
  char *name = select_project(_("Read options from project"));

  if (name)
  {
    LoadOptions(name);
    string_free(name);
  }
  SetGlobalOptions();
}

static bool
ClearDeskTop(Boolean remove_msg = True)
{
  if (windows)
  {
    int delta = 0;

    while (windows->getCount() > delta)
    {
      TWindow *window = DESKTOPWINDOW(delta);

      if (window != msg_window || remove_msg == True)
      {
        if (window->valid(cmClose) == True)
        {
          RemoveWindow(window, False);
          window->close();
        }
        else
          return false;
      }
      else
      {
        delta++;
      }
    }
  }
  ClearHelpDesktop();
  ClearWindowList();
  return true;
}

void
SetGlobalEditorOptions(TCEditWindow * ew)
{
  if (OverWrite)
    ew->editor->overwrite = True;
  else
    ew->editor->overwrite = False;
  if (AutoIndent)
    ew->editor->autoIndent = True;
  else
    ew->editor->autoIndent = False;
  if (CAutoIndent)
    ew->editor->intelIndent = True;
  else
    ew->editor->intelIndent = False;
  if (UseRealTabs)
    ew->editor->UseTabs = True;
  else
    ew->editor->UseTabs = False;
  if (NoPersistentBlocks)
    ew->editor->PersistentBlocks = False;
  else
    ew->editor->PersistentBlocks = True;
  if (ShowColumnCursor)
    ew->editor->CrossCursorInCol = True;
  else
    ew->editor->CrossCursorInCol = False;
  if (ShowRowCursor)
    ew->editor->CrossCursorInRow = True;
  else
    ew->editor->CrossCursorInRow = False;
  if (TransparentBlocks)
    ew->editor->TransparentSel = True;
  else
    ew->editor->TransparentSel = False;
}

void
SetGlobalEditorOptions()
{
  if (AutoIndent)
    TCEditor::staticAutoIndent = True;
  else
    TCEditor::staticAutoIndent = False;
  if (CAutoIndent)
    TCEditor::staticIntelIndent = True;
  else
    TCEditor::staticIntelIndent = False;
  if (UseRealTabs)
    TCEditor::staticUseTabs = True;
  else
    TCEditor::staticUseTabs = False;
  if (NoPersistentBlocks)
    TCEditor::staticPersistentBlocks = False;
  else
    TCEditor::staticPersistentBlocks = True;
  if (ShowColumnCursor)
    TCEditor::staticCrossCursorInCol = True;
  else
    TCEditor::staticCrossCursorInCol = False;
  if (ShowRowCursor)
    TCEditor::staticCrossCursorInRow = True;
  else
    TCEditor::staticCrossCursorInRow = False;
  if (TransparentBlocks)
    TCEditor::staticTransparentSel = True;
  else
    TCEditor::staticTransparentSel = False;
  if (CreateBackupFiles)
    TCEditor::editorFlags |= efBackupFiles;
  else
    TCEditor::editorFlags &= ~efBackupFiles;
#if 1                           // Apply all the options also to the currently opened
  if (windows != NULL)
  {
    int count = windows->getCount(), i;

    for (i = 0; i < count; i++)
    {
      TCEditWindow *ew = (TCEditWindow *) DESKTOPWINDOW(i);
      TEvent event;

      event.what = evBroadcast;
      event.message.command = cmEditorAnswer;
      ew->handleEvent(event);
      if (event.what == evNothing)
      {
        SetGlobalEditorOptions(ew);
      }
    }
  }
#endif
}

static void
SetGlobalOptions()
{
  if (CreateBackupFiles)
    TCEditor::editorFlags |= efBackupFiles;
  else
    TCEditor::editorFlags &= ~efBackupFiles;
  c_words_changed =
    gpc_words_changed = fpc_words_changed = user_words_changed = 1;
  if (ShowSyntax)
    message(TProgram::application, evBroadcast, cmTurnSyntaxOn, NULL);
  else
    message(TProgram::application, evBroadcast, cmTurnSyntaxOff, NULL);
  Repaint();
#ifdef INTERNAL_DEBUGGER
  if (VerboseGDB)
    verbose_gdb_commands = 1;
  else
    verbose_gdb_commands = 0;
#endif
#ifdef __DJGPP__
  extern int save_text_palette;

  if (SaveTextPalette)
    save_text_palette = 1;
  else
    save_text_palette = 0;
#endif
  max_closed = Project.max_closed_windows;
}

static int
TryStandardProject(const char *dir, const char *name,
                   char *&desktopname, char *&projectname)
{
  string_free(desktopname);
  string_free(projectname);
  string_cat(desktopname, dir, "/", name, DESKTOP_EXT, NULL);
  string_cat(projectname, dir, "/", name, PROJECT_EXT, NULL);
  return __file_exists(projectname);
}

static char *standard_project_name = NULL;

static Boolean
OpenStandardProject(const char *prjname, Boolean with_desktop = True)
{
  char *dir = NULL, *tmp = NULL;

#ifdef INTERNAL_DEBUGGER
  DeleteAllWatches();
  DeleteAllBreakPoints();
#endif
  int found = 0;
  char *try_dir;
  char *try_dirs = expand_rhide_spec("$(RHIDE_CONFIG_DIRS)");
  char *try_dirs_p = try_dirs;

  try_dir = try_dirs;
  while (!found)
  {
    while (*try_dirs_p && (*try_dirs_p != ' '))
      try_dirs_p++;
    if (!*try_dirs_p)
      break;
    *try_dirs_p = 0;
    found = TryStandardProject(try_dir, RHIDE_OPTIONS_NAME, tmp, dir);
    *try_dirs_p = ' ';
    try_dirs_p++;
    try_dir = try_dirs_p;
  }
  string_free(standard_project_name);
  string_free(dskname);
  if ((project = ReadProject(dir)) != NULL)
  {
    standard_project_name = string_dup(dir);
    dskname = string_dup(tmp);
    if (with_desktop == True)
    {
      ifpstream *idfile;

      idfile = open_ifpstream(tmp);
      if (idfile && idfile->good())
        LoadDesktop(*idfile);
      else
      {
        SetProjectScreenMode();
        if (project_name)
          ShowProject();
      }
      if (idfile)
        close_ifpstream(idfile);
    }
    else
    {
      SetProjectScreenMode();
      if (project_name)
        ShowProject();
    }
    string_free(tmp);
    /*
       Check for an invalid project name. We come here, when
       it is already checked, that the project file does not
       exist, so I can try to create an empty file to see, if
       it is possible to create the project 
     */
    {
      ofpstream *of = new ofpstream(prjname);

      if (!of->good())
      {
        delete of;

        SetMainTargetName("aout" STANDARD_EXE_SUFFIX);
        string_free(project_name);
        messageBox(mfError | mfOKButton, _("Couldn't open %s"), prjname);
        string_free(dir);
        return True;            // The standard project was opened
      }
      delete of;

      remove(prjname);
    }
    BaseName(prjname, tmp, False);
    string_cat(tmp, STANDARD_EXE_SUFFIX);
    SetMainTargetName(tmp);
    string_free(tmp);
    string_free(dir);
    return True;
  }
  string_free(tmp);
  string_free(dir);
  if (!project)
  {
    try_dir = expand_rhide_spec("$(GET_HOME)");
    struct stat s;

    if ((stat(try_dir, &s) == 0) &&
        S_ISDIR(s.st_mode) && (s.st_mode & S_IWUSR))
    {
      TryStandardProject(try_dir, RHIDE_OPTIONS_NAME,
                         dskname, standard_project_name);
    }
    else
    {
      string_free(try_dir);
      try_dir = string_dup(".");
      TryStandardProject(try_dir, RHIDE_OPTIONS_NAME,
                         dskname, standard_project_name);
    }
    string_free(try_dir);
  }
  return False;
}

static void
LoadDesktop()
{
  char *dir, *ext;
  ifpstream *idfile;

  if (dskname)
    string_free(dskname);
  split_fname(project_name, dir, dskname, ext);
  string_cat(dskname, DESKTOP_EXT);
  string_free(dir);
  string_free(ext);
  idfile = open_ifpstream(dskname);
  if (idfile && idfile->good())
    LoadDesktop(*idfile);
  else
    ShowProject();
  if (idfile)
    close_ifpstream(idfile);
}

Boolean
OpenProject(const char *prjname)
{
  ifpstream *idfile;
  ofpstream *ofile;
  char ori_dir[256];

  getcwd(ori_dir, 255);
#ifdef INTERNAL_DEBUGGER
  if (DEBUGGER_STARTED())
    RESET();
  ClearSymbols();
#endif
  if (project != NULL)
  {
    if (!CloseProject())
      return false;
  }
  project = NULL;
  project_name = NULL;
  already_maked = 0;
  if (!prjname)
  {
    OpenStandardProject("aout" STANDARD_EXE_SUFFIX);
    if (!project)
    {
      project = new TProject();
      InitOptions();
      SetProjectScreenMode();
    }
    SetMainTargetName("aout" STANDARD_EXE_SUFFIX);
    SetGlobalOptions();
    if (project_directory)
      string_free(project_directory);
    project_directory = getcwd(NULL, 512);
    setup_title(_("No project"));
    push_environment();
    return True;
  }
  {
    char *name, *ext;
    char *tmp;

    string_dup(tmp, prjname);
    FExpand(tmp);
    split_fname(tmp, project_name, name, ext);
    string_free(tmp);
    if (project_name[1]
        && (project_name[1] != ':' || strlen(project_name) != 3))
      project_name[strlen(project_name) - 1] = 0;
    chdir(project_name);
    string_free(project_name);
    string_dup(project_name, name);
    string_dup(dskname, project_name);
    string_cat(dskname, DESKTOP_EXT);
    string_cat(project_name, PROJECT_EXT);
    string_free(name);
    string_free(ext);
  }
  if ((project = ReadProject(project_name)) != NULL)
  {
    ClearDeskTop();
    idfile = open_ifpstream(dskname);
    if (idfile && idfile->good())
      LoadDesktop(*idfile);
    else
    {
      SetProjectScreenMode();
      ShowProject();
    }
    if (idfile)
      close_ifpstream(idfile);
  }
  if (!project)
  {
    OpenStandardProject(project_name, False);
    if (project)
    {
      if (project_window)
      {
        TWindow *w = project_window;

        RemoveWindow(project_window, False);
        w->close();
      }
      TProject *_project = project;

      project = new TProject();
      *project = *_project;
      SetMainTargetName(FName(_project->dest_name));
      destroy(_project);
      ClearFindCache();
      ShowProject();
      string_free(dskname);
      char *spec = NULL;

      string_cat(spec, "$(subst .gpr,.gdt,", project_name, ")", NULL);
      dskname = expand_rhide_spec(spec);
      string_free(spec);
    }
  }
  if (project_directory)
    string_free(project_directory);
  project_directory = getcwd(NULL, 512);
  if (project)
  {
    if (!CReservedWords)
      DefaultReservedWords();
    if (!GPCReservedWords)
      DefaultGPCReservedWords();
    if (!FPCReservedWords)
      DefaultFPCReservedWords();
    if (!RHIDEUserWords)
      DefaultUserWords();
    SetGlobalOptions();
    setup_title(project_name);
    push_environment();
    return True;
  }
  ofile = new ofpstream(project_name);
  if (ofile->good())
  {
    char *dir, *name, *ext;

    delete(ofile);
    remove(project_name);
    project = new TProject();
    split_fname(project_name, dir, name, ext);
    string_cat(name, STANDARD_EXE_SUFFIX);
    SetMainTargetName(name);
    string_free(dir);
    string_free(ext);
    string_free(name);
    InitOptions();
    ShowProject();
    SetGlobalOptions();
    setup_title(project_name);
    push_environment();
    return True;
  }
  delete(ofile);
  chdir(ori_dir);
  // I must create a project, because it is assumed every where
  // in RHIDE to have one
  project = new TProject();
  InitOptions();
  SetMainTargetName("aout" STANDARD_EXE_SUFFIX);
  SetGlobalOptions();
  messageBox(mfError | mfOKButton, _("Couldn't open %s"), prjname);
  string_free(project_name);
  setup_title(_("No project"));
  push_environment();
  return False;
}

struct PSTACK
{
  char *dir;
  char *name;
};

PSTACK *PRJSTACK = NULL;
int PRJSTACKCOUNT = 0;

void
AddToStack()
{
  char tmp[256];

  if (!project_name)
    return;
  PRJSTACKCOUNT++;
  PRJSTACK = (PSTACK *) realloc(PRJSTACK, PRJSTACKCOUNT * sizeof(PSTACK));
  PSTACK *ps = PRJSTACK + (PRJSTACKCOUNT - 1);

  getcwd(tmp, 255);
  string_dup(ps->dir, tmp);
  string_dup(ps->name, project_name);
}

int
OpenFromStack()
{
  if (!PRJSTACKCOUNT)
    return 0;
  PSTACK *ps = PRJSTACK + (PRJSTACKCOUNT - 1);

  chdir(ps->dir);
  OpenProject(ps->name);
  string_free(ps->dir);
  string_free(ps->name);
  PRJSTACKCOUNT--;
  PRJSTACK = (PSTACK *) realloc(PRJSTACK, PRJSTACKCOUNT * sizeof(PSTACK));
  return 1;
}

void
RemoveFromStack()
{
  if (!PRJSTACKCOUNT)
    return;
  PSTACK *ps = PRJSTACK + (PRJSTACKCOUNT - 1);

  string_free(ps->dir);
  string_free(ps->name);
  PRJSTACKCOUNT--;
  PRJSTACK = (PSTACK *) realloc(PRJSTACK, PRJSTACKCOUNT * sizeof(PSTACK));
}

Boolean
PushProject(const char *directory, const char *prjname)
{
  if (!SaveProjectOnlyWhenClosing)
    SaveProject();
  Boolean retval = _PushProject(directory, prjname);

  if (retval == True && (debug_commands || debug_files))
  {
    fprintf(stderr, _("changing cwd to %s\n"), directory);
    fprintf(stderr, _("temporary opening project %s\n"), prjname);
    setup_title(project_name);
  }
  return retval;
}

void
PopProject(Boolean Save)
{
  if (stack_count == 0)
    return;
  if (Save == True)
    SaveProject();
  if (debug_commands || debug_files)
  {
    fprintf(stderr, _("closing temporary opened project project %s\n"),
            project_name);
  }
  _PopProject();
  setup_title(project_name);
  if (debug_commands || debug_files)
  {
    fprintf(stderr, _("changing cwd to %s\n"), project_directory);
  }
}

void
ResetProjectStack()
{
  project_stack *ps;

  while (stack_count > 0)
  {
    ps = PROJECT_STACK + (stack_count - 1);
    string_free(ps->dname);
    stack_count--;
  }
  free(PROJECT_STACK);
  PROJECT_STACK = NULL;
  ClearDeskTop(False);
  LoadDesktop();
  if (debug_commands || debug_files)
  {
    fprintf(stderr, _("finaly opening project %s\n"), project_name);
  }
  SetGlobalOptions();
  if (msg_window)
    msg_window->select();
}

void
SaveProject()
{
  if (!project_name)
  {
    if (!standard_project_name)
      return;
    SaveProject(project, standard_project_name);
  }
  else
    SaveProject(project, project_name);
  if (stack_count == 0)
  {
    ofpstream *dfile;

    dfile = new ofpstream(dskname);
    SaveDesktop(*dfile);
    delete(dfile);
  }
}

bool
CloseProject()
{
  if (DEBUGGER_STARTED())
    RESET();
  SaveProject();
  if (dskname)
  {
    if (!ClearDeskTop())        // must be done before delete project
      return false;
  }
  destroy(project);
  project = NULL;
  if (project_name)
  {
    string_free(project_name);
    string_free(dskname);
  }
  pop_environment();
  return true;
}

class TIDEProjectWindow:public TProjectWindow
{
public:
  TIDEProjectWindow(const TRect & b, TDepCollection * dc,
                    const char *c):TProjectWindow(b, dc, c),
    TWindowInit(TIDEProjectWindow::initFrame)
  {
  }
  virtual void changeBounds(const TRect &);
};

void
TIDEProjectWindow::changeBounds(const TRect & r)
{
  TProjectWindow::changeBounds(r);
  ProjectWindowRect = r;
}

void
ShowProject(void)
{
  if (!project_window)
  {
    if (ProjectWindowRect.a.x == -1)
    {
      ProjectWindowRect = TProgram::deskTop->getExtent();
      ProjectWindowRect.a.y = ProjectWindowRect.b.y - 7;
    }
    project_window = new TIDEProjectWindow(ProjectWindowRect,
                                           Project.dependencies,
                                           _("Project Window"));
    AddWindow(project_window, (TWindow **) & project_window);
  }
  project_window->select();
}

ccIndex
AddProjectItem(const char *name)
{
  char *dir, *_name, *ext, *fname = NULL;
  ccIndex index;
  TDependency *dep;

  split_fname(name, dir, _name, ext);
  if (AllowDirectories)
  {
    /*
       Try to find the name in a deeper directory from the current
       or any from the source paths. Do not accept relative paths,
       which start with .. (on unix you can make mostly any filename
       relative to an other path) 
     */
    string_dup(fname, name);
    AbsToRelPath(project_directory, fname);
    if (IsRooted(fname) || fname[0] == '.')
    {
      int i, count = Options.SrcDirs->getCount();

      string_free(fname);
      for (i = 0; i < count; i++)
      {
        char *dname = expand_rhide_spec((char *) Options.SrcDirs->at(i));

        // AbsToRelpath expects this
        FExpand(dname);
        string_dup(fname, name);
        AbsToRelPath(dname, fname);
        string_free(dname);
        if (!IsRooted(fname) && fname[0] != '.')
          break;
        string_free(fname);
      }
    }
  }
  if (!fname)
  {
    // always store the pathname when using a sub-project
    if (get_file_type(name) == FILE_PROJECT)
    {
      fname = string_dup(name);
      AbsToRelPath(project_directory, fname);
    }
    else
    {
      string_dup(fname, _name);
      string_cat(fname, ext);
    }
  }
  if (!Project.dependencies)
    Project.dependencies = new TDepCollection(15, 16);
  if (Project.dependencies->search(fname, index) == True)
  {
    string_free(fname);
    string_free(_name);
    string_free(dir);
    string_free(ext);
    return -1;
  }
  dep = new TDependency();
  InitFName(dep->source_name, fname);
  dep->source_file_type = get_file_type(FName(dep->source_name));
  if (dep->source_file_type == FILE_LIBRARY)
  {
    InitFName(dep->dest_name, FName(dep->source_name));
  }
  else if (dep->source_file_type == FILE_PROJECT)
  {
    char *dname;

    string_dup(dname, dir);
    string_cat(dname, _name);
    string_cat(dname, ".a");
    AbsToRelPath(project_directory, dname);
    InitFName(dep->dest_name, dname);
    string_free(dname);
  }
  else
  {
    char *dot = strrchr(fname, '.');
    char *dname;

    if (dot)
      *dot = 0;
    string_dup(dname, fname);
    if (dot)
      *dot = '.';
    string_cat(dname, STANDARD_OBJECT_SUFFIX);
    InitFName(dep->dest_name, dname);
    string_free(dname);
  }
  dep->dest_file_type = get_file_type(FName(dep->dest_name));
  dep->compile_id = how_to_compile(dep->source_file_type,
                                   dep->dest_file_type);
  string_free(dir);
  string_free(fname);
  string_free(ext);
  string_free(_name);
  Project.dependencies->atInsert(index, dep);
  return index;
}

TFileDialog *
FileOpenDialog(char *init_val, char *title, char *input_label,
               ushort aOptions, int hist, char *init_dir)
{
  TFileDialog *dialog;

  if (!init_dir || !*init_dir)
  {
    dialog = new TFileDialog(init_val, title, input_label, aOptions, hist);
    return dialog;
  }
  dialog =
    new TFileDialog(init_val, title, input_label, aOptions | fdNoLoadDir,
                    hist);
  if (dialog->directory)
    delete(char *) dialog->directory;

  if (access(init_dir, X_OK) == 0)
    dialog->directory = strdup(init_dir);
  else
  {
    char *dname = string_dup(project_directory);

    string_cat(dname, "/");
    dialog->directory = dname;
  }
  dialog->valid(cmFileInit);
  return dialog;
}

void
AddProjectItem(void)
{
  TFileDialog *dialog;
  int result;

  InitHistoryID(RHIDE_History_source_file);
  dialog = FileOpenDialog(Project.defaultprojectmask,
                          _("Add Item"), _("~N~ame"), fdOKButton,
                          RHIDE_History_source_file, default_directory);
  TProgram::deskTop->insert(dialog);
  do
  {
    dialog->setState(sfModal, True);
    result = dialog->execute();
    if (result != cmCancel)
    {
      char fname[512];
      ccIndex index;

      string_free(Project.defaultprojectmask);
      string_free(default_directory);
      string_dup(default_directory, dialog->directory);
      string_dup(Project.defaultprojectmask, dialog->wildCard);
      dialog->getData(fname);
      index = AddProjectItem(fname);
      if (index < 0)
      {
        messageBox(mfError | mfOKButton,
                   _("The item '%s' is already in your project"), fname);
      }
      else
      {
        char *find_name = NULL;

        if (((TDependency *) Project.dependencies->at(index))->
            source_file_type != FILE_PROJECT
            &&
            FindFile(FName
                     (((TDependency *) Project.dependencies->at(index))->
                      source_name), find_name) == False)
        {
          int i, count, found = 0;
          char *tmp = (char *) malloc(strlen(fname) + 1);

          count = Options.SrcDirs->getCount();
          split_fname_fmt(fname, "%D", tmp);
          if (!(*tmp == '/' && !tmp[1])
              && !(rh_isalpha(*tmp) && tmp[1] == ':' && tmp[2] == '/'
                   && !tmp[3]))
            /*
               remove the last slash, but only when not the
               root directory 
             */
            tmp[strlen(tmp) - 1] = 0;
          AbsToRelPath(project_directory, tmp);
          if (*tmp)
          {
            for (i = 0; i < count; i++)
            {
              char *_tmp = expand_rhide_spec((char *) Options.SrcDirs->at(i));

              AbsToRelPath(project_directory, _tmp);
              if (strcmp(tmp, _tmp) == 0)
                found = 1;
              string_free(_tmp);
              if (found)
                break;
            }
          }
          else
          {
            // the current directory is found at any time
            found = 1;
          }
          if (!found && messageBox(mfConfirmation | mfYesButton | mfNoButton,
                                   _
                                   ("Add '%s' to the search path for source files"),
                                   tmp) == cmYes)
          {
            Options.SrcDirs->insert(strdup(tmp));
          }
          string_free(tmp);
        }
        string_free(find_name);
        if (project_window)
        {
          project_window->liste->setRange(Project.dependencies->getCount());
          project_window->liste->focusItem(index);
          project_window->liste->drawView();
        }
      }
      {
        int foc = dialog->fileList->focused + 1;

        if (foc < dialog->fileList->range)
          dialog->fileList->focusItemNum(foc);
      }
    }
  }
  while (result != cmCancel);
  TProgram::deskTop->remove(dialog);
  destroy(dialog);
}

void
DelProjectItem(void)
{
  if (!project_window)
    return;
  if (Project.dependencies->getCount() > 0)
  {
    int i = project_window->liste->focused;
    TDependency *dep = (TDependency *) Project.dependencies->at(i);

    if (messageBox(mfYesNoCancel, _("Remove '%s' from the project"),
                   FName(dep->source_name) ? FName(dep->
                                                   source_name) : FName(dep->
                                                                        dest_name))
        == cmYes)
    {
      Project.dependencies->atRemove(i);
      destroy(dep);
      project_window->liste->setRange(Project.dependencies->getCount());
      project_window->liste->drawView();
    }
  }
}

class TIncListBox:public TSortedListBox
{
public:
  TIncListBox(const TRect & r, int cols, TScrollBar * bar):TSortedListBox(r,
                                                                          cols,
                                                                          bar)
  {
  }
  virtual void handleEvent(TEvent &);
  virtual void selectItem(ccIndex);
};

void
TIncListBox::selectItem(ccIndex item)
{
  TSortedListBox::selectItem(item);
  endModal(cmOK);
}

void
TIncListBox::handleEvent(TEvent & event)
{
  TSortedListBox::handleEvent(event);
  switch (event.what)
  {
    case evKeyDown:
      switch (event.keyDown.keyCode)
      {
        case kbEnter:
          if (range > 0)
            selectItem(focused);
          clearEvent(event);
          break;
        default:
          break;
      }
    default:
      break;
  }
}

class TIncCollection:public TStringCollection
{
public:
  TIncCollection():TStringCollection(15, 16)
  {
    duplicates = True;
  }
  virtual int compare(void *, void *);
};

int
TIncCollection::compare(void *key1, void *key2)
{
  return strcasecmp((char *) key1, (char *) key2);
}

void
ShowIncludes()
{
  TDependency *dep;
  char *tmp;
  TIncCollection *list;
  TIncListBox *box;
  TRect r;
  TDialog *dialog;
  int i;

  if (!project_window)
    return;
  if (project->dependencies->getCount() == 0)
    return;
  dep =
    (TDependency *) (project->dependencies->
                     at(project_window->liste->focused));
  list = new TIncCollection();
  r = TProgram::deskTop->getExtent();
  r.b.y -= 7;
  string_dup(tmp, _("Include files for "));
  string_cat(tmp,
             dep->source_name ? FName(dep->source_name) : FName(dep->
                                                                dest_name));
  dialog = new TDialog(r, tmp);
  r = dialog->getExtent();
  r.grow(-1, -1);
  box = new TIncListBox(r, 1, dialog->standardScrollBar(sbVertical));
  if (dep->dependencies)
  {
    for (i = 0; i < dep->dependencies->getCount(); i++)
    {
      TDependency *d = (TDependency *) (dep->dependencies->at(i));

      if (d->dest_name)
      {
        char *dir, *name, *ext;
        char buffer[100];

        split_fname(FName(d->dest_name), dir, name, ext);
        string_cat(name, ext);
        sprintf(buffer, "%-20s", name);
        string_free(name);
        string_free(ext);
        string_dup(name, buffer);
        string_cat(name, "(");
        string_cat(name, dir);
        string_cat(name, ")");
        string_free(dir);
        list->insert(name);
      }
    }
  }
  box->newList(list);
  dialog->insert(box);
  if (TProgram::deskTop->execView(dialog) == cmOK)
  {
    char *name, *dir, _tmp[256], *tmp = _tmp, fname[256];

    strcpy(tmp, (char *) list->at(box->focused));
    name = tmp;
    while (*tmp != ' ')
      tmp++;
    *tmp = 0;
    tmp++;
    while (*tmp != '(')
      tmp++;
    tmp++;
    dir = tmp;
    while (*tmp != ')')
      tmp++;
    *tmp = 0;
    strcpy(fname, dir);
    strcat(fname, name);
    OpenEditor(fname, False);
  }
  destroy(dialog);
  destroy(list);
}

static void
clear_deps(TDependency * dep)
{
  if (debug_dependencies)
  {
    fprintf(stderr, _("clearing dependencies for %s\n"),
            FName(dep->dest_name));
  }
  if (dep->dependencies)
  {
    int i = 0;

    while (i < dep->dependencies->getCount())
    {
      TDependency *_dep = (TDependency *) dep->dependencies->at(i);

      clear_deps(_dep);
      if (!_dep->source_name && !_dep->dependencies)
        dep->dependencies->atFree(i);
      else
        i++;
    }
    if (dep->dependencies->getCount() == 0)
    {
      destroy(dep->dependencies);
      dep->dependencies = NULL;
    }
  }
}

void
ClearDependencies()
{
  if (Project.dependencies)
  {
    int i, count = Project.dependencies->getCount();

    for (i = 0; i < count; i++)
      clear_deps((TDependency *) Project.dependencies->at(i));
  }
  ClearFileHash();
  ClearFindCache();
}

static void
clean_files(TDependency *dep)
{
  char *files = expand_rhide_spec("$(CLEAN_FILES)");
  char *file = files;
  char *tmp = files;
  while (tmp && *tmp)
  {
    file = tmp;
    while (*tmp && *tmp != ' ') tmp++;
    char c = *tmp;
    *tmp = 0;
    remove(file);
    *tmp = c;
    tmp++;
  }
  string_free(files);
  if (!DeleteRecursive)
    return;
  int count = 0;

  if (project->dependencies)
    count = project->dependencies->getCount();
  for (int i = 0; i < count; i++)
  {
    TDependency *_dep = (TDependency *) project->dependencies->at(i);

    if (_dep->source_file_type == FILE_PROJECT)
    {
      AddToStack();
      if (_PushProject(_dep) == True)
        clean_files(_dep);
      _PopProject();
      RemoveFromStack();
    }
  }
}

void
MakeClear()
{
  clear_deps(&Project);
  clean_files(&Project);
  ClearFileHash();
  ClearFindCache();
}

