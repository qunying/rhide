/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifndef __LIBRHGDB__H_
#define __LIBRHGDB__H_

/* 
  Warning codes from libtvgdb:
*/

/*
  Warn, when a breakpoint was set at a location, for which was no code
  generated
*/
#define WARN_NOCODE        1

/*
  Warn, when a breakpoint was found, which is invalid (in a file, which
  has no debugging information, ...)
*/
#define WARN_INVALID_BREAK 2

/*
  Warn, when the user has not defined a program_name (see _GetProgName())
*/
#define WARN_NOPROGRAM 3

/*
  Warn, when the program cannot be found.
*/
#define WARN_NOPROGRAM_FOUND 4

/*
  Warn, when no debugging symbols were found.
*/
#define WARN_NOSYMBOLS 5

/*
  Warn, when a breakpoint could not enabled
  This can be happen only under Windows 3.1x
*/
#define WARN_NOT_ENABLED 6

/*
  Warn, when a breakpoint could not set
  This can be happen only under Windows 3.1x
*/
#define WARN_NO_BREAK_AVAILABLE 7

/*
  Message, that the program got a signal which has killed it.
*/
#define WARN_SIGNALED 8

#ifdef __cplusplus
extern "C"
{
#endif

/*
  This kills the debugged program
*/
  void ResetDebugger();

/*
  This unloads the symbol-tabel. This is sometimes needed, if you want
  to overwrite the program (new linking) and it is opened and it frees
  also a lot of memory.
*/
  void ClearSymbols();

/*
  GDB command: next
*/
  void Step();

/*
  GDB command: finish
*/
  void Finish();

/*
  GDB command: step
*/
  void Trace();

/*
  GDB command: nexti
*/
  void Step_i();

/*
  GDB command: stepi
*/
  void Trace_i();

/*
  Set a temp. breakpoint at line 'line' of file 'fname' and continue
  execution until this or an other break/watchpoint is hit.
*/
  void GoToLine(char *fname, int line);

/*
  GDB command: continue
*/
  void Continue();

/*
  Initializes the GDB interface. You must define at least the function
  '_GetProgName'. Returns zero on error.
*/
  int InitRHGDB();

/*
  If some action should be postponed after return from GDB command.
  user can set procedure to run once at exit from command
*/

  void SetPostCommandHook(void (*hook) (void));

/*
  Returns the filename for the function 'main' and puts in 'line' the
  linenumber, where it starts.
  Store optionally the compilation directory in 'dirname' if
  it is known.
*/
  char *SourceForMain(int *line, char **dirname);

/*
  Nonzero, if the debugger has started, otherwise zero.
*/
  int debugger_started();


/*
  Non zero when annotate_signal_string_end() allows to continue execution
  of program to force showing disassembler window (We cannot be sure
  we'll be in some procedure with source). Cleared when disassembler
  window is shown.
*/

  extern int force_disassembler_window;

/*
  This is normaly set to zero. If it is set to nonzero by
  the program, which uses this lib, only 3 hw breakpoints
  are available (1 is reserved for the temp bp when stepping)
  The application is responsible for detecting if running
  under Windows 3.1x and setting this variable.
*/
  extern int win31;

/*
  This is a hook, which is called after most commands to enable the
  user inteface to update some things, which may be changed after
  executing a GDB command.
*/
  extern void (*post_command_hook) (void);

/*
   This struct is used for the backtrace command
   DO NOT MODIFY ANY OF THE FIELDS OR VARIABLES.
   THIS IS DONE ONLY IN FRAME.C FROM THAT LIB.
*/

  typedef struct
  {
    char *file_name;
    char *function_name;
    char *args;
    int line_number;
    unsigned long address;
  }
  frame_entry;

  extern frame_entry **frames;
  extern int frame_count;

/*
  This functions does the gdb command 'backtrace' and fills the
  variables 'frames' and 'frame_count'. DO NOT MODIFY THEM.
*/
  void BackTrace();

/*
  This functions does the gdb command 'f %d' 
*/
  void SetFrame(int level);

#define B_ENABLED   0x0001      /*
                                   is this brakpoint enabled? 
                                 */
#define B_LINE      0x0002      /*
                                   is it a breakpoint at a line number? 
                                 */
#define B_FUNCTION  0x0004      /*
                                   is it a breakpoint at a function? 
                                 */
#define B_CONDITION 0x0008      /*
                                   has this breakpoint a condition? 
                                 */
#define B_COUNT     0x0010      /*
                                   has this breakpoint an ignore count? 
                                 */
#define B_HARD      0x0020      /*
                                   is it a hardware breakpoint? 
                                 */

  struct BreakPoint
  {
    unsigned short type;        /*
                                   B_xxx above 
                                 */
    int number;                 /*
                                   this is the breakpoint number used by GDB. Do not modify it
                                 */
    unsigned long address;      /*
                                   the real address of this breakpoint 
                                 */
    char *filename;             /*
                                   valid, if B_LINE, possible valid if B_FUNCTION 
                                 */
    char *condition;            /*
                                   valid, if B_CONDITION 
                                 */
    char *function;             /*
                                   valid, if B_FUNCTION 
                                 */
    int count;                  /*
                                   valid, if B_COUNT 
                                 */
    int line_number;            /*
                                   valid, if B_LINE, possible valid if B_FUNCTION 
                                 */
  };

#define BREAK_ENABLED(bp) ((bp->type & B_ENABLED) == B_ENABLED)
#define BREAK_DISABLED(bp) ((bp->type & B_ENABLED) == 0)
#define BREAK_LINE(bp) ((bp->type & B_LINE) == B_LINE)
#define BREAK_CONDITION(bp) ((bp->type & B_CONDITION) == B_CONDITION)
#define BREAK_COUNT(bp) ((bp->type & B_COUNT) == B_COUNT)
#define BREAK_HW(bp) ((bp->type & B_HARD) == B_HARD)
#define BREAK_FUNCTION(bp) ((bp->type & B_FUNCTION) == B_FUNCTION)


/*
  All breakpoint numbers in libtvgdb are used only internal. They are
  not the same as used by GDB.
*/

/*
  Add a breakpoint at line 'line' of file 'fname'
*/
  int AddBreakPointLine(char *fname, int line);

/*
  Removes the breakpoint 'bpnumber'
*/
  void RemoveBreakPoint(int);

/*
  Disable breakpoint 'bpnumber'. The breakpoint is not removed, so it
  can be enabled later with 'EnableBreakPoint'
*/
  void DisableBreakPoint(int);

/*
  Enable the breakpoint 'bpnumber'
*/
  void EnableBreakPoint(int bpnumber);

/*
  Set the ignore count for brekpoint 'bpnumber'. This is needed,
  because the ignore count is not reset by GBD after it is reached.
*/
  void SetBreakCount(int bpnumber);

/*
  Returns the breakpoint 'bpnumber'
*/
  struct BreakPoint *GetBreakPoint(int bpnumber);

/*
  Edit a breakpoint, which was returned by 'GetBreakPoint' (use the same
  'bpnumber') or set a new breakpoint by setting 'bpnumber' to -1 
*/
  void EditBreakPoint(struct BreakPoint *bp, int bpnumber);

/*
  Return the count of currently used breakpoints
*/
  int BreakPointCount();

/*
  Return nonzero, if at line 'line' in file 'fname' a breakpoint,
  otherwise zero.
*/
  int IsBreakPointLine(char *fname, int line);

/*
  This is the interface to the GDB command
    print expr
  It returns the result or NULL, if not available.
  The returned value is not newly allocated, it points to
  a statically buffer which might be overwritten with any next
  call to any GDB-releated function!

  If force is 0 then EvaluateWatch immediatelly returns 0 when not in
  debugger, otherwise it forwards print command to libgdb anyway
 */
  char *EvaluateWatch(const char *expr, int force);

/* 
   This is similiar to the GDB command:
   set variable expr
*/
  char *SetValue(char *var, char *expr);

/* 
   This functions tries to make a traceback line like '  0x00001234'
   more readable by returning the corresponding symbols and/or
   file in 'function', 'file' and 'line'. If one of the return
   values could not found, it will be "" and line will be 0.
   The strings in `function` and `file` will be allocated and
   should be freed by the caller. If there was no function or file
   was found, `function` and/or `file` will be set to NULL.
*/
  void symify(const char *s, char **function, char **file, int *line,
              int diff);

/* 
  This function returns in 'list' a array of 'function_entry' with function
  names which match the regular expression 'regex' or all if NULL.
  'count' is set to the count of the strings in 'list'.
  Do not modify the pointers and do not free them. This done internal.
  It returns only funtions, for which debugging information is
  available. 
*/

  struct function_entry
  {
    char *demangled_name;
    char *linkage_name;
    char *file_name;
    char *return_name;
    int line_number;
  };

  void ListFunctions(const char *regex, struct function_entry **list,
                     int *count);

/*
  The following variables are pointers to functions, which have to
  be declared by the user in the front end application. If they are
  not set, default functions are used, which are almost dummies, which
  do nothing. 
*/

/*
  This function is called, when the line number 'line' of file 'fname'
  should be displayed.
  if 'fullname' is not not NULL, it is the full name provided
  by gdb (probably from the debug info).
  if 'dirname' is not NULL, it is the compilation directory.
*/
  extern void (*_select_source_line) (char *fname, int line,
                                      char *dirname, char *fullname);
/*
  Return the name of the program, being debugged
*/
  extern char *(*_GetProgName) (void);
/*
  Return an array of strings with the source directories and setup
  the count in 'count', or return NULL.
  If you have defined '_select_source_line' to show the correct source
  line, it is betther (for speed) not to define this function, because
  GDB is very slow in finding the source line.
*/
  extern char **(*_GetSourceDirectories) (int *count);
/*
  Return an array of strings with the arguments for the program
  being debugged and setup the count in 'count', or return NULL
*/
  extern char **(*_GetProgramArguments) (int *count);
/*
  This function should return nonzero, if the the program is up to date,
  else zero
*/
  extern int (*_DoMake) (void);
/*
  This function is called to initialize the outputs from GDB
*/
  extern void (*_InitGDBOutWin) (void);
/*
  This function is called with the outputs from GDB
*/
  extern void (*_UpdateGDBOutWin) (char *);
/*
  This function is called when a warning shuld be displayed. 'number'
  if one the 'WARN_...' and with some arguments. Currently the other
  argument is set only by the breakpoint warning to the struct BreakPoint *
  and WARN_STOPPED to the signal name
*/
  extern void (*_UserWarning) (int number, ...);
/*
  This function should switch to the user screen
*/
  extern void (*_UserScreen) (void);
/*
  This function should switch to the debugger screen (and save the contents
  of the user screen)
*/
  extern void (*_DebuggerScreen) (void);
/*
  This function is called, when the debugger session starts
*/
  extern void (*_StartSession) (void);
/*
  This function is called with the exit value, when the debuggee
  exits
*/
  extern void (*_EndSession) (int);
/*
  This function is called, when the debugger session is canceled
*/
  extern void (*_BreakSession) (void);

/*
  Set this to nonzero, if you want to see, which commands are
  executed by libgdb. The output is sent to _UpdateGDBOutWin()
*/
  extern int verbose_gdb_commands;

/*
  Set this to zero, if you don't want to switch to the user screen
  when running/stepping the debuggee.
  WARNING: You should know what will happen.
*/
  extern int switch_to_user;

/*
  This function should return the name of the main function of the
  program. Normally this should be 'main', but GPC and G77 use
  other main functions.
*/

  extern char *(*_GetMainFunction) (void);

/*
  This function disassemble from address 'address' and returns
  an array of strings with 'line_count' lines.
  Set 'mark_start' to nonzero, if you want to get symbolic markers
  for the begin of a function on a single line.
  Set 'with_source' to nonzero (meaningless when 'mark_start' is
  zero) to include in the marks also the source file and line number
  if available.
  In 'next_address' the next address is returned
*/
  const char *const *disassemble(unsigned long address, int *line_count,
                                 int mark_start, int with_source,
                                 unsigned long *next_address);

/*
  Set a temp. breakpoint at address 'address' and continue
  execution until this or an other break/watchpoint is hit.
*/
  void GoToAddress(unsigned long address);

/*
  Return the number of known processor registers.
*/
  int register_count();

/*
  Return the name of the processor register 'num'.
*/
  const char *register_name(int num);

/*
  Return the contents of the processor register 'num'.
*/
  unsigned long get_register_value(int num);

/*
  Return the contents of the FPU register 'num'.
*/
  long double get_float_register_value(int num);

/*
  Set processor register 'num' to 'value'.
*/
  void set_register_value(int num, unsigned long value);

/*
  Set FPU register 'num' to 'value'.
*/
  void set_float_register_value(int num, long double value);

/*
  Return the size of processor/FPU register 'num' in bytes.
*/
  int get_register_size(int num);

  void ShowUserScreen(void);

  void ShowDebuggerScreen(void);

/*
  Return nonzero, if the given register number is a float register
*/
  int is_float_reg(int num);

/*
  Return the gdb version as a string
*/
  const char *gdb_version();

#ifdef __cplusplus
}
#endif

#endif
