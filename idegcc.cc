/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TDependency
#define Uses_TProject
#define Uses_TDepCollection
#define Uses_TDependency
#define Uses_TProjectWindow
#define Uses_TOptions
#define Uses_TDirList
#define Uses_TParamList
#define Uses_TProjectListBox
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TScreen
#define Uses_TEvent
#define Uses_IDEConst
#define Uses_TNSSortedCollection
#define Uses_TIDEEditWindow
#define Uses_TStaticText
#define Uses_TIDEFileEditor
#define Uses_TFileViewer
#define Uses_TWindowList
#define Uses_MsgBox

#define Uses_ideFunctions
#define Uses_ideEnums
#define Uses_ideCommands
#define Uses_TIDEEditWindow
#define Uses_TFileName
#include <libide.h>

#define Uses_TSCollection
#define Uses_TCheckDialog
#define Uses_TMsgCollection
#define Uses_MsgRec
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#include "rhide.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <ctype.h>

#ifdef INTERNAL_DEBUGGER
#include <librhgdb.h>
#endif

#ifndef __DJGPP__
#include <termios.h>
#endif

int
project_index(const char *name)
{
  ccIndex index;
  char *tmp;

  string_dup(tmp, name);
  BaseName(tmp);
  Boolean found = Project.dependencies->search(tmp, index);

  string_free(tmp);
  if (found == True)
    return index;
  return -1;
}

static TCheckDialog *CheckDialog = NULL;
static TCheckDialog *ExecDialog = NULL;

int
RunProgram(const char *cmd,
           Boolean redir_stderr, Boolean redir_stdout, Boolean SwitchToUser)
{
#ifdef __linux__
  int vintr = 0;
  struct termios term;
#endif
  int retval;

  if (ExecDialog)
  {
    ExecDialog->update(cmd);
  }
  if (debug_commands)
  {
    fprintf(stderr, _("executing: "));
    fprintf(stderr, "%s", cmd);
    fprintf(stderr, "\n");
  }
  SaveAll();

  if (SwitchToUser == True)
  {
    TProgram::deskTop->setState(sfVisible, False);
    TProgram::application->suspend();
  }
#ifdef __linux__
  else
  {
    tcgetattr(fileno(stdin), &term);
    vintr = term.c_cc[VINTR];
    term.c_cc[VINTR] = 3;
    tcsetattr(fileno(stdin), TCSANOW, &term);
  }
#endif

  if (redir_stdout == True)
    cpp_outname = open_stdout();
  if (redir_stderr == True)
    cpp_errname = open_stderr();
  retval = system(cmd);
  setup_main_title();
  if (project)
    setup_title(project_name);
  else
    setup_title(_("No project"));

#ifdef __linux__
  if (SwitchToUser == false)
  {
    term.c_cc[VINTR] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &term);
  }
#endif

  external_program_executed = 1;
  if (redir_stdout == True)
    close_stdout();
  if (redir_stderr == True)
    close_stderr();
  if (SwitchToUser == True)
  {
    TProgram::application->resume();
    TProgram::deskTop->setState(sfVisible, True);
    SetProjectScreenMode();
    Repaint();
  }
  if (debug_commands)
  {
    fprintf(stderr, _("return value: %d\n"), retval);
  }
  return retval;
}

static Boolean up_to_date;

class FileTimeCollection:public TNSSortedCollection
{
public:
  FileTimeCollection(ccIndex alimit,
                     ccIndex adelta):TNSSortedCollection(alimit, adelta)
  {
  }
private:
    virtual int compare(void *, void *);
  virtual void freeItem(void *);
};

#if 0

struct FileTime
{
  FileTime(const char *aname, long atime);
   ~FileTime();
  char *key;
  char *name;
  long time;
};

FileTime::FileTime(const char *aname, long atime)
{
  string_dup(name, aname);
  time = atime;
  BaseName(name, key);
}

FileTime::~FileTime()
{
  string_free(name);
  string_free(key);
}

int
FileTimeCollection::compare(void *key1, void *key2)
{
  int retval = strcmp(((FileTime *) key1)->key, ((FileTime *) key2)->key);

  if (retval)
    return retval;
  return strcmp(((FileTime *) key1)->name, ((FileTime *) key2)->name);
}

