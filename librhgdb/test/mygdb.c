/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/* This is a very simple program to show how to use the
   librhgdb.a library.
   It can be used as a debugger, which debugs the program argv[1]
   with the arguments from argv[2] on.

   It supports only the commands from
   gdb:
     run
     continue
     next
     step
     kill
     quit

   The output from librhgdb is written to stderr so a common usage would
   be

 redir -e err mygdb program arguments

This program was written only to show a sample usage so do not
expect too much from it.

To link the program you the librhgdb library (normal the parent directory
of this program here) and the libgdb library relativ from here normaly
in ../../libgdb.

With RHIDE you can use the mygdb.gpr project file otherwise use the
makefile mygdb.mak. You have to set the environment variable RHIDESRC
to the root directory of the RHIDE sources as an ABSOLUTE path. Normaly
this is the directory two levels above the current.

WARNING: This example 'mygdb' is hardly UNTESTED!!!!

*/

#include <librhgdb.h>
#include <rhgdbint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *current_filename=NULL;
static char **file_lines = NULL;
static int line_count = 0;
static char _line[256];

static void Disass()
{
  int i,count;
  extern unsigned long stop_pc;
  unsigned long pc = stop_pc;
  char *arg = _line+7;
  const char * const *lines;
  int line_count;
  sscanf(arg,"%d",&count);
  while (count > 0)
  {
    lines = disassemble(pc, &line_count, 1, 1, &pc);
    for (i=0;i<line_count;i++)
    {
      fprintf(stdout,"%s\n",lines[i]);
      count--;
    }
  }
}

static void read_file(char *fname)
{
  FILE *f;
  static char buffer[1000];
  if (current_filename) free(current_filename);
  if (file_lines)
  {
    int i;
    for (i=0;i<line_count;i++) free(file_lines[i]);
    free(file_lines);
    file_lines = NULL;
    line_count = 0;
  }
  current_filename = NULL;
  f = fopen(fname,"r");
  if (!f) return;
  while (fgets(buffer,999,f))
  {
    buffer[strlen(buffer)-1] = 0;
    line_count++;
    file_lines = (char **)realloc(file_lines,sizeof(char *)*line_count);
    file_lines[line_count-1] = strdup(buffer);
  }
  fclose(f);
  current_filename = strdup(fname);
}

static void show_line(char *fname,int line_no
                      , char *dirname __attribute__((unused))
                      , char *fullname __attribute__((unused)))
{
  if (!current_filename || strcmp(current_filename,fname))
    read_file(fname);
  if (!current_filename) return;
  fprintf(stdout,"%s:%d:\n",fname,line_no);
  printf("%s\n",file_lines[line_no-1]);
}

static char *progname=NULL;
static char **global_argv=NULL;
static int global_argc=0;

static char *GetProgName()
{
  return progname;
}

static char **GetProgramArguments(int *count)
{
  *count = global_argc-1;
  return global_argv+1;
}

static void UpdateGDBOutWin(char *buf)
{
  fprintf(stderr,"%s\n",buf);
}

static void InitMyGDB()
{
  _GetProgName = GetProgName;
  _GetProgramArguments = GetProgramArguments;
  _select_source_line = show_line;
  _UpdateGDBOutWin = UpdateGDBOutWin;
  InitRHGDB();
}

typedef void (*FUNC)();

static FUNC commands[] = {
  Step,
  Trace,
  Continue,
  ResetDebugger,
  Continue,
  Disass,
  0
};

static int read_command()
{
  fprintf(stdout,"> ");
  fflush(stdout);
  fgets(_line,255,stdin);
  _line[strlen(_line)-1] = 0;
#define C(x,y) if (strncmp(_line,#x,strlen(#x)) == 0) return y
  C(step,0);
  C(next,1);
  C(run,2);
  C(kill,3);
  C(continue,4);
  C(disass,5);
  C(quit,6);
#undef C
  return -1;
}

static void commandloop()
{
  int command;
  FUNC func;
  do
  {
    command = read_command();
    if (command == -1)
    {
      fprintf(stdout,"Unknown command: %s\n",_line);
      continue;
    }
    func = commands[command];
    if (!func) return;
    func();
  } while (1);
}

int main(int argc,char *argv[])
{
  progname = argv[1];
  argc--;
  argv++;
  global_argc = argc;
  global_argv = argv;
  verbose_gdb_commands = 1;
  InitMyGDB();
  fprintf(stdout,"This is MyGDB. A very simple sample program to show how to use librhgdb\n");
  fprintf(stdout,"Available commands:\n\trun\n\tnext\n\tstep\n\tcontinue\n\tkill\n\tquit\n");
  commandloop();
  return 0;
}


