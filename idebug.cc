/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include "rhide.h"
#include <rhutils.h>

#include <stdlib.h>
#include <stdio.h>
#ifdef __DJGPP__
#include <dos.h>
#include <dpmi.h>
#endif

#ifdef __DJGPP__
unsigned short win_version = 0;
int nt_detected = 0;
int w95_detected = 0;

/* Return windows version number, or 0 if not returned */
static unsigned short
_get_win_version()
{
  __dpmi_regs regs;

  regs.x.ax = 0x160a;
  __dpmi_int(0x2f, &regs);
  if (regs.x.ax)
    return 0;
  return regs.x.bx;
}

CONSTRUCTOR_FUNCTION(init_win_check)
{
  win_version = _get_win_version();
  nt_detected = (win_version == 0) && (_get_dos_version(1) == 0x0532);
  w95_detected = win_version >= 0x0400;
}

#endif

static const char *
_getenv(char *v)
{
  const char *vv = getenv(v);

  return vv ? vv : "";
}

static void
sample_program(char *&buf, int c)
{
  char *cmd;

#ifdef __DJGPP__
  string_dup(cmd,
             "echo \"int main(){return 0;}\" | redir -eo gcc -Xlinker -v -Wa,-v -v -o /dev/null -x ");
#else
  string_dup(cmd,
             "echo \"int main(){return 0;}\" | gcc -Xlinker -v -Wa,-v -v -o /dev/null -x ");
#endif
  string_cat(cmd, c ? "c -" : "c++ -");
#ifndef __DJGPP__
  string_cat(cmd, " 2>&1");
#endif
  string_cat(buf, _("executing: "), "\n", cmd, "\n", NULL);
  FILE *f;
  char Buf[1000];

  f = popen(cmd, "r");
  if (!f)
    return;
  while (fgets(Buf, 999, f))
  {
    string_cat(buf, Buf);
  }
  pclose(f);
}

char *
create_bug_report(int call_gcc)
{
  char *buf = NULL;

  string_cat(buf, "This is a bug report for ");
  string_cat(buf, IDEVersion);
  string_cat(buf, " (");
  string_cat(buf, build_date);
  string_cat(buf, " ");
  string_cat(buf, build_time);
  string_cat(buf, ")\n");
#ifdef __DJGPP__
  {
    char temp[100];

    _get_dos_version(1);
    sprintf(temp, "%s %d.%d", _os_flavor, _osmajor, _osminor);
    string_cat(buf, "I am running ");
    string_cat(buf, temp);
    if (win_version > 0)
    {
      string_cat(buf, " (Windows ");
      if (w95_detected)
        string_cat(buf, "95");
      else
      {
        sprintf(temp, "%d.%d", win_version / 0x100, win_version & 0xff);
        string_cat(buf, temp);
      }
      string_cat(buf, ")");
    }
    else if (nt_detected)
    {
      string_cat(buf, " (Windows NT :-( )");
    }
  }
#endif
  string_cat(buf, "\n\nImportant environment variables:");
#define VAR(x) string_cat(buf,"\n"#x"="); string_cat(buf,_getenv(#x))
  VAR(PATH);
#ifdef __DJGPP__
  VAR(DJGPP);
  VAR(DJDIR);
  VAR(LFN);
#endif
  VAR(INFOPATH);
  VAR(LOCALEDIR);
  VAR(LANGUAGE);
  VAR(SHELL);
#ifdef __DJGPP__
  VAR(COMSPEC);
  VAR(DJSYSFLAGS);
#endif
#ifdef __linux__
  VAR(TERM);
#endif
  string_cat(buf, "\n");
  if (call_gcc)
  {
    string_cat(buf,
               "\nHere is the output from a sample compilation and link for C\n");
    sample_program(buf, 1);
    string_cat(buf, "\nEnd of the sample\n");
    string_cat(buf,
               "\nHere is the output from a sample compilation and link for C++\n");
    sample_program(buf, 0);
    string_cat(buf, "\nEnd of the sample\n");
  }
  string_cat(buf, "\n\n\n\
My problem is the following:\n\
============================\n\
\n\n");
  return buf;
}

#ifdef TEST

int
main()
{
  fprintf(stderr, "%s", create_bug_report(1));
  return 0;
}

#endif