#else

struct FileTime
{
  FileTime(const char *aname, long atime);
   ~FileTime();
  char *key;
  TFileName *name;
  long time;
};

FileTime::FileTime(const char *aname, long atime)
{
  name = new TFileName((char *) aname);
  time = atime;
  BaseName(name->Name(), key);
}

FileTime::~FileTime()
{
  delete name;

  string_free(key);
}

int
FileTimeCollection::compare(void *key1, void *key2)
{
  int retval = strcmp(((FileTime *) key1)->key, ((FileTime *) key2)->key);

  if (retval)
    return retval;
  return strcmp(((FileTime *) key1)->name->Name(),
                ((FileTime *) key2)->name->Name());
}

#endif

void
FileTimeCollection::freeItem(void *item)
{
  delete((FileTime *) item);
}

static FileTimeCollection *file_times = NULL;

long
TimeOfFile(const char *name, Boolean update, Boolean is_full_name)
{
  FileTime *ft;
  ccIndex index;
  long retval;
  char *full_name;
  TCEditWindow *e;

  if (debug_files)
  {
    fprintf(stderr, _("checking file time for '%s'\n"), name);
  }
  if (is_full_name == True)
    full_name = (char *) name;
  else
  {
    full_name = (char *) alloca(512);
    strcpy(full_name, name);
    FExpand(full_name, False);
  }
  e = is_on_desktop(full_name, True, False);
  if (e)
  {
    if (!NoFileCache && file_times != NULL)
    {
      ft = new FileTime(full_name, 0);
      if (file_times->search(ft, index) == True)
      {
        file_times->atRemove(index);
      }
      delete(ft);
    }
    if (debug_files)
    {
      fprintf(stderr, _("found '%s' on desktop, time: %08lx\n"), name,
              ((TIDEFileEditor *) (e->editor))->edittime);
    }
    if (update == True)
    {
      if (debug_files)
      {
        fprintf(stderr, _("and updating it"));
        fprintf(stderr, "\n");
      }
      retval = ((TIDEFileEditor *) (e->editor))->edittime =
        time_of_file(full_name);
      goto ende;
    }
    else
    {
      retval = ((TIDEFileEditor *) (e->editor))->edittime;
      goto ende;
    }
  }
  if (NoFileCache)
  {
    retval = time_of_file(full_name);
    if (debug_files)
    {
      fprintf(stderr, _("found '%s' not in filetime cache, time: %08lx\n"),
              name, retval);
    }
    goto ende;
  }
  if (file_times == NULL)
    file_times = new FileTimeCollection(100, 100);
  ft = new FileTime(full_name, 0);
  if (file_times->search(ft, index) == True)
  {
    if (debug_files)
    {
      fprintf(stderr, _("found '%s' in filetime cache "), name);
    }
    if (update == True)
    {
      if (debug_files)
      {
        fprintf(stderr, _("and updating it"));
      }
      ((FileTime *) file_times->at(index))->time = retval = time_of_file(full_name);
    }
    else
    {
      retval = ((FileTime *) file_times->at(index))->time;
    }
    if (debug_files)
    {
      fprintf(stderr, _(",time: %08lx\n"), retval);
    }
    delete(ft);
  }
  else
  {
    retval = time_of_file(full_name);
    ft->time = retval;
    file_times->insert(ft);
    if (debug_files)
    {
      fprintf(stderr, _("found '%s' not in filetime cache, time: %08lx\n"),
              name, retval);
    }
  }
ende:
  return retval;
}

static void
indent(int depth)
{
  int i;

  for (i = 0; i < depth * 2; i++)
    fputc(' ', stderr);
}

static jmp_buf cancel_make;

static void
InitExecDialog()
{
  TRect r = TProgram::deskTop->getExtent();

  r.b.y -= 7;
  ExecDialog = new TCheckDialog(r, _("executing: "));
  TProgram::deskTop->lock();
  TProgram::deskTop->insert(ExecDialog);
  TProgram::deskTop->unlock();
  r.a.y = 5;
  r.b.y = r.a.y + 4;
  r.a.x += 2;
  r.b.x -= 2;
  CheckDialog = new TCheckDialog(r, _("checking"));
  TProgram::deskTop->insert(CheckDialog);
}

