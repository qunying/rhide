/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TStringCollection
#include <tv.h>

#define Uses_TCEditor
#define Uses_TNoCaseStringCollection
#define Uses_TCEditor_Internal
#include <ceditor.h>

#define Uses_TDirList
#include <libtvuti.h>

#include <rhutils.h>

#include <rhgdb.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>

static void
init_c_reserved_words()
{
#define F(x) ReservedWords->insert(strdup(x));
#include "../include/flags/reserved.h"
#undef F
}

static void
init_pascal_reserved_words()
{
  char *tmp;
#define F(x) tmp = strdup(x); string_down(tmp); PascalRWords->insert(tmp);
#include "../include/flags/gpcreser.h"
#undef F
}

static void __attribute__ ((__constructor__))
init_syntax()
{
  char *syntax_file;
  TCEditor::SHLGenList=new TNoCaseStringCollection(5,5);
  syntax_file = ExpandFileNameToThePointWhereTheProgramWasLoaded(SHLFile);
  if (LoadSyntaxHighLightFile(syntax_file,TCEditor::SHLArray,
                              TCEditor::SHLGenList,
                              TCEditor::SHLCant) != 0)
  {
    syntax_file = "__syntax__";
    FILE *f = fopen(syntax_file,"w+t");
    fprintf(f,"\n\
Name=C/C++\n\
Files=C,c,cpp,cxx,cc,h,hpp,i,ii\n\
UseInternal=1\n\
End\n\
Name=Pascal\n\
Files=pas,inc,p\n\
UseInternal=2\n\
End\n\
");
    fclose(f);
    LoadSyntaxHighLightFile(syntax_file,TCEditor::SHLArray,
                            TCEditor::SHLGenList,
                            TCEditor::SHLCant);
    unlink(syntax_file);
  }
 
  ReservedWords = new TStringCollection(15,16);
  PascalRWords = new TStringCollection(15,16);

  init_c_reserved_words();
  init_pascal_reserved_words();

  CreateSHShortCutTables();
}


