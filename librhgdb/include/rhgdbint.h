/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifndef __RHDGBINT_H__
#define __RHGDBINT_H__

#if __GNUC__>2 || (__GNUC__==2 && __GNUC_MINOR__>95)

//  For new GCC versions (newer than 2.95.X)

#define DEBUG_(x,...) printf_unfiltered(x, ## __VA_ARGS__ );
#if 0
#define DEBUG(x,...) fprintf(stderr,x, ## __VA_ARGS__);
#define _DEBUG(x,...) fprintf(stderr,x, ## __VA_ARGS__);
#else
#define DEBUG(x,...)
#define _DEBUG(x,...)
#endif

#else

//  For old GCC versions (2.95.X or older)

#define DEBUG_(x...) printf_unfiltered(##x);
#if 0
#define DEBUG(x...) fprintf(stderr,##x);
#define _DEBUG(x...) fprintf(stderr,##x);
#else
#define DEBUG(x...)
#define _DEBUG(x...)
#endif

#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define OLD_GDB

#ifdef UI_FILE_H
#undef OLD_GDB
#endif

#ifdef OLD_GDB
#define RH_GDB_FILE GDB_FILE
#else
#define RH_GDB_FILE struct ui_file
#endif

  void handle_gdb_command(char *);
  void reset_gdb_output();
  void reset_gdb_error();
  void init_gdb(char *);
  void done_gdb();
  void SetBreakPoints();
  void DeleteBreakPoints();
  extern void (*select_source_line) (char *fname, int line);
  void Command(char *, int call_hook);
  char *Bname(char *);
#define COMMAND(x) Command(x,1)

#ifdef __cplusplus
}
#endif

extern char *gdb_output_buffer;
extern char *gdb_error_buffer;
int get_gdb_output_buffer(void);
int get_gdb_error_buffer(void);
extern int invalid_line;
extern int last_breakpoint_number;
extern int last_breakpoint_line;
extern char *last_breakpoint_file;
extern int init_count;
extern int gdb_error;
extern char *error_start;
extern unsigned long last_breakpoint_address;
extern int reset_command;
void __StartSession();
void __BreakSession();
void __EndSession(int);
extern char *_progname;
void reset_debugger();
extern int call_reset;
void init_started();
void clear_started();
void clear_inferior();
int _inferior_pid();

#endif