static void
DeleteExecDialog()
{
  TProgram::deskTop->remove(CheckDialog);
  TProgram::deskTop->remove(ExecDialog);
  destroy(CheckDialog);
  destroy(ExecDialog);
  CheckDialog = NULL;
  ExecDialog = NULL;
}

Boolean
Compile(TDependency * _dep)
{
  TDependency *dep;
  const char *name = NULL;

  if (!SaveAll())
    return False;
  ShowMessages(NULL, True);
  Boolean retval = False;

  if ((dep = _dep) == NULL)
  {
    if (project_window && ((project_window->liste->state & sfFocused) != 0))
    {
      dep =
        (TDependency *) Project.dependencies->at(project_window->liste->
                                                 focused);
    }
    else
    {
      int i;
      TWindow *w = (TWindow *) TProgram::deskTop->current;

      if (!w)
        return False;
      TEvent event;

      event.what = evBroadcast;
      event.message.command = cmEditorAnswer;
      w->handleEvent(event);
      if (event.what != evNothing)
        return False;
      name = ((TCEditWindow *) w)->editor->fileName;
      i = project_index(name);
      if (i >= 0)
        dep = (TDependency *) Project.dependencies->at(i);
      else
        dep = NULL;
    }
  }
  if (ShowWhatDoing)
  {
    InitExecDialog();
    CheckDialog->hide();
  }
  if (!dep)
  {
    dep = new TDependency();
    InitFName(dep->source_name, name);
    char *dname;

    BaseName(name, dname, False);
    string_cat(dname, STANDARD_OBJECT_SUFFIX);
    InitFName(dep->dest_name, dname);
    string_free(dname);
    dep->source_file_type = get_file_type(name);
    dep->dest_file_type = FILE_OBJECT;
    dep->compile_id = how_to_compile(dep->source_file_type,
                                     dep->dest_file_type);
    retval = compile_dep(dep);
    destroy(dep);
  }
  else
  {
    retval = compile_dep(dep);
  }
  if (ShowWhatDoing)
    DeleteExecDialog();
  return retval;
}

