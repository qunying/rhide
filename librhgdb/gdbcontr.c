/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <librhgdb.h>
#include <rhgdbint.h>
#include <sys/stat.h>
#include <unistd.h>

#define TBREAK win31?"thbreak":"tbreak"

int debugger_started = 0;

int init_count = 0;

extern int inferior_pid;

void (*post_command_hook)(void) = 0;

static int in_command = 0;

int verbose_gdb_commands = 0;

int call_reset = 0;

int force_disassembler_window = 0;

void Command(char *x,int call_hook)
{
  in_command++;
  reset_gdb_output();
  reset_gdb_error();
  if (verbose_gdb_commands)
  {
    DEBUG_("executing command: %s\n",x);
    _UpdateGDBOutWin(gdb_output_buffer);
    reset_gdb_output();
  }
/* fprintf(stderr,"command: %s\n",x); */
  handle_gdb_command(x);
#if 0
  DEBUG_("inferior_pid: %d target_has_execution: %d in_command: %d\n",inferior_pid,target_has_execution,in_command);
#endif
  _UpdateGDBOutWin(gdb_error_buffer);
  _UpdateGDBOutWin(gdb_output_buffer);
  debugger_started = inferior_pid;
  if (in_command == 1 && call_hook && post_command_hook) post_command_hook(); 
  in_command--;
  if (!in_command && call_reset)
  {
    call_reset = 0;
    in_command++;
    ResetDebugger();
    in_command--;
  }
}

static void set_source_directories(char **SrcDirs,int count)
{
  int i;
  char command[1000];
  if (!SrcDirs || count == 0)
  {
    extern char *source_path;
    /* This is a little hack to disable GDB for finding the files */
    if (source_path) free(source_path);
    source_path = strdup("");
    return;
  }
  for (i=0;i<count;i++)
  {
    strcpy(command,"directory ");
    strcat(command,SrcDirs[i]);
    Command(command,0);
  }
}

char *_progname = NULL;
static time_t _progtime = 0;

static int INIT()
{
  char command[512],*progname;
  int i;
  char **argv;
  int argc;
  char **srcdirs;
  int srccount;
  struct stat st;
  _InitGDBOutWin();
  /*
    Change gdb's internal current_directory. This is absolutely
    needed in the case the app has changed the directory
  */
  strcpy(command,"cd ");
  getcwd(command+3,512-3);
  Command(command,0);
  init_gdb(_GetProgName());
  init_count++;
  progname = _GetProgName();
  if (!progname)
  {
    _UserWarning(WARN_NOPROGRAM);
    init_count = 0;
    return 0;
  }
  if (stat(progname,&st) != 0)
  {
    _UserWarning(WARN_NOPROGRAM_FOUND);
    init_count = 0;
    return 0;
  }
  if (!_progname || _progtime != st.st_atime || strcmp(progname,_progname) != 0)
  {
    strcpy(command,"file ");
    strcat(command,_GetProgName());
    Command(command,0);
    if (!have_partial_symbols() && !have_full_symbols())
    {
      _UserWarning(WARN_NOSYMBOLS);
      init_count = 0;
      _progname = NULL;
      return 0;
    }
    _progtime = st.st_atime;
    _progname = progname;
  }
  srcdirs = _GetSourceDirectories(&srccount);
  set_source_directories(srcdirs,srccount);
#if 0
  select_source_line = _select_source_line;
#endif
  argv = _GetProgramArguments(&argc);
  if (argv && argc > 0)
  {
    strcpy(command,"set args");
    for (i=0;i<argc;i++)
    {
      strcat(command," ");
      if (strchr(argv[i],' ') != NULL)
      {
        /* the arg must be quoted. What quote should be used " or ' ??
           I'm using " */
        strcat(command,"\"");
        strcat(command,argv[i]);
        strcat(command,"\"");
      }
      else
        strcat(command,argv[i]);
    }
    Command(command,0);
  }
  else
  {
    Command("set args",0);
  }
  SetBreakPoints();
  return 1;
}

void GoToAddress(unsigned long address)
{
  char command[256],*run;
  if (!_DoMake()) return;
  if (!debugger_started)
  {
    run = "run";
    if (!INIT()) return;
    __StartSession();
  }
  else
  {
    run = "continue";
  }
  sprintf(command,"%s *0x%lx",TBREAK,address);
  Command(command,0);
  COMMAND(run);
  sprintf(command,"delete %d",last_breakpoint_number);
  Command(command,0);
}

void GoToLine(char *fname,int line)
{
  char command[256],*run;
  int no_found = 0;
  if (!_DoMake()) return;
  if (!debugger_started)
  {
    run = "run";
    if (!INIT()) return;
    __StartSession();
  }
  else
  {
    run = "continue";
  }
  sprintf(command,"%s %s:%d",TBREAK,Bname(fname),line);
  Command(command,0);
  if (invalid_line || (no_found = strncmp(gdb_output_buffer,"No ",3)) == 0)
  {
    _UserWarning(WARN_NOCODE);
    if (no_found == 0) return;
    sprintf(command,"delete %d",last_breakpoint_number);
    Command(command,0);
  }
  else
  {
    COMMAND(run);
    sprintf(command,"delete %d",last_breakpoint_number);
    Command(command,0);
  }
}  
    
static void step(char *cmd)
{
  if (!_DoMake()) return;
  if (!debugger_started)
  {
    char command[100];
    if (!INIT()) return;
    __StartSession();
    strcpy(command,TBREAK);
    strcat(command," ");
    strcat(command,_GetMainFunction());
    Command(command,0);
    COMMAND("run");
  }
  else
  {
    COMMAND(cmd);
  }
}

void Step()
{
  step("next");
}

void Step_i()
{
  step("nexti");
}

static void trace(char *cmd)
{
  if (!_DoMake()) return;
  if (!debugger_started)
  {
    char command[100];
    if (!INIT()) return;
    __StartSession();
    strcpy(command,TBREAK);
    strcat(command," ");
    strcat(command,_GetMainFunction());
    Command(command,0);
    COMMAND("run");
  }
  else
  {
    COMMAND(cmd);
  }
}

void Trace()
{
  trace("step");
}

void Trace_i()
{
  trace("stepi");
}

void reset_debugger()
{
  call_reset = 0;
  reset_command = 1;
  DeleteBreakPoints();
  __BreakSession();
  COMMAND("kill");
  debugger_started = 0;
  reset_command = 0;
}

void ResetDebugger()
{
  call_reset = 0;
#if 1
  if (debugger_started || inferior_pid)
#endif
    reset_debugger();
  DeleteBreakPoints();
}

void ClearSymbols()
{
  if (debugger_started || inferior_pid)
    ResetDebugger();
  if (init_count > 0) Command("file",0);
  _progname = NULL;
}

void Continue()
{
  if (!_DoMake()) return;
  if (!debugger_started)
  {
    if (!INIT()) return;
    __StartSession();
    COMMAND("run");
  }
  else
  {
    COMMAND("continue");
  }
}

int InitRHGDB()
{
  if (debugger_started) reset_debugger();
  return INIT();
}

