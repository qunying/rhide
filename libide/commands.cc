/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#define Uses_ideCommands
#include <libide.h>

#define Uses_tvutilFunctions
#include <libtvuti.h>

#undef __CM
#undef __HC
#define __CM(x,y) add_command_constant("cm"#x,cm##x);
#define __HC(x,y) add_help_constant("hc"#x,hc##x);

static void __attribute__ (( __constructor__ ))
init_ide_commands()
{
  __IDE_COMMAND_CODES__
  __IDE_HELP_CODES__
}