Boolean
time_of_dep(TDependency * dep, long &target_time, Boolean build,
            int depth, int recur_depth)
{
  long source_time = 0;
  char *dest_name = NULL;
  Boolean retval = True;
  Boolean is_rcs_file = False;
  char *rcs_file = NULL;

  if (dep->source_file_type == FILE_PROJECT)
  {
    char *dname = NULL, *pname;
    const char *_pname;

    if (!dep->new_project_dep)
    {
      dname = (char *) alloca(strlen(FName(dep->dest_name)) + 1);
      split_fname_fmt(FName(dep->dest_name), "%D", dname);
      if (*dname && dname[strlen(dname) - 1] == '/')
        dname[strlen(dname) - 1] = 0;
      _pname = FName(dep->source_name);
      pname = (char *) alloca(strlen(_pname) + 1);
      strcpy(pname, _pname);
      BaseName(pname);
    }
    else
    {
      dname = (char *) alloca(strlen(FName(dep->source_name)) + 1);
      pname = (char *) alloca(strlen(FName(dep->source_name)) + 1);
      split_fname_fmt(FName(dep->source_name), "%D%f", dname, pname);
      if (*dname && dname[strlen(dname) - 1] == '/')
        dname[strlen(dname) - 1] = 0;
    }
    if (!dep->rebuild_only_nonexistant ||
        !__file_exists(FName(dep->dest_name)))
    {
      AddToStack();
      if (PushProject(dname, pname) == False)
      {
        ResetProjectStack();
        longjmp(cancel_make, -1);
      }
      retval = time_of_dep(project, target_time, build, depth + 1, depth + 1);
      if (retval == False)
      {
        ResetProjectStack();
        longjmp(cancel_make, -1);
      }
      if (dep->dest_name)
        delete dep->dest_name;

      string_dup(dname, FName(project->dest_name));
      if (*dname)
        FExpand(dname);
      pname = string_dup(pname);
      FExpand(pname);
      PopProject(SaveProjectOnlyWhenClosing == 0);
      RemoveFromStack();
      AbsToRelPath(project_directory, dname);
      InitFName(dep->dest_name, dname);
      AbsToRelPath(project_directory, pname);
      InitFName(dep->source_name, pname);
      dep->dest_file_type = get_file_type(dname);
      dep->compile_id = how_to_compile(dep->source_file_type,
                                       dep->dest_file_type);
      dep->new_project_dep = 1;
      string_free(dname);
      string_free(pname);
    }
    else
    {
      target_time = TimeOfFile(dname, False, True);
    }
    return retval;
  }
  if (dep->dest_file_type == FILE_NOTHING)
  {
    if (dep != project)
      target_time = LONG_MAX;
    dest_name = string_dup("");
  }
  else
  {
    FindFile(FName(dep->dest_name), dest_name);
    if (CheckDialog && depth < recur_depth + 2)
      CheckDialog->update(dest_name);
    target_time = TimeOfFile(dest_name, False, True);
  }
  if (debug_dependencies)
  {
    indent(depth);
    fprintf(stderr, "%s: %s\n", _("checking"), dest_name);
  }
  if (dep->source_name == NULL)
    source_time = target_time;
  else
  {
    char *sname;
    Boolean was_found;

    if (debug_dependencies)
    {
      indent(depth + 1);
      fprintf(stderr, "%s %s\n", _("checking"), FName(dep->source_name));
    }
    was_found = FindFile(FName(dep->source_name), sname);
    if (was_found == False && UseRCS)
    {
      string_free(sname);
      is_rcs_file = was_found =
        FindRCSFile(FName(dep->source_name), sname, rcs_file);
      string_free(rcs_file);
    }
    if (was_found == False &&
        TimeOfFile(FName(dep->source_name), False, False) == 0)
    {
      if (debug_dependencies)
      {
        indent(depth + 1);
        fprintf(stderr, "%s does not exist\n", sname);
      }
      string_free(sname);
      BigmessageBox(mfError | mfOKButton, _("Could not find the source file "
                                            "'%s'. Make sure, that the file "
                                            "exist or check the settings in "
                                            "'Options/Directories' for the "
                                            "correct paths."),
                    FName(dep->source_name));
      return False;
    }
    source_time = TimeOfFile(sname, False, True);
    if (debug_dependencies)
    {
      indent(depth + 1);
      fprintf(stderr, "%s %s\n", FName(dep->source_name), _("is up to date"));
      indent(depth);
      fprintf(stderr, "%s %s %s %s %s\n",
              FName(dep->source_name), _("is"), (source_time > target_time ?
                                                 _("newer") : _("older")),
              _("than"), dest_name);
    }
    string_free(sname);
  }
  if ((build == True || source_time <= target_time)
      && (build == True || !dep->rebuild_only_nonexistant
          || !__file_exists(dest_name))
      && dep->dependencies != NULL
      && dep->compile_id != COMPILE_LINK_PASCAL_AUTOMAKE)
  {
    int i, count = dep->dependencies->getCount();
    long akt_time = 0;

    for (i = 0; i < count; i++)
    {
      TDependency *_dep = (TDependency *) dep->dependencies->at(i);

      retval = time_of_dep(_dep, akt_time, build, depth + 1, recur_depth);
      if (retval == False)
        return retval;
      if (akt_time > source_time)
        source_time = akt_time;
      if (debug_dependencies)
      {
        char *dname;

        indent(depth);
        FindFile(FName(_dep->dest_name), dname);
        fprintf(stderr, "%s %s %s %s %s\n",
                dname, _("is"),
                (akt_time > target_time ? _("newer") : _("older")), _("than"),
                dest_name);
        string_free(dname);
      }
    }
  }
  if (!*dest_name)
    target_time = source_time;
  if (build == False && source_time <= target_time)
  {
    string_free(dest_name);
    return retval;
  }
  if (debug_dependencies)
  {
    indent(depth);
    fprintf(stderr, "%s %s\n", _("must rebuild"), dest_name);
  }
  if (CheckDialog)
    CheckDialog->hide();
  retval = compile_dep(dep);
  if (CheckDialog)
    CheckDialog->show();
  if (debug_dependencies)
  {
    indent(depth);
    fprintf(stderr, "%s %s  ===>  %s\n", _("rebuilding of"), dest_name,
            (retval == True ? _("success") : _("error")));
  }
  up_to_date = False;
  if (retval == True)
    target_time = TimeOfFile(dest_name, True, True);
  string_free(dest_name);
  return retval;
}

