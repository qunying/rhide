/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#include <libgdb.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <librhgdb.h>
#include <rhgdbint.h>

#include <signal.h>
#include <setjmp.h>

void gdb_init();

int last_breakpoint_number = 0;
int last_breakpoint_line = 0;
char *last_breakpoint_file;
unsigned long last_breakpoint_address = 0;
int invalid_line = 0;
int gdb_error = 0;
char *error_start;
int reset_command = 0;

int switch_to_user = 1;

static int user_screen_shown = 0;
static void __UserScreen();
static void __DebuggerScreen();

static int signal_start;
static int signal_end;

void
annotate_signalled ()
{
DEBUG("|signalled|");
}

void
annotate_signal_name ()
{
DEBUG("|signal_name|");
  signal_start = gdb_output_pos;
}

void
annotate_signal_name_end ()
{
DEBUG("|signal_name_end|");
}

void
annotate_signal_string ()
{
DEBUG("|signal_string|");
}

void
annotate_signal_string_end ()
{
  char *signal;
DEBUG("|signal_string_end|");
  signal_end = gdb_output_pos;
#if 0 /* def __DJGPP__ */
  inferior_pid = 0;
#endif
  signal = (char *)alloca(signal_end-signal_start+1);
  strncpy(signal,gdb_output_buffer+signal_start,signal_end-signal_start);
  signal[signal_end-signal_start] = 0;
  if (user_screen_shown)
  {
    __DebuggerScreen();
    _UserWarning(WARN_SIGNALED,signal);
    __UserScreen();
  }
  else
    _UserWarning(WARN_SIGNALED,signal);
  call_reset = 1;
}

void
annotate_signal ()
{
DEBUG("|signal|");
}

#ifdef __DJGPP__
#include <dpmi.h>
#endif

void
annotate_exited (int exitstatus)
{
  _DEBUG("|exited(%d)|",exitstatus);
#ifdef __DJGPP__
 /* this is very important. The exit code of a djgpp program
   disables interrupts and after this there is no other interrupt
   called, which enables interrupts with the iret. */
  __dpmi_get_and_enable_virtual_interrupt_state();
#endif
  __DebuggerScreen();
  DeleteBreakPoints();
  __EndSession(exitstatus);
}

void
annotate_error ()
{
_DEBUG("|error|");
  /* gdb_error = 1; */
}

void
annotate_error_begin ()
{
_DEBUG("a_error_begin\n");
  /* error_start = gdb_output_buffer + strlen(gdb_output_buffer); */
}

void CreateBreakPointHook(struct breakpoint *b)
{
  struct symtab_and_line s = find_pc_line(b->address,0);
  last_breakpoint_number = b->number;
  invalid_line = (b->line_number != s.line);
  last_breakpoint_address = b->address;
  last_breakpoint_line = s.line;
  if (s.symtab)
    last_breakpoint_file = s.symtab->filename;
  else
    last_breakpoint_file = NULL; 
}

static int top_level_val;

#define SET_TOP_LEVEL() \
  (((top_level_val = setjmp (error_return)) \
    ? (PTR) 0 : (PTR) memcpy (quit_return, error_return, sizeof (jmp_buf))) \
   , top_level_val)

void
annotate_starting ()
{
DEBUG("|starting|");
  __UserScreen();
}

void annotate_stopped ()
{
  struct symtab_and_line s;
  char *fname = NULL;
DEBUG("|stopped|");
  __DebuggerScreen();
  debugger_started = inferior_pid;
  if (!reset_command)
  {
    s = find_pc_line(stop_pc,0);
    fname = s.symtab ? s.symtab->filename : NULL;
    _select_source_line(fname,s.line);
  }
  _DEBUG("a_stopped(%s,%d)\n",fname,s.line);
}

#define blocksize 2048

char *gdb_output_buffer = NULL;
char *gdb_error_buffer = NULL;
int gdb_output_size = 0;
int gdb_error_size = 0;
int gdb_output_pos = 0;
int gdb_error_pos = 0;

#define RESIZE(x) \
do {\
  x##_size += blocksize;\
  x##_buffer = (char *)realloc(x##_buffer,x##_size);\
} while (0)

