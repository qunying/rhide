/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_tvutilFunctions
#define Uses_tvutilCommands
#include <libtvuti.h>

#undef __CM
#undef __HC
#define __CM(x,y) add_command_constant("cm"#x,cm##x);
#define __HC(x,y) add_help_constant("hc"#x,hc##x);

static void __attribute__ ((__constructor__)) init_tvutil_commands()
{
__TVUTIL_COMMAND_CODES__ __TVUTIL_HELP_CODES__}