void
ClearFileHash()
{
  if (debug_files)
  {
    fprintf(stderr, _("clearing the filetime cache\n"));
  }
  destroy(file_times);
  file_times = NULL;
}

static int save_source_name;

Boolean
Make(Boolean force_messages)
{
  Boolean retval;
  long akttime;
  int need_make = hasmodified || !already_maked;

  save_source_name = 1;
  if (!SaveAll())
    return False;
  if (!project_name)
  {
    if (Project.dependencies)
    {
      while (Project.dependencies->getCount() > 0)
      {
        Project.dependencies->atFree(0);
      }
    }
    if (!windows)
      return False;
    int count = windows->getCount(), i;

    for (i = 0; i < count; i++)
    {
      TEvent event;
      TWindow *window = DESKTOPWINDOW(i);

      event.what = evBroadcast;
      event.message.command = cmEditorAnswer;
      window->handleEvent(event);
      if (event.what == evNothing)
      {
        const char *title = ((TCEditWindow *) window)->editor->fileName;
        COMPILE_ID id = how_to_compile(title, STANDARD_OBJECT_SUFFIX);

        if (id != COMPILE_NONE && id != COMPILE_UNKNOWN)
        {
          char *full_name;
          char *bname;

          BaseName(title, bname);
          if (FindFile(bname, full_name) == False)
          {
            /*
               If the file was not found in the current or source
               directories, add a search path to it 
             */
            char dir[PATH_MAX];

            split_fname_fmt(title, "%D", dir);
            strcat(dir, ".");
            Options.SrcDirs->insert(dir);
          }
          string_free(full_name);
          string_free(bname);
          AddProjectItem(title);
        }
      }
    }
    if (!Project.dependencies || Project.dependencies->getCount() == 0)
      return False;
    if (!Project.source_name)
      save_source_name = 0;
    if (Project.dependencies->getCount() == 1)
    {
      char *name;

      if (save_source_name)
        BaseName(FName(Project.source_name), name, False);
      else
        BaseName(FName
                 (((TDependency *) Project.dependencies->at(0))->source_name),
                 name, False);
      string_cat(name, STANDARD_EXE_SUFFIX);
      if (!save_source_name)
      {
        if (((TDependency *) Project.dependencies->at(0))->source_file_type
            == FILE_PASCAL_SOURCE)
          InitFName(Project.source_name,
                    FName(((TDependency *) Project.dependencies->at(0))->
                          source_name));
        else if (((TDependency *) Project.dependencies->at(0))->
                 source_file_type == FILE_FPC_SOURCE)
          InitFName(Project.source_name,
                    FName(((TDependency *) Project.dependencies->at(0))->
                          source_name));
      }
      SetMainTargetName(name);
      string_free(name);
    }
    else
    {
      if (save_source_name)
      {
        char *name;

        BaseName(FName(Project.source_name), name, False);
        string_cat(name, STANDARD_EXE_SUFFIX);
        SetMainTargetName(name);
        string_free(name);
      }
      else
        SetMainTargetName("aout" STANDARD_EXE_SUFFIX);
    }
  }
  if (ShowWhatDoing)
  {
    InitExecDialog();
  }
  if (force_messages == True)
    ShowMessages(NULL, True);
  up_to_date = True;
  retval = True;
  if (need_make)
  {
    if (setjmp(cancel_make) == 0)
    {
      // simulate a build when using '--automake'
      if (Project.source_name && *FName(Project.source_name))
        retval = time_of_dep(project, akttime, True, 0, 0);
      else
        retval = time_of_dep(project, akttime, False, 0, 0);
    }
    else
    {
      retval = False;
      up_to_date = False;
    }
  }
  if (!save_source_name && Project.source_name)
  {
    delete Project.source_name;

    Project.source_name = NULL;
  }
  if (retval == False)
  {
    up_to_date = False;
    already_maked = 0;
    ShowMessages(NULL, False, True);
  }
  else
  {
    if (PRJSTACKCOUNT)
      already_maked = 0;
    else
      already_maked = 1;
  }
  if (up_to_date == True && force_messages == True)
  {
    TMsgCollection *errs;
    char *tmp;

    string_dup(tmp, FName(project->dest_name));
    string_cat(tmp, " ");
    string_cat(tmp, _("is up to date"));
    errs = new TMsgCollection();
    errs->insert(new MsgRec(NULL, tmp));
    string_free(tmp);
    ShowMessages(errs, True);
  }
  if (ExecDialog)
  {
    DeleteExecDialog();
  }
  if (!SaveProjectOnlyWhenClosing)
    SaveProject();
  return retval;
}

