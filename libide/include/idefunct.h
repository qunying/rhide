/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( ideFunctions__ )
#define ideFunctions__

class TIDEFileEditor;

void goto_line(TIDEFileEditor * editor, int line, int column = 1);

long time_of_file(const char *fname);
COMPILE_ID how_to_compile(const char *ext_from, const char *ext_to);
COMPILE_ID how_to_compile(FILE_TYPE from, FILE_TYPE to);
FILE_TYPE get_file_type(const char *ext);

#endif
