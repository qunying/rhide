/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdbrh.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <librhgdb.h>
#include <rhgdbint.h>

#include <signal.h>
#include <setjmp.h>

#ifdef OLD_GDB
void gdb_init(char *);
#else
extern void error_init(void);
void gdb_init(char *argv0);
#endif

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

static int signal_start;
static int signal_end;

void
annotate_signalled()
{
  DEBUG("|signalled|");
}

void
annotate_signal_name()
{
  DEBUG("|signal_name|");
  signal_start = get_gdb_output_buffer();
}

void
annotate_signal_name_end()
{
  DEBUG("|signal_name_end|");
}

void
annotate_signal_string()
{
  DEBUG("|signal_string|");
}

void
annotate_signal_string_end()
{
  char *signal;

  DEBUG("|signal_string_end|");
  signal_end = get_gdb_output_buffer();
  signal = (char *) alloca(signal_end - signal_start + 1);
  strncpy(signal, gdb_output_buffer + signal_start,
          signal_end - signal_start);
  signal[signal_end - signal_start] = 0;
  if (user_screen_shown)
  {
    ShowDebuggerScreen();
    _UserWarning(WARN_SIGNALED, signal);
    ShowUserScreen();
  }
  else
    _UserWarning(WARN_SIGNALED, signal);
  // An attempt to recover after SIGINT and other signals

  if (1 ||                      /*
                                   Why this ? 
                                 */
      strncmp(signal, "SIGINT,", 7) == 0
      || strncmp(signal, "SIGFPE,", 7) == 0
      || strncmp(signal, "SIGABRT,", 8) == 0
      || strncmp(signal, "SIGSEGV,", 8) == 0
      || strncmp(signal, "SIGILL,", 8) == 0)
  {
    force_disassembler_window = 1;
  }
  else
  {
    call_reset = 1;
  }
}

void
annotate_signal()
{
  DEBUG("|signal|");
}

#ifdef __DJGPP__
#include <dpmi.h>
#endif

void
annotate_exited(int exitstatus)
{
  _DEBUG("|exited(%d)|", exitstatus);
#ifdef __DJGPP__
  /*
     this is very important. The exit code of a djgpp program
     disables interrupts and after this there is no other interrupt
     called, which enables interrupts with the iret. 
   */
  __dpmi_get_and_enable_virtual_interrupt_state();
#endif
  ShowDebuggerScreen();
  DeleteBreakPoints();
  __EndSession(exitstatus);
}

void
annotate_error()
{
  _DEBUG("|error|");
  /*
     gdb_error = 1; 
   */
}

void
annotate_error_begin()
{
  _DEBUG("a_error_begin\n");
  /*
     error_start = gdb_output_buffer + strlen(gdb_output_buffer); 
   */
}

void
CreateBreakPointHook(struct breakpoint *b)
{
  struct symtab_and_line s = find_pc_line(b->address, 0);

  last_breakpoint_number = b->number;
  invalid_line = (b->line_number != s.line);
  last_breakpoint_address = b->address;
  last_breakpoint_line = s.line;
  if (s.symtab)
    last_breakpoint_file = s.symtab->filename;
  else
    last_breakpoint_file = NULL;
}

#ifdef OLD_GDB
static int top_level_val;

#define SET_TOP_LEVEL() \
  (((top_level_val = setjmp (error_return)) \
    ? (PTR) 0 : (PTR) memcpy (quit_return, error_return, sizeof (jmp_buf))) \
   , top_level_val)
#endif

void
annotate_starting()
{
  DEBUG("|starting|");
  ShowUserScreen();
}

void
annotate_stopped()
{
  struct symtab_and_line s;
  char *fname = NULL;

  DEBUG("|stopped|");
  ShowDebuggerScreen();
  init_started();
  if (!reset_command)
  {
    char *dirname = NULL, *fullname = NULL;
    s = find_pc_line(stop_pc, 0);
    if (s.symtab != NULL)
    {
      fname = s.symtab->filename;
      dirname = s.symtab->dirname;
      fullname = s.symtab->fullname;
    }
    _DEBUG("select_source_line(%s,%s,%s)\n", fname, dirname, fullname);
    _select_source_line(fname, s.line, dirname, fullname);
  }
  _DEBUG("a_stopped(%s,%d)\n", fname, s.line);
}

char *gdb_output_buffer = NULL;
char *gdb_error_buffer = NULL;
long gdb_output_pos = 0;
long gdb_error_pos = 0;

#ifdef OLD_GDB
#define blocksize 2048

long gdb_output_size = 0;
long gdb_error_size = 0;

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

#endif