Boolean
Build()
{
  ShowMessages(NULL, True);
  ClearFileHash();
  ClearFindCache();
  MakeClear();
  already_maked = 0;
  return Make();
}

#ifdef INTERNAL_DEBUGGER
static void
ScanForTraceback(TFileViewer * viewer)
{
  int count = viewer->Count();
  int i = 0;
  TMsgCollection *msgs = new TMsgCollection();
  const char *tmp;
  const char *_tmp;
  const char *buffer;
  int trace_start = 0;

  for (i = 0; i < count; i++)
  {
    buffer = (*viewer)[i];
    if (trace_start)
    {
      tmp = buffer;
      while (*tmp == ' ')
        tmp++;
      if (tmp[0] == '0' && tmp[1] == 'x')
      {
        _tmp = tmp + 2;
        while (isxdigit(*_tmp))
          _tmp++;
        int len = (int) (_tmp - tmp);
        char *dummy;

        dummy = (char *) alloca(len + 1);
        memcpy(dummy, tmp, len);
        dummy[len] = 0;
        int line;
        char *function, *file, *temp = NULL;

        symify(dummy, &function, &file, &line, 0);
        if (function)
        {
          string_dup(temp, _("in function "));
          string_cat(temp, function);
        }
        else
        {
          string_dup(temp, tmp);
          string_cat(temp, " ???");
        }
        msgs->insert(new MsgRec(file, temp, msgMessage, line));
        string_free(function);
        string_free(file);
        string_free(temp);
      }
    }
    else
    {
      if (strstr(buffer, "Call frame traceback EIPs") != NULL)
      {
        trace_start = 1;
        InitRHGDB();
        msgs->insert(new MsgRec(NULL, _("Call frame traceback:")));
      }
    }
  }
  if (msgs->getCount() > 0)
    ShowMessages(msgs, False);
  else
    destroy(msgs);
}
#endif

class TIDEFileWindow:public TWindow
{
public:
  TIDEFileWindow(const char *);
  TFileViewer *FileViewer;
};

TIDEFileWindow::TIDEFileWindow(const char *fileName):
TWindow(TProgram::deskTop->getExtent(), fileName, wnNoNumber),
TWindowInit(&TIDEFileWindow::initFrame)
{
  options |= ofTileable;
  TRect r(getExtent());

  r.grow(-1, -1);
  insert((FileViewer = new TFileViewer(r,
                                       standardScrollBar(sbHorizontal |
                                                         sbHandleKeyboard),
                                       standardScrollBar(sbVertical |
                                                         sbHandleKeyboard),
                                       fileName)));
  FileViewer->growMode = gfGrowHiX | gfGrowHiY;
  growMode = gfGrowLoY | gfGrowHiX | gfGrowHiY;
  flags |= wfGrow | wfZoom;
}

