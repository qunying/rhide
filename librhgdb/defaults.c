/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <librhgdb.h>

static void
__select_source_line(char *fname __attribute__ ((unused)),
                     int lineno __attribute__ ((unused)),
                     char *dirname __attribute__((unused)),
                     char *fullname __attribute__((unused)))
{
}

void (*_select_source_line) (char *, int, char *, char *)
  = __select_source_line;

static void
__DebuggerScreen(void)
{
}

void (*_DebuggerScreen) (void) = __DebuggerScreen;

static int
__DoMake(void)
{
  return 1;
}

int (*_DoMake) (void) = __DoMake;

static char *
__GetProgName(void)
{
  return 0;
}

char *(*_GetProgName) (void) = __GetProgName;

static char **
__GetProgramArguments(int *count)
{
  *count = 0;
  return 0;
}

char **(*_GetProgramArguments) (int *) = __GetProgramArguments;

static char **
__GetSourceDirectories(int *count)
{
  *count = 0;
  return 0;
}

char **(*_GetSourceDirectories) (int *) = __GetSourceDirectories;

static void
__InitGDBOutWin(void)
{
}

void (*_InitGDBOutWin) (void) = __InitGDBOutWin;

static void
__UpdateGDBOutWin(char *buffer __attribute__ ((unused)))
{
}

void (*_UpdateGDBOutWin) (char *) = __UpdateGDBOutWin;

static void
__UserScreen(void)
{
}

void (*_UserScreen) (void) = __UserScreen;

static void
__UserWarning(int number __attribute__ ((unused)),...)
{
}

void (*_UserWarning) (int, ...) = __UserWarning;

static void
__StartSession(void)
{
}

void (*_StartSession) (void) = __StartSession;

static void
__EndSession(int exit_code __attribute__ ((unused)))
{
}

void (*_EndSession) (int) = __EndSession;

static void
__BreakSession(void)
{
}

void (*_BreakSession) (void) = __BreakSession;

static char *
__GetMainFunction()
{
  return "main";
}

char *(*_GetMainFunction) (void) = __GetMainFunction;