#define APPEND(x,s) \
do {\
  int len = strlen(s);\
  if (len + x##_pos >= x##_size) RESIZE(x);\
  memcpy(x##_buffer+x##_pos,s,len);\
  x##_pos += len;\
  x##_buffer[x##_pos] = 0;\
} while (0)

#define RESET(x) \
do {\
  *x##_buffer = 0;\
  x##_pos = 0;\
} while (0)

static void __attribute__((constructor))
_init_librhgdb()
{
  char command[256];
  void (*oldINT)(int);
  oldINT = signal(SIGINT,SIG_DFL);
  gdb_init();
  signal(SIGINT,oldINT);
  {
    extern int rh_annotate;
    extern int watchdog;
    rh_annotate = 0; /* This is used only to force to have annotate.o
                        known to the linker, otherwise you have to
                        link your program with this lib here like
                        -lrhgdb -lgdb -lrhgdb
                     */
    watchdog = 0;
  }
  sprintf(command,"set width %u",UINT_MAX);
  Command(command,0);
  sprintf(command,"set height %u",UINT_MAX);
  Command(command,0);
  Command("set print null-stop",0);
}

void init_gdb(char *fname)
{
  reset_gdb_output();
  reset_gdb_error();
  create_breakpoint_hook = CreateBreakPointHook;
}

void done_gdb()
{
  target_kill();
  target_close(1);
  create_breakpoint_hook = NULL;
}

void handle_gdb_command(char *command)
{
  jmp_buf old_quit_return,old_error_return;
  /* Save the old jmp_buf's, because we may be
     called nested */
  memcpy(old_quit_return,quit_return,sizeof(jmp_buf));
  memcpy(old_error_return,error_return,sizeof(jmp_buf));
  gdb_error = 0;
  _DEBUG("start of handle_gdb_command(%s)\n",command);
  if (!SET_TOP_LEVEL())
  {
    execute_command(command,0);
  }
  _DEBUG("end of handle_gdb_command(%s)\n",command);
  /* Restore the old jmp_buf's */
  memcpy(quit_return,old_quit_return,sizeof(jmp_buf));
  memcpy(error_return,old_error_return,sizeof(jmp_buf));
}

void
fputs_unfiltered (linebuffer, stream)
     const char *linebuffer;
     FILE *stream;
{
  _DEBUG("fputs_unfiltered(%s)\n",linebuffer);
  APPEND(gdb_output,linebuffer);
}

void reset_gdb_output()
{
  if (!gdb_output_buffer) RESIZE(gdb_output);
  RESET(gdb_output);
}

void reset_gdb_error()
{
  if (!gdb_error_buffer) RESIZE(gdb_error);
  RESET(gdb_error);
}

/* If nonzero, display time usage both at startup and for each command.  */

int display_time;

/* If nonzero, display space usage both at startup and for each command.  */

int display_space;

void init_proc()
{
}

char *SourceForMain(int *line)
{
  struct symbol *sym;
  struct symtab *symtab;
  *line = 0;
  sym = lookup_symbol(_GetMainFunction(), NULL, VAR_NAMESPACE, NULL, &symtab);
  if (!sym)
  {
    struct minimal_symbol *msymbol =
      lookup_minimal_symbol (_GetMainFunction(), NULL, NULL);
    if (msymbol)
      *line = SYMBOL_VALUE_ADDRESS(msymbol);
    return NULL;
  }
  if (!symtab || !symtab->filename)
  {
    *line = BLOCK_START(SYMBOL_BLOCK_VALUE(sym));
    return NULL;
  }
  if (symtab->linetable)
  {
    int i;
    for (i=0;i<symtab->linetable->nitems;i++)
    {
      if (symtab->linetable->item[i].pc
          == BLOCK_START(SYMBOL_BLOCK_VALUE(sym)))
      {
        *line = symtab->linetable->item[i].line;
        break;
      }
    }
  }
  else
    *line = 0;
  return symtab->filename;
}

void __StartSession()
{
  _StartSession();
}

void __BreakSession()
{
  _BreakSession();
}

void __EndSession(int exit_code)
{
  debugger_started = 0;
#ifdef __DJGPP__
  inferior_pid = 0;
#endif
  _EndSession(exit_code);
}

static void __DebuggerScreen()
{
DEBUG("|DebuggerScreen|");
  if (user_screen_shown)
  {
    _DebuggerScreen();
    user_screen_shown = 0;
  }
}

static void __UserScreen()
{
DEBUG("|UserScreen|");
  if (switch_to_user)
  {
    if (!user_screen_shown)
      _UserScreen();
    user_screen_shown = 1;
  }
}



