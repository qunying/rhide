/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( __TLButton )
#define __TLButton

class TRect;

class TLButton:public TButton
{
public:
  TLButton(TRect & bounds, const char *aTitle, ushort aCommand,
           ushort aFlags);
};

#endif
