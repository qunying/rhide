/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TApplication
#define Uses_TWindow
#define Uses_TCommandSet
#define Uses_TPalette
#define Uses_TDeskTop
#define Uses_TStringCollection

#define Uses_TCEditor
#define Uses_TCEditWindow
#define Uses_TCEditor_Commands
#define Uses_TIDEEditWindow
#define Uses_TProject
#include <libide.h>

#include "rhide.h"


#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#include "ideapp.h"

#include <unistd.h>

#include "pal.h"

#ifdef __DJGPP__
#include <sys/exceptn.h>
#endif

TStringCollection *ReservedWords;
TStringCollection *UserWords;
TStringCollection *PascalRWords;
TStringCollection *FpcRWords;
TStringCollection *ClipperRWords;
TCEditWindow *clipWindow;

IDE::IDE():
TProgInit(IDE::initStatusLine,
          IDE::initMenuBar, IDE::initDeskTop), TApplication()
{
  TCEditor::editorDialog = IDEdoEditDialog;
  createClipBoard();
  update();
}

static const char *_cpIDEColor = cpIDEColor;
static const char *_cpIDEBlackWhite = cpIDEBlackWhite;
static const char *_cpIDEMono = cpIDEMono;
static int cpColorSize = sizeof(cpIDEColor) - 1;
static int cpBlackWhiteSize = sizeof(cpIDEBlackWhite) - 1;
static int cpMonoSize = sizeof(cpIDEMono) - 1;

static int last_palette = -1;

TPalette & IDE::getPalette() const
{
  static TPalette
  color(_cpIDEColor, cpColorSize);
  static TPalette
  bw(_cpIDEBlackWhite, cpBlackWhiteSize);
  static TPalette
  mono(_cpIDEMono, cpMonoSize);

  if (last_palette != appPalette)
  {
    last_palette = appPalette;
    TCEditor::colorsCached = 0;
  }
  return appPalette == apBlackWhite ? bw :
    appPalette == apMonochrome ? mono : color;
}

void
IDE::resume()
{
  TApplication::resume();
  initScreen();
}

TCEditWindow *
IDE::openEditor(char *fileName, Boolean visible)
{
  char *fname = string_dup(fileName);
  TRect r = deskTop->getExtent();
  TWindow *p;

  r.b.y -= 7;
  if (UseRCS && *fname)
  {
    if (!__file_exists(fname))
    {
      char *rcs_name = NULL, *_fname = NULL;
      Boolean found = FindRCSFile(fname, rcs_name, _fname);

      if (found == True)
      {
        CheckoutRCSFile(rcs_name, _fname, 1);
        string_free(fname);
        fname = string_dup(_fname);
        string_free(rcs_name);
      }
    }
  }
  int was_closed = IsClosedWindow(fname);

  if (was_closed == -1)
  {
    p = new TIDEEditWindow(r, fname, wnNoNumber);
    if (visible == False)
      p->hide();
    AddWindow(p, NULL, False, visible);
  }
  else
  {
    p = OpenClosedWindow(was_closed);
  }
  if (fname && *fname)
    TimeOfFile(((TCEditWindow *) p)->editor->fileName, True);	// this removes the file from the hashtable
  p->growMode = gfGrowHiX | gfGrowHiY;
  string_free(fname);
  return (TCEditWindow *) p;
}

void
IDE::DosShell()
{
  char *directory;

  if (!SaveAll())
    return;
  if (!SaveProjectOnlyWhenClosing)
    SaveProject();
  App->suspend();
  directory = getcwd(NULL, PATH_MAX);
#ifdef __DJGPP__
  if (debug_commands)
  {
    fprintf(stderr, "%s: %s\n", _("executing: "), "calling system(\"\")");
  }
  __djgpp_exception_toggle();
  system("");                   // let system() choose the correct shell
  __djgpp_exception_toggle();
#else
  char *sh = expand_spec("$(SHELL)", NULL);

  if (!*sh)
    sh = string_dup("/bin/sh");
  // I hope any usual shell accepts the -i switch
  string_cat(sh, " -i");
  if (debug_commands)
  {
    fprintf(stderr, "%s: %s\n", _("executing: "), sh);
  }
  system(sh);
  string_free(sh);
#endif
  chdir(directory);
  free(directory);
  App->resume();
  check_for_external_modifications();
  Repaint();
  ClearFileHash();
  ClearFindCache();
  already_maked = 0;
}

void
IDE::createClipBoard(void)
{
  TRect r = deskTop->getExtent();

  r.b.y -= 7;
  clipWindow = new TIDEEditWindow(r, 0, wnNoNumber);
  clipWindow->hide();
  clipWindow->growMode = gfGrowHiX | gfGrowHiY;
  TCEditor::clipboard = clipWindow->editor;
  TCEditor::clipboard->canUndo = False;
  TProgram::deskTop->insert(clipWindow);
}

void
IDE::fileNew()
{
  openEditor(0, True);
}

void
IDE::ShowClip()
{
  clipWindow->select();
  clipWindow->show();
}

void
IDE::Tile()
{
  deskTop->tile(deskTop->getExtent());
}

void
IDE::Cascade()
{
  deskTop->cascade(deskTop->getExtent());
}
