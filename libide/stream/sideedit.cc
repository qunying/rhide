/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TIDEEditWindow
#define Uses_TStreamableClass
/*
  ced_clas.h uses FILE * but does not
  include stdio.h :-(
*/
//#include <stdio.h>
#include <libide.h>

__link(RCEditWindow) __link(RIDEFileEditor) s(IDEEditWindow)