/* Moved from gdb-4.18  (gdb/main.c) as main.c does not go into libgdb.a  */
/* Whether this is the command line version or not */
int tui_version = 0;

/* Whether xdb commands will be handled */
int xdb_commands = 0;

/* Whether dbx commands will be handled */
int dbx_commands = 0;

RH_GDB_FILE *gdb_stdout;
RH_GDB_FILE *gdb_stderr;
RH_GDB_FILE *gdb_stdlog;
RH_GDB_FILE *gdb_stdtarg;

int
get_gdb_output_buffer(void)
{
#ifndef OLD_GDB
  if (gdb_output_buffer)
    free(gdb_output_buffer);
  gdb_output_buffer = ui_file_xstrdup(gdb_stdout, &gdb_output_pos);
#endif
  return gdb_output_pos;
}

int
get_gdb_error_buffer(void)
{
#ifndef OLD_GDB
  if (gdb_error_buffer)
    free(gdb_error_buffer);
  gdb_error_buffer = ui_file_xstrdup(gdb_stderr, &gdb_error_pos);
#endif
  return gdb_error_pos;
}

#if defined(__DJGPP__) && defined(INCLUDE_WIN31_HACK)

/* Here comes the hack for Windows 3.1x.
   The case for setting breakpoints s handled in librhgdb by allowing
   only up to 3 hw breakpoints. But GDB uses for executing the step/next
   commands internal breakpoints, which are set by target_insert_breakpoint
   and deleteted by target_delete_breakpoint.
   Under DJGPP I can assume that we are using only the go32 target, so I
   overwrite the target ops to inserting/deleting every time a hw
   breakpoint.
*/

#include <dpmi.h>

extern int win31;               /*

                                   in librhgdb 
                                 */
static struct target_ops *_go32_ops;

static int
_win31_memory_insert_breakpoint(CORE_ADDR addr,
                                char *shadow __attribute__ ((unused)))
{
  return go32_insert_hw_breakpoint(addr, 0	/*
	   this is not used 
	 */ );
}

static int
_win31_memory_remove_breakpoint(CORE_ADDR addr,
                                char *shadow __attribute__ ((unused)))
{
  return go32_remove_hw_breakpoint(addr, 0	/*
	   this is not used 
	 */ );
}

/* Pointer to array of target architecture structures; the size of the
   array; the current index into the array; the allocated size of the 
   array.  */
extern struct target_ops **target_structs;
extern unsigned target_struct_size;

void
__win31_hack()
{
  __dpmi_regs r;
  unsigned i;

  _go32_ops = NULL;
  r.x.ax = 0x160a;
  __dpmi_int(0x2f, &r);
  if (r.x.ax == 0 && r.h.bh == 3)
    win31 = 1;
  else
    win31 = 0;
  if (!win31)
    return;
  for (i = 0; i < target_struct_size; i++)
  {
    if ((target_structs[i]->to_shortname) &&
        (strcmp(target_structs[i]->to_shortname, "djgpp") == 0))
    {
      _go32_ops = target_structs[i];
      break;
    }
  }
  if (!_go32_ops)
    return;
  _go32_ops->to_insert_breakpoint = _win31_memory_insert_breakpoint;
  _go32_ops->to_remove_breakpoint = _win31_memory_remove_breakpoint;
}

#endif /*
          __DJGPP__ 
        */

static void __attribute__ ((constructor)) _init_librhgdb()
{
  char command[256];
  void (*oldINT) (int);

  oldINT = signal(SIGINT, SIG_DFL);

#ifdef OLD_GDB
  gdb_stdout = (GDB_FILE *) malloc(sizeof(GDB_FILE));
  gdb_stdout->ts_streamtype = afile;
  gdb_stdout->ts_filestream = stdout;
  gdb_stdout->ts_strbuf = NULL;
  gdb_stdout->ts_buflen = 0;

  gdb_stderr = (GDB_FILE *) malloc(sizeof(GDB_FILE));
  gdb_stderr->ts_streamtype = afile;
  gdb_stderr->ts_filestream = stderr;
  gdb_stderr->ts_strbuf = NULL;
  gdb_stderr->ts_buflen = 0;
  gdb_init("rhide");
#else

  /*
     fragment from gdb/main.c  
   */

  gdb_stdout = mem_fileopen();  // stdio_fileopen (stdout);
  gdb_stderr = mem_fileopen();  // stdio_fileopen (stderr);
  gdb_stdlog = gdb_stderr;      /*
                                   for moment 
                                 */
  gdb_stdtarg = gdb_stderr;     /*
                                   for moment 
                                 */
  error_init();

  /*
     end of fragment from gdb/main.c  
   */

  gdb_init("rhide");
#endif
#if defined(__DJGPP__) && defined(INCLUDE_WIN31_HACK)
  __win31_hack();
#endif
  signal(SIGINT, oldINT);
  {
    extern int rh_annotate;

    rh_annotate = 0;            /*
                                   This is used only to force to have annotate.o
                                   known to the linker, otherwise you have to
                                   link your program with this lib here like
                                   -lrhgdb -lgdb -lrhgdb
                                 */
  }
  sprintf(command, "set width %u", UINT_MAX);
  Command(command, 0);
  sprintf(command, "set height %u", UINT_MAX);
  Command(command, 0);
  Command("set print null-stop", 0);
#if 0
  Command("set print object on", 0);
#endif
  Command("set print vtbl on", 0);
}

