/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdbrh.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <librhgdb.h>
#include <rhgdbint.h>
#include <sys/stat.h>
#include <unistd.h>

#define TBREAK win31?"thbreak":"tbreak"

/* Whether this is the async version or not.  The async version is
   invoked on the command line with the -nw --async options.  In this
   version, the usual command_loop is substituted by and event loop which
   processes UI events asynchronously. */
int event_loop_p = 0;

static
int _debugger_started = 0;
int debugger_started()
{
  return _debugger_started;
}

#if !defined(GDB_VERSION)
#define GDB_VERSION 50
#endif

#if GDB_VERSION == 50
#define PID_TYPE int
#define NULL_PID 0
#else
#define PID_TYPE ptid_t
#define NULL_PID null_ptid
#define inferior_pid inferior_ptid

char *interpreter_p = 0;

#endif

extern PID_TYPE inferior_pid;

int _inferior_pid()
{
  return PIDGET(inferior_pid);
}

void clear_inferior()
{
  inferior_pid = NULL_PID;
}

void init_started()
{
  _debugger_started = _inferior_pid();
}

void clear_started()
{
  _debugger_started = 0;
}

int init_count = 0;

void (*post_command_hook) (void) = 0;

static int in_command = 0;

int verbose_gdb_commands = 0;

int call_reset = 0;

int force_disassembler_window = 0;

static char *start_directory = 0;

static void (*PostCommandHook) (void) = 0;

void
Command(char *x, int call_hook)
{
  in_command++;
  reset_gdb_output();
  reset_gdb_error();
  if (verbose_gdb_commands)
  {
    DEBUG_("executing command: %s\n", x);
    get_gdb_output_buffer();
    _UpdateGDBOutWin(gdb_output_buffer);
    reset_gdb_output();
  }
/* fprintf(stderr,"command: %s\n",x); */
  handle_gdb_command(x);
#if 0
  DEBUG_("inferior_pid: %d target_has_execution: %d in_command: %d\n",
         inferior_pid, target_has_execution, in_command);
#endif
  get_gdb_output_buffer();
  get_gdb_error_buffer();
  _UpdateGDBOutWin(gdb_error_buffer);
  _UpdateGDBOutWin(gdb_output_buffer);
  init_started();
  if (in_command == 1 && call_hook && post_command_hook)
    post_command_hook();
  in_command--;
  if (!in_command && call_reset)
  {
    call_reset = 0;
    in_command++;
    ResetDebugger();
    in_command--;
  }
  if (PostCommandHook)
  {
    PostCommandHook();
    PostCommandHook = 0;
  }
}

static void
set_source_directories(char **SrcDirs, int count)
{
  int i;
  char command[1000];

#if 1
  if (!SrcDirs || count == 0)
  {
    extern char *source_path;

    /*
       This is a little hack to disable GDB for finding the files 
     */
    if (source_path)
      free(source_path);
    source_path = strdup("");
    return;
  }
#else /*
         Why should this be better ? 
       */
  extern char *source_path;

  if (source_path)
    free(source_path);
  source_path = strdup("");
  Command("directory $cwd", 0);
  Command("directory $cdir", 0);
  if (start_directory)
  {
    strcpy(command, "directory ");
    strcat(command, start_directory);
    Command(command, 0);
  }
#endif
  for (i = 0; i < count; i++)
  {
    strcpy(command, "directory ");
    strcat(command, SrcDirs[i]);
    Command(command, 0);
  }
}

char *_progname = NULL;
static time_t _progtime = 0;

static void
cmdcat(char **command, char *str, int *maxlen)
{
  int len = strlen(*command) + strlen(str);

  if (len >= *maxlen)
  {
    *maxlen = 3 * len / 2 + 128;
    *command = realloc(*command, *maxlen);
    if (*command == 0)
      abort();
  }
  strcat(*command, str);
}

