/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <rhutils.h>

#define Uses_MsgRec
#include <libtvuti.h>

MsgRec::MsgRec(const char *fname, const char *m, msgType atype,
               int line, int acolumn):
type(atype), lineno(line), column(acolumn)
{
  if (!fname || !*fname)
    filename = NULL;
  else
    string_dup(filename, fname);
  if (!m || !*m)
    msg = NULL;
  else
    string_dup(msg, m);
}

MsgRec::~MsgRec()
{
  string_free(filename);
  string_free(msg);
}
