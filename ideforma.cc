/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TIDEEditWindow
#define Uses_TIDEFileEditor
#include <libide.h>
#include "rhide.h"

#ifdef INTERNAL_DEBUGGER
#include <librhgdb.h>
#endif

#define cNormal   1
#define cMarked   2
#define cComment  3
#define cReserved 4
#define cIdent    5
#define cSymbol   6
#define cString   7
#define cInteger  8
#define cFloat    9
#define cOctal    10
#define cHex      11
#define cChar     12
#define cPre      13
#define cIllegal  14
#define cUser     15
#define cCPU      16
#define cBreak    17
#define cSymbol2  18

#ifdef INTERNAL_DEBUGGER
extern uint32 CPULine;
extern TCEditWindow *current_editor;

int
DebuggerFormatLine(TCEditor * editor,
                   void *DrawBuf,
                   unsigned LinePtr,
                   int Width,
                   unsigned short Colors,
                   unsigned lineLen, uint32 Attr, unsigned LineNo
#if (TCEDITOR_VERSION >= 0x000452)
                   , uint32 *colmap
#endif
		   )
{
#define drawbuf ((ushort *)DrawBuf)
  uint32 offset = 0;
  ushort color;

  if (DEBUGGER_STARTED() && current_editor &&
      editor == current_editor->editor && CPULine == LineNo)
  {
    editor->formatLine(DrawBuf, LinePtr, Width, Colors, lineLen, Attr,
                       LineNo
#if (TCEDITOR_VERSION >= 0x000452)
                       ,colmap
#endif
                       );
    color = editor->getColor(cCPU) << 8;
    while (Width--)
    {
      drawbuf[offset] &= 0x00FF;
      drawbuf[offset++] |= color;
    }
    return 1;
  }
  if (IsBreakPointLine(((TIDEFileEditor *) editor)->bname, LineNo + 1) >= 0)
  {
    editor->formatLine(DrawBuf, LinePtr, Width, Colors, lineLen, Attr,
                       LineNo
#if (TCEDITOR_VERSION >= 0x000452)
                       ,colmap
#endif
                       );
    color = editor->getColor(cBreak) << 8;
    while (Width--)
    {
      drawbuf[offset] &= 0x00FF;
      drawbuf[offset++] |= color;
    }
    return 1;
  }
  return 0;
}
#endif
