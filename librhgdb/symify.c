/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdb.h>
#include <librhgdb.h>

void symify(const char *s,char *function,char *file,int * line,int _diff)
{
  struct minimal_symbol *msymbol = NULL;
  struct symtab_and_line sal;
  struct symtabs_and_lines sals;
  struct symtab *symtab;
  struct symbol *symbol;
  unsigned long core;
  long diff;
  char *tmp = alloca(1000);
  sscanf(s,"%lx",&core);
  core += _diff;
  sprintf(tmp,"*%ld",core);
  sals = decode_line_1(&tmp,0,0,0,0);
  if (!sals.sals[0].symtab)
  {
      msymbol = lookup_minimal_symbol_by_pc(core);
      if (!msymbol)
      {
        *function = 0;
        *file = 0;
        *line = 0;
        return;
      }
      diff = core - SYMBOL_VALUE_ADDRESS(msymbol);
      sprintf(function,"%s%+ld",SYMBOL_NAME(msymbol),diff);
      *file = 0;
      *line = 0;
      return;
  }
  sal = find_pc_line(core,1);
  symbol = find_pc_function(core);
  symtab = sal.symtab;
  strcpy(function,SYMBOL_SOURCE_NAME(symbol));
  strcpy(file,symtab->filename);
  *line = sal.line;
}

#ifdef TEST
char *progname;

static char *GetProgName()
{
  return progname;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pc.h>

#define SC(r,c) (*(char *)(sc + (r)*ScreenCols() + (c)))
#define SW(r,c) (*(sc + (r)*ScreenCols() + (c)))

int main(int argc,char *argv[])
{
  char function[1024],file[256];
  int lineno;
  int r, c;
  short *sc;
  char buf[1024];
  int i;
  int diff=0;
  unsigned v;
  FILE *ofile=0;
  FILE *ifile=0;

  if (argc < 2)
  {
    fprintf(stderr, "Usage: gsymify [-o <outfile>] [-i <tracefile>] [-a <adjust>] <program>\n");
    fprintf(stderr, "This program adds debug information to DJGPP program call frame tracebacks\n");
    return 1;
  }
  while (argv[1][0] == '-')
  {
    if ((strcmp(argv[1], "-a") == 0) && (argc > 3))
    {
      diff = atoi(argv[2]);
      argc -= 2;
      argv += 2;
    }
    if ((strcmp(argv[1], "-o") == 0) && (argc > 3))
    {
      ofile = fopen(argv[2], "w");
      if (ofile == 0)
        fprintf(stderr, "Error: unable to open file %s\n", argv[2]);
      argc -= 2;
      argv += 2;
    }
    if ((strcmp(argv[1], "-i") == 0) && (argc > 3))
    {
      ifile = fopen(argv[2], "r");
      if (ifile == 0)
        fprintf(stderr, "Error: unable to open file %s\n", argv[2]);
      argc -= 2;
      argv += 2;
    }
  }
  progname = argv[1];
  _GetProgName = GetProgName;
  InitRHGDB();

  if (ifile)
  {
    char line[1000];
    if (ofile == 0)
      ofile = stdout;
    while (fgets(line, 1000, ifile))
    {
      if (strncmp(line, "  0x", 4) == 0)
      {
        sscanf(line+4, "%x", &v);
        symify(line+4,function,file,&lineno,diff);
        fprintf(ofile, "  0x%08x", v);
        if (*function)
        {
          fprintf(ofile," %s",function);
          if (*file)
          {
            fprintf(ofile,", line %d of %s",lineno,file);
          }
        }
        fprintf(ofile,"\n");
      }
      else
        fputs(line, ofile);
    }
    return 0;
  }

  sc = (short *)malloc(ScreenRows() * ScreenCols() * 2);

  ScreenRetrieve(sc);

  for (r=0; r<ScreenRows(); r++)
  {
    if (SC(r,0) == ' ' && SC(r,1) == ' ' && SC(r,2) == '0' && SC(r,3) == 'x')
    {
      buf[8] = 0;
      for (i=0; i<8; i++)
        buf[i] = SC(r, i+4);
      sscanf(buf, "%x", &v);
      symify(buf,function,file,&lineno,diff);

      buf[0] = 0;
      if (*function)
      {
	strcpy(buf, function);
      }
      if (*file)
      {
        if (buf[0])
          strcat(buf, ", ");
        sprintf(buf+strlen(buf), "line %d of %s", lineno, file);
      }
      if (buf[0])
        for (i=0; buf[i]; i++)
          SW(r, 15+i) = 0x0f00 + buf[i];
    }
  }

  if (ofile)
  {
    for (r=0; r<ScreenRows(); r++)
    {
      c = 0;
      for (i=0; i<ScreenCols(); i++)
        if (SC(r, i) != ' ')
          c = i;
      for (i=0; i<=c; i++)
        fputc(SC(r,i), ofile);
      fputc('\n', ofile);
    }
    fclose(ofile);
  }
  else
    ScreenUpdate(sc);
  return 0;
}

#endif

