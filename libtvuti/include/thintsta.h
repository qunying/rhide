/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_THintStatusLine ) && !defined( __THintStatusLine__ )
#define __THintStatusLine__

class TRect;
class TStatusDef;

class THintStatusLine:public TStatusLine
{
public:
  const char *(*HintFunction) (ushort);
    THintStatusLine(const TRect & r, TStatusDef & def,
                    const char *(*hintfunction) (ushort) =
                    NULL):TStatusLine(r, def), HintFunction(hintfunction)
  {
  }
  virtual const char *hint(ushort);
};

inline const char *
THintStatusLine::hint(ushort ctx)
{
  if (!HintFunction)
    return "";
  return HintFunction(ctx);
}

#endif