static int
INIT()
{
  static char gdbVersionShown = 0;
  char *command, *progname;
  int i, cmdlen = 512;
  char **argv;
  int argc;
  char **srcdirs;
  int srccount;
  struct stat st;

  _InitGDBOutWin();
  command = malloc(cmdlen);
  if (!command)
    abort();
  /*
     Show GDB version and copyright notice in GDB output window
     (of course if such is enabled). But do this only once.
     It's rather well hidden though
   */
  if (!gdbVersionShown)
  {
    Command("show version", 0);
    gdbVersionShown = 1;
  }

  /*
     We should not crash if gdb command like 'x' gets exception.
     This also could be usefull to make gdb command 'call'
     accessible in rhide
   */
  Command("set unwindonsignal on", 0);
  /*
     Change gdb's internal current_directory. This is absolutely
     needed in the case the app has changed the directory
   */
  strcpy(command, "cd ");
  getcwd(command + 3, 512 - 3);
  start_directory = strdup(command + 3);
  Command(command, 0);
  init_gdb(_GetProgName());
  init_count++;
  progname = _GetProgName();
  if (!progname)
  {
    _UserWarning(WARN_NOPROGRAM);
    init_count = 0;
    free(command);
    return 0;
  }
  if (stat(progname, &st) != 0)
  {
    _UserWarning(WARN_NOPROGRAM_FOUND);
    init_count = 0;
    free(command);
    return 0;
  }
  if (!_progname || _progtime != st.st_atime
      || strcmp(progname, _progname) != 0)
  {
    strcpy(command, "file ");
    strcat(command, _GetProgName());
    Command(command, 0);
    if (!have_partial_symbols() && !have_full_symbols())
    {
      _UserWarning(WARN_NOSYMBOLS);
      init_count = 0;
      _progname = NULL;
      free(command);
      return 0;
    }
    _progtime = st.st_atime;
    _progname = progname;
  }
  srcdirs = _GetSourceDirectories(&srccount);
  set_source_directories(srcdirs, srccount);
#if 0
  select_source_line = _select_source_line;
#endif
  argv = _GetProgramArguments(&argc);
  *command = 0;
  if (argv && argc > 0)
  {
    cmdcat(&command, "set args", &cmdlen);
    for (i = 0; i < argc; i++)
    {
      cmdcat(&command, " ", &cmdlen);
      if (strchr(argv[i], ' ') != NULL)
      {
        /*
           the arg must be quoted. What quote should be used " or ' ??
           I'm using " 
         */
        cmdcat(&command, "\"", &cmdlen);
        cmdcat(&command, argv[i], &cmdlen);
        cmdcat(&command, "\"", &cmdlen);
      }
      else
        cmdcat(&command, argv[i], &cmdlen);
    }
    Command(command, 0);
  }
  else
  {
    Command("set args", 0);
  }
  SetBreakPoints();
  free(command);
  return 1;
}

void
GoToAddress(unsigned long address)
{
  char command[256], *run;

  if (!_DoMake())
    return;
  if (!debugger_started())
  {
    run = "run";
    if (!INIT())
      return;
    __StartSession();
    ShowUserScreen();
  }
  else
  {
    run = "continue";
  }
  sprintf(command, "%s *0x%lx", TBREAK, address);
  Command(command, 0);
  COMMAND(run);
  sprintf(command, "delete %d", last_breakpoint_number);
  Command(command, 0);
  ShowDebuggerScreen();
}

void
GoToLine(char *fname, int line)
{
  char command[256], *run;
  int no_found = 0;

  if (!_DoMake())
    return;
  if (!debugger_started())
  {
    run = "run";
    if (!INIT())
      return;
    __StartSession();
    ShowUserScreen();
  }
  else
  {
    run = "continue";
  }
  sprintf(command, "%s %s:%d", TBREAK, Bname(fname), line);
  Command(command, 0);
  if (invalid_line || (no_found = strncmp(gdb_output_buffer, "No ", 3)) == 0)
  {
    _UserWarning(WARN_NOCODE);
    if (no_found == 0)
      return;
    sprintf(command, "delete %d", last_breakpoint_number);
    Command(command, 0);
  }
  else
  {
    COMMAND(run);
    sprintf(command, "delete %d", last_breakpoint_number);
    Command(command, 0);
    ShowDebuggerScreen();
  }
}

static void
step(char *cmd)
{
  if (!_DoMake())
    return;
  if (!debugger_started())
  {
    char command[100];

    if (!INIT())
      return;
    __StartSession();
    ShowUserScreen();
    strcpy(command, TBREAK);
    strcat(command, " ");
    strcat(command, _GetMainFunction());
    Command(command, 0);
    COMMAND("run");
  }
  else
  {
    COMMAND(cmd);
  }
  ShowDebuggerScreen();
}

void
Step()
{
  step("next");
}

void
Step_i()
{
  step("nexti");
}

static void
trace(char *cmd)
{
  if (!_DoMake())
    return;
  if (!debugger_started())
  {
    char command[100];

    if (!INIT())
      return;
    __StartSession();
    ShowUserScreen();
    strcpy(command, TBREAK);
    strcat(command, " ");
    strcat(command, _GetMainFunction());
    Command(command, 0);
    COMMAND("run");
  }
  else
  {
    COMMAND(cmd);
  }
  ShowDebuggerScreen();
}

void
Trace()
{
  trace("step");
}

void
Trace_i()
{
  trace("stepi");
}

void
reset_debugger()
{
  call_reset = 0;
  reset_command = 1;
  DeleteBreakPoints();
  __BreakSession();
  COMMAND("kill");
  clear_started();
  reset_command = 0;
#ifdef DJGPP
  clear_inferior();
#endif
}

void
ResetDebugger()
{
  call_reset = 0;
#if 1
  if (debugger_started() || _inferior_pid())
#endif
    reset_debugger();
  DeleteBreakPoints();
}

void
ClearSymbols()
{
  if (debugger_started() || _inferior_pid())
    ResetDebugger();
  if (init_count > 0)
    Command("file", 0);
  _progname = NULL;
}

void
Continue()
{
  if (!_DoMake())
    return;
  if (!debugger_started())
  {
    if (!INIT())
      return;
    __StartSession();
    ShowUserScreen();
    COMMAND("run");
  }
  else
  {
    COMMAND("continue");
  }
  ShowDebuggerScreen();
}

int
InitRHGDB()
{
  if (debugger_started())
    reset_debugger();
  return INIT();
}


void
SetPostCommandHook(void (*hook) (void))
{
  if (in_command > 0)
    PostCommandHook = hook;
}