void
init_gdb(char *fname __attribute__ ((unused)))
{
  reset_gdb_output();
  reset_gdb_error();
  create_breakpoint_hook = CreateBreakPointHook;
}

void
done_gdb()
{
  target_kill();
  target_close(1);
  create_breakpoint_hook = NULL;
}

void
handle_gdb_command(char *command)
{
  /*
     Strings may not be writtable. Therefore let's make local copy  
   */
  char *copy_of_command = strdup(command);

#ifdef OLD_GDB
  jmp_buf old_quit_return, old_error_return;

  /*
     Save the old jmp_buf's, because we may be
     called nested 
   */
  memcpy(old_quit_return, quit_return, sizeof(jmp_buf));
  memcpy(old_error_return, error_return, sizeof(jmp_buf));
  gdb_error = 0;
  _DEBUG("start of handle_gdb_command(%s)\n", command);
  if (!SET_TOP_LEVEL())
  {
    execute_command(copy_of_command, 0);
  }
  _DEBUG("end of handle_gdb_command(%s)\n", command);
  /*
     Restore the old jmp_buf's 
   */
  memcpy(quit_return, old_quit_return, sizeof(jmp_buf));
  memcpy(error_return, old_error_return, sizeof(jmp_buf));
#else
  /*
     (GDB-5.0 is removing white space at end of string. So it's     
   */
  /*
     modifying it  
   */
  catch_command_errors(execute_command, copy_of_command, 0, RETURN_MASK_ALL);
#endif
  free(copy_of_command);
}

#ifdef OLD_GDB
void
fputs_unfiltered(const char *linebuffer, GDB_FILE * stream
                 __attribute__ ((unused)))
{
  _DEBUG("fputs_unfiltered(%s)\n", linebuffer);
  APPEND(gdb_output, linebuffer);
}
#endif

void
reset_gdb_output()
{
#ifdef OLD_GDB
  if (!gdb_output_buffer)
    RESIZE(gdb_output);
  RESET(gdb_output);
#else
  ui_file_rewind(gdb_stdout);
  if (gdb_output_buffer)
    *gdb_output_buffer = 0;
#endif
}

void
reset_gdb_error()
{
#ifdef OLD_GDB
  if (!gdb_error_buffer)
    RESIZE(gdb_error);
  RESET(gdb_error);
#else
  ui_file_rewind(gdb_stderr);
  if (gdb_error_buffer)
    *gdb_error_buffer = 0;
#endif
}

/* If nonzero, display time usage both at startup and for each command.  */

int display_time;

/* If nonzero, display space usage both at startup and for each command.  */

int display_space;

void
init_proc()
{
}

char *
SourceForMain(int *line, char **dirname)
{
  struct symbol *sym;
  struct symtab *symtab;

  *dirname = NULL;

  *line = 0;
  sym = lookup_symbol(_GetMainFunction(), NULL, VAR_NAMESPACE, NULL, &symtab);
  if (!sym)
  {
    struct minimal_symbol *msymbol =
      lookup_minimal_symbol(_GetMainFunction(), NULL, NULL);

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

    for (i = 0; i < symtab->linetable->nitems; i++)
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
  *dirname = symtab->dirname;
  return symtab->filename;
}

void
__StartSession()
{
  _StartSession();
}

void
__BreakSession()
{
  _BreakSession();
}

void
__EndSession(int exit_code)
{
  clear_started();
#ifdef __DJGPP__
  clear_inferior();
#endif
  _EndSession(exit_code);
}

void
ShowDebuggerScreen()
{
  DEBUG("|DebuggerScreen|");
  if (user_screen_shown)
  {
    _DebuggerScreen();
    user_screen_shown = 0;
  }
}

void
ShowUserScreen()
{
  DEBUG("|UserScreen|");
  if (switch_to_user)
  {
    if (!user_screen_shown)
      _UserScreen();
    user_screen_shown = 1;
  }
}

const char *gdb_version()
{
  return version;
}

