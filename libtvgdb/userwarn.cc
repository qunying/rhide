/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_MsgBox
#define Uses_tvutilFunctions
#include <libtvuti.h>

#define Uses_tvgdbFunctions
#include <libtvgdb.h>

#include <librhgdb.h>

#include <stdio.h>
#include <string.h>

static char buffer[1000];

static char *
IdentifyBreakPoint(struct BreakPoint *bp, char *msg)
{
  if (BREAK_FUNCTION(bp))
  {
    sprintf(buffer, _("%s\n\n(Breakpoint at function '%s' in file %s)"),
            msg, bp->function, bp->filename);
  }
  else
  {
    sprintf(buffer, _("%s\n\n(Breakpoint at line %d in file %s)"),
            msg, bp->line_number, bp->filename);
  }
  return buffer;
}

static char *
merge_message(char *msg1, char *msg2)
{
  strcpy(buffer, msg1);
  strcat(buffer, msg2);
  return buffer;
}

void
UserWarning(int number, ...)
{
  char *msg = NULL;
  va_list arg;
  struct BreakPoint *bp;

  va_start(arg, number);

  switch (number)
  {
    case WARN_SIGNALED:
    {
      char *sig;
      sig = va_arg(arg, char *);

      msg = merge_message(_("Program has received signal: "), sig);
      break;
    }
    case WARN_NOCODE:          // probably optimized out
      msg = _("For this line is no code generated");
      break;
    case WARN_NOPROGRAM:       /*
                                   This can't be happen in RHIDE 
                                 */
    case WARN_NOPROGRAM_FOUND:
      msg = _("Could not find the program. You must build at first "
              "the program to use this function, because the symbols "
              "are taken from that file.");
      break;
    case WARN_INVALID_BREAK:
      msg = _("The selected breakpoint is invalid, probably this file "
              "is not compiled with debugging information "
              "or for this line is no code generated.");
      bp = va_arg(arg, struct BreakPoint *);

      msg = IdentifyBreakPoint(bp, msg);
      break;
    case WARN_NOSYMBOLS:
      msg = _("This program has no usable debugging information. Try "
              "to compile it with the -g switch.");
      break;
    case WARN_NOT_ENABLED:     // produced only under Windows 3.1x
      msg = _("This breakpoint could not enabled, because you are "
              "running under Windows 3.1x and there the number of "
              "breakpoints is limited to 3");
      bp = va_arg(arg, struct BreakPoint *);

      msg = IdentifyBreakPoint(bp, msg);
      break;
    case WARN_NO_BREAK_AVAILABLE:	// produced only under Windows 3.1x
      msg = _("You have already 3 breakpoints set and under Windows 3.1x "
              "the number of breakpoints is limited to 3");
      bp = va_arg(arg, struct BreakPoint *);

      msg = IdentifyBreakPoint(bp, msg);
      break;
  }
  if (msg)
  {
    if (strlen(msg) > 50)
      BigmessageBox(msg, mfError | mfOKButton);
    else
      messageBox(msg, mfError | mfOKButton);
  }
  va_end(arg);
}
