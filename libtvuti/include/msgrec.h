/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_MsgRec ) && !defined( __MsgRec__ )
#define __MsgRec__

typedef enum
{
  msgError,
  msgWarning,
  msgMessage
}
msgType;

struct MsgRec
{
  char *filename;
  msgType type;
  int lineno;
  int column;
  char *msg;
    MsgRec(const char *file_name, const char *msg,
           msgType atype = msgMessage, int alineno = 0, int acolumn = 1);
   ~MsgRec();
};

#endif