void
CheckStderr(bool erase)
{
  if (ShowStderr)
  {
    TIDEFileWindow *window;
    char *tmp;
    TRect rect = TProgram::deskTop->getExtent();

    rect.a.y = rect.b.y - 7;
    string_dup(tmp, _("contents of stderr from "));
    string_cat(tmp, FName(project->dest_name));
    if (windows != NULL)
    {
      int count = windows->getCount(), i;

      for (i = 0; i < count; i++)
      {
        window = (TIDEFileWindow *) DESKTOPWINDOW(i);
        if (window->title && (strcmp(tmp, window->title) == 0))
        {
          window->close();
          RemoveWindow(window);
          break;
        }
      }
    }
    struct stat st;

    if (stat(cpp_errname, &st) == 0 && st.st_size > 0)
    {
      window = new TIDEFileWindow(cpp_errname);
      delete(char *) (window->title);

      (char *&) window->title = tmp;
      window->palette = wpGrayWindow;
      window->locate(rect);
      AddWindow(window);
#ifdef INTERNAL_DEBUGGER
      ScanForTraceback(window->FileViewer);
#endif
    }
  }
  //if (ShowStderr && !debug_tempfiles && erase) remove(cpp_errname);
  if (erase)
    RemoveStderr();
}

void
CheckStdout(bool erase)
{
  if (ShowStdout)
  {
    TIDEFileWindow *window;
    char *tmp;
    TRect rect = TProgram::deskTop->getExtent();

    rect.b.y -= 7;
    string_dup(tmp, _("contents of stdout from "));
    string_cat(tmp, FName(project->dest_name));
    if (windows != NULL)
    {
      int count = windows->getCount(), i;

      for (i = 0; i < count; i++)
      {
        window = (TIDEFileWindow *) DESKTOPWINDOW(i);
        if (window->title && (strcmp(tmp, window->title) == 0))
        {
          window->close();
          RemoveWindow(window);
          break;
        }
      }
    }
    struct stat st;

    if (stat(cpp_outname, &st) == 0 && st.st_size > 0)
    {
      window = new TIDEFileWindow(cpp_outname);
      delete(char *) (window->title);

      (char *&) window->title = tmp;
      window->palette = wpGrayWindow;
      window->locate(rect);
      AddWindow(window);
    }
  }
  //if (ShowStdout && !debug_tempfiles && erase) remove(cpp_outname);
  if (erase)
    RemoveStdout();
}


void
RemoveStdout(void)
{
  if (ShowStdout && !debug_tempfiles && cpp_outname)
    remove(cpp_outname);
}


void
RemoveStderr(void)
{
  if (ShowStderr && !debug_tempfiles && cpp_errname)
    remove(cpp_errname);
}



Boolean
RunMainTarget()
{
  char *spec, *cmd;
  Boolean redir_stdout = False, redir_stderr = False;

#ifdef INTERNAL_DEBUGGER
  if (BreakPointCount() > 0)
  {
    CONTINUE();
  }
  else
#endif
  {
    int exit_code;

    if (Project.dest_file_type != FILE_COFF &&
        Project.dest_file_type != FILE_EXE) return False;
    if (ShowStderr)
      redir_stderr = True;
    if (ShowStdout)
      redir_stdout = True;
    string_dup(spec, FName(project->dest_name));
    FExpand(spec);
    string_cat(spec, " $(PROG_ARGS)");
    cmd = BuildCompiler(project, spec);
    string_free(spec);
    exit_code = RunProgram(cmd, redir_stderr, redir_stdout, True);
    string_free(cmd);
    chdir(project_directory);
    if (!DontShowExitCode)
      messageBox(mfInformation | mfOKButton,
                 _("Program exit code: %d (0x%04x)"), exit_code, exit_code);
    if (ShowUserAfterExit)
    {
      TEvent event;

      event.what = evCommand;
      event.message.command = cmUserScreen;
      TProgram::application->handleEvent(event);
    }
    CheckStderr(true);
    CheckStdout(true);
  }
  return True;
}

#ifdef FILETIMEDEBUG
static __attribute__ ((__destructor__))
     void print_filetimes()
{
  if (!file_times)
    return;
  int i, count = file_times->getCount();

  fprintf(stderr, "FILETIMES(%d)\n", count);
  for (i = 0; i < count; i++)
  {
    FileTime *ft = (FileTime *) file_times->at(i);

    fprintf(stderr, "%s(%s)\n", ft->name, ft->key);
  }
}
#endif


