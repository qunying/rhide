/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#define Uses_TListViewer
#define Uses_TScrollBar
#define Uses_TStaticText
#define Uses_TEvent
#define Uses_TPalette
#define Uses_TKeys
#define Uses_TRangeValidator

#define Uses_TDisassemblerWindow
#define Uses_tvgdbCommands
#include <libtvgdb.h>

#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <librhgdb.h>

#include <rhutils.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct Register
{
  char *name;
  int size;
  int has_changed;
  unsigned long value;
};

/*
 01 Unchanged register
 02 Changed register
 03 Focused register
*/

#define cpRegisters "\x0B\x0C\x0D"

class TRegisters : public TView
{
public:
  TRegisters(const TRect &bounds);
  ~TRegisters();
  void getText(char *dest, int reg_num);
  void update();
  void editRegister(int num);
  virtual void draw();
  virtual TPalette& getPalette() const;
  virtual void setState(ushort aState, Boolean enable);
  virtual void handleEvent(TEvent &event);

  int selected;
  Register *Registers;
};

void TRegisters::editRegister(int num)
{
  unsigned long val;
  char buf[256];
  TRangeValidator *valid;
  sprintf(buf,"0x%lx",Registers[num].value);
  unsigned long max_val;
  switch (Registers[num].size)
  {
    case 1:
      max_val = 0xff;
      break;
    case 2:
      max_val = 0xffff;
      break;
    case 4:
      max_val = 0xffffffffL;
      break;
    default:
      max_val = 0xffffffffL;
      break;
  }
  valid = new TRangeValidator(0,max_val);
  if (ValidInputBox( _("Change register value"), _("~V~alue"),
                     buf, 255, valid))
  {
    if (buf[0] == '0')
    {
      if (buf[1] == 'x' || buf[1] == 'X')
        sscanf(buf+2, "%lux", &val);
      else
        sscanf(buf, "%luo", &val);
    }
    else
      sscanf(buf, "%lud", &val);
    if (Registers[num].value != val)
    {
      Registers[num].value = val;
      Registers[num].has_changed = 1;
      set_register_value(num, val);
      drawView();
    }
  }
}

void TRegisters::handleEvent(TEvent & event)
{
  TView::handleEvent(event);
  switch (event.what)
  {
    default:
      break;
    case evCommand:
      switch (event.message.command)
      {
        default:
          break;
        case cmModifyRegister:
          if (debugger_started)
            editRegister(selected);
          clearEvent(event);
          break;
      }
      break;
    case evKeyDown:
      switch (event.keyDown.keyCode)
      {
        case kbUp:
          if (selected > 0)
            selected--;
          clearEvent(event);
          drawView();
          break;
        case kbDown:
          if (selected < size.y-1)
            selected++;
          clearEvent(event);
          drawView();
          break;
        default:
          break;
      }
      break;
  }
}

void TRegisters::setState( ushort aState, Boolean enable )
{
  TView::setState(aState, enable);
  switch (aState)
  {
    case sfFocused:
      drawView();
      break;
    default:
      break;
  }
}

TPalette &TRegisters::getPalette() const
{
  static TPalette pal( cpRegisters, sizeof( cpRegisters )-1 );
  return pal;
}

TRegisters::TRegisters(const TRect & bounds)
 : TView(bounds)
{
  int i;
  setState(sfShadow, False);
  options |= ofSelectable;
  helpCtx = hcRegister;
  growMode = gfGrowLoX | gfGrowHiX;
  Registers = (Register *)malloc(size.y*sizeof(Register));
  for (i=0;i<size.y;i++)
  {
    Registers[i].name = string_dup(register_name(i));
    Registers[i].size = get_register_size(i);
    Registers[i].value = 0;
    Registers[i].has_changed = 0;
  }
  selected = 0;
}

TRegisters::~TRegisters()
{
  int i;
  for (i=0;i<size.y;i++)
  {
    string_free(Registers[i].name);
  }
  free(Registers);
}

void TRegisters::getText(char *retval, int reg_num)
{
  char fmt[32];
  int space_width;
  int value_width;
  int reg_name_len = strlen(Registers[reg_num].name);
  value_width = Registers[reg_num].size*2;
  space_width = size.x-reg_name_len-value_width;
  sprintf(fmt,"%%s%%%dc%%0%dlx",space_width,value_width);
  sprintf(retval, fmt, Registers[reg_num].name, ' ', Registers[reg_num].value);
}

void TRegisters::draw()
{
  TDrawBuffer b;
  uchar normal_color = getColor(1);
  uchar changed_color = getColor(2);
  uchar focused_color = getColor(3);
  int i;
  char buf[size.x+1];
  for (i=0;i<size.y;i++)
  {
    uchar color;
    getText(buf,i);
    if (i == selected && (state & sfFocused))
      color = focused_color;
    else
    {
      if (Registers[i].has_changed)
        color = changed_color;
      else
        color = normal_color;
    }
    b.moveChar(0, ' ', color, size.x);
    b.moveCStr(0, buf, color);
    writeLine(0, i, size.x, 1, b);
  }
}

void TRegisters::update()
{
  unsigned long val;
  int i;
  for (i=0;i<size.y;i++)
  {
    val = get_register_value(i);
    if (val != Registers[i].value)
    {
      Registers[i].value = val;
      Registers[i].has_changed = 1;
    }
    else
      Registers[i].has_changed = 0;
  }
  drawView();
}

struct TDisassemblerLine
{
  unsigned long address;
  char *text;
  int only_text;
};

/*
 01 Normal text (active)
 02 Normal text (not active)
 03 Focused text
 04 Selected text
 05 Execution line
 06 Breakpoint
*/

#define cpDisassemblerViewer "\x06\x06\x07\x08\x09\x0A"

class TDisassemblerViewer : public TListViewer
{
public:
  TDisassemblerViewer(const TRect& bounds, TScrollBar *aHScrollBar,
                      TScrollBar *aVScrollBar, check_func _is_bp);
  ~TDisassemblerViewer();
  virtual TPalette& getPalette() const;
  virtual void getText(char *dest, ccIndex item, short maxLen);
  virtual void changeBounds(const TRect &bounds);
  virtual void focusItemNum(ccIndex item);
  virtual void setState( ushort aState, Boolean enable);
  virtual void draw();
  void update(unsigned long address);
  void select_address(unsigned long address);

  TDisassemblerLine *dis_lines;
  unsigned long start_address;
  unsigned long end_address;
  unsigned long next_address;
  int max_len;

  check_func is_bp;
};

extern unsigned long stop_pc;

void TDisassemblerViewer::draw()
{
 int i, j;
 ccIndex item;
 ushort normalColor, selectedColor, focusedColor=0, color;
 int colWidth, curCol, indent;
 TDrawBuffer b;
 uchar scOff;

   if( (state&(sfSelected | sfActive)) == (sfSelected | sfActive))
        {
        normalColor = getColor(1);
        focusedColor = getColor(3);
        selectedColor = getColor(4);
        }
    else
        {
        normalColor = getColor(2);
        selectedColor = getColor(4);
        }

    if( hScrollBar != 0 )
        indent = hScrollBar->value;
    else
        indent = 0;

    colWidth = size.x / numCols + 1;
    for( i = 0; i < size.y; i++ )
        {
        for( j = 0; j < numCols; j++ )
            {
            int width;;
            item =  j * size.y + i + topItem;
            curCol = j * colWidth;
            if (j == numCols-1) width = size.x - curCol + 1; 
            else width = colWidth;
            if (debugger_started && dis_lines[item].address == stop_pc)
            {
              color = getColor(5);
              scOff = 2;
            }
            else if( (state & (sfSelected | sfActive)) == (sfSelected | sfActive) &&
                focused == item &&
                range > 0)
                {
                color = focusedColor;
                setCursor( curCol + 1, i );
                scOff = 0;
                }
            else if (is_bp && is_bp(dis_lines[item].address))
            {
              color = getColor(6);
              scOff = 2;
            }
            else if( item < range && isSelected(item) )
                {
                color = selectedColor;
                scOff = 2;
                }
            else
                {
                color = normalColor;
                scOff = 4;
                }

            b.moveChar( curCol, ' ', color, width );
            if( item < range )
                {
                char text[width + indent + 1]; // This was probably the
                                               // reason for a bug, because
                                               // getText assumes a buffer
                                               // with a length of maxLen + 1
                getText( text, item, width + indent );
                char buf[width+1];
                memcpy( buf, text+indent, width );
                buf[width] = EOS;
                b.moveStr( curCol+1, buf, color );
                if( showMarkers )
                    {
                    b.putChar( curCol, specialChars[scOff] );
                    b.putChar( curCol+width-2, specialChars[scOff+1] );
                    }
                }
            else if( i == 0 && j == 0 )
                b.moveStr( curCol+1, _("<empty>"), getColor(1) );

            b.moveChar( curCol+width-1, 179, getColor(5), 1 );
            }
        writeLine( 0, i, size.x, 1, b );
        }
}

void TDisassemblerViewer::setState( ushort aState, Boolean enable)
{
  TView::setState( aState, enable );
  if (aState & sfFocused)
    drawView();
}

TPalette &TDisassemblerViewer::getPalette() const
{
  static TPalette pal( cpDisassemblerViewer, sizeof( cpDisassemblerViewer )-1 );
  return pal;
}

TDisassemblerViewer::TDisassemblerViewer(const TRect& bounds, TScrollBar *aHScrollBar,
                                         TScrollBar *aVScrollBar,
                                         check_func _is_bp) :
  TListViewer(bounds, 1, aHScrollBar, aVScrollBar),
  dis_lines(NULL), start_address(0), end_address(0), next_address(0),
  max_len(0), is_bp(_is_bp)
{
  setRange(0);
  helpCtx = hcDisassemblerViewer;
}

TDisassemblerViewer::~TDisassemblerViewer()
{
  int i;
  for (i=0;i<range;i++)
  {
    string_free(dis_lines[i].text);
  }
  if (dis_lines)
    free(dis_lines);
}

void TDisassemblerViewer::getText(char *dest, ccIndex item, short maxLen)
{
  if (item < range)
  {
    strncpy(dest, dis_lines[item].text, maxLen);
    dest[maxLen] = 0;
  }
  else
    *dest = 0;
}

#define Max_Len ((max_len-size.x+1) >= 0 ? (max_len-size.x+1) : 0)

void TDisassemblerViewer::changeBounds(const TRect &bounds)
{
  TListViewer::changeBounds(bounds);
  if (range < size.y)
  {
    ccIndex old_focused = focused;
    end_address = 0;
    update(start_address);
    focusItemNum(old_focused);
  }
  if (hScrollBar)
    hScrollBar->setRange(0, Max_Len);
  drawView();
}

void TDisassemblerViewer::focusItemNum(ccIndex item)
{
  if (!start_address)
    return;
  int force_redraw = 0;
  if (item < 0)
  {
    const char * const * lines;
    int i,lines_count;
    unsigned long adr = start_address + item + 1, nadr;
    do
    {
      adr--;
      lines_count = -item;
      lines = disassemble(adr, &lines_count, 1, 1, &nadr);
    } while (nadr > start_address);
    start_address = adr;
    dis_lines = (TDisassemblerLine *)realloc(dis_lines,
        (lines_count+range)*sizeof(TDisassemblerLine));
    memmove(dis_lines+lines_count, dis_lines,
            range*sizeof(TDisassemblerLine));
    for (i=0;i<lines_count;i++)
    {
      if (lines[i][0] == ' ')
      {
        dis_lines[i].text = string_dup(lines[i]+1);
        dis_lines[i].only_text = 1;
      }
      else
      {
        dis_lines[i].only_text = 0;
        sscanf(lines[i], "%lx", &dis_lines[i].address);
        dis_lines[i].text = string_dup(lines[i]);
      }
      int len = strlen(lines[i]);
      if (len > max_len)
        max_len = len;
    }
    setRange(range+lines_count);
    if (hScrollBar)
      hScrollBar->setRange(0, Max_Len);
    item = 0;
    force_redraw = 1;
  }
  else if (item >= range)
  {
    const char * const * lines;
    int i,lines_count=item-range+1;
    lines = disassemble(next_address, &lines_count, 1, 1, &next_address);
    dis_lines = (TDisassemblerLine *)realloc(dis_lines,
        (lines_count+range)*sizeof(TDisassemblerLine));
    for (i=0;i<lines_count;i++)
    {
      if (lines[i][0] == ' ')
      {
        dis_lines[range+i].text = string_dup(lines[i]+1);
        dis_lines[range+i].only_text = 1;
      }
      else
      {
        dis_lines[range+i].only_text = 0;
        sscanf(lines[i], "%lx", &dis_lines[range+i].address);
        end_address = dis_lines[range+i].address;
        dis_lines[range+i].text = string_dup(lines[i]);
      }
      int len = strlen(lines[i]);
      if (len > max_len)
        max_len = len;
    }
    setRange(range+lines_count);
    if (hScrollBar)
      hScrollBar->setRange(0, Max_Len);
  }
  TListViewer::focusItemNum(item);
  if (force_redraw)
    drawView();
}

void TDisassemblerViewer::update(unsigned long address)
{
  if (address < start_address || address > end_address)
  {
    const char * const * lines;
    int i,lines_count=size.y;
    lines = disassemble(address, &lines_count, 1, 1, &next_address);
    for (i=0; i<range; i++)
    {
      string_free(dis_lines[i].text);
    }
    max_len = 0;
    dis_lines = (TDisassemblerLine *)realloc(dis_lines,
        lines_count*sizeof(TDisassemblerLine));
    start_address = 0;
    for (i=0; i<lines_count; i++)
    {
      if (lines[i][0] == ' ')
      {
        dis_lines[i].text = string_dup(lines[i]+1);
        dis_lines[i].only_text = 1;
      }
      else
      {
        dis_lines[i].only_text = 0;
        sscanf(lines[i], "%lx", &dis_lines[i].address);
        end_address = dis_lines[i].address;
        if (!start_address)
          start_address = end_address;
        dis_lines[i].text = string_dup(lines[i]);
      }
      int len = strlen(lines[i]);
      if (len > max_len)
        max_len = len;
    }
    setRange(lines_count);
    if (hScrollBar)
      hScrollBar->setRange(0, Max_Len);
  }
  select_address(address);
}

void TDisassemblerViewer::select_address(unsigned long address)
{
  ccIndex i;
  for (i=0; i<range;i++)
  {
    if (dis_lines[i].address == address)
    {
      focusItemNum(i);
      break;
    }
  }
}

/*
 01 Frame disabled
 02 Frame
 03 Frame icons
 04 Scroll bar page
 05 Scroll bar icons
 06-0A TDisassemblerViewer
 0B-0D TRegisters
*/

#define cpDisassemblerWindow "\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7"\
                             "\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF"

TPalette &TDisassemblerWindow::getPalette() const
{
    static TPalette pal (cpDisassemblerWindow,
                         sizeof (cpDisassemblerWindow) - 1 );
    return pal;
}

TDisassemblerWindow::TDisassemblerWindow(const TRect &bounds,
                 const char *aTitle, check_func is_bp) :
  TDialog(bounds, aTitle),
  TWindowInit( TDisassemblerWindow::initFrame)
{
  int RegisterCount = register_count();
  int max_reg_len = 0;
  TScrollBar *vs,*hs;
  int i,l;
  TRect r = getExtent();
  for (i=0;i<RegisterCount;i++)
  {
    l = strlen(register_name(i));
    if (l>max_reg_len)
      max_reg_len = l;
  }
  r.grow(-1,-1);
  r.b.x -= max_reg_len+1+8;
  vs = new TScrollBar(TRect(r.b.x-1,r.a.y,r.b.x,r.b.y));
  insert(vs);

  hs = new TScrollBar(TRect(r.a.x,r.b.y-1,r.b.x,r.b.y));
  insert(hs);

  r.b.x--;
  r.b.y--;
  viewer = new TDisassemblerViewer(r, hs, vs, is_bp);
  insert(viewer);
  viewer->growMode = gfGrowHiX | gfGrowHiY;
  growMode = gfGrowLoY | gfGrowHiX | gfGrowHiY;
  flags |= wfGrow|wfZoom;

  r.a.x = r.b.x+1;
  r.b.x = size.x-1;
  r.b.y = r.a.y+RegisterCount;
  registers = new TRegisters(r);
  insert(registers);

  viewer->select();
  if (debugger_started)
    update(stop_pc);
}

void TDisassemblerWindow::update(unsigned long address)
{
  viewer->update(address);
  registers->update();
}

unsigned long TDisassemblerWindow::focused_address()
{
  return viewer->dis_lines[viewer->focused].only_text ?
    0 : viewer->dis_lines[viewer->focused].address;
}

void TDisassemblerWindow::sizeLimits( TPoint& min, TPoint& max )
{
  TWindow::sizeLimits(min, max);
  int minx = registers->size.x + 10 + 2;
  int miny = registers->size.y + 2;
  min.x = minx > min.x ? minx : min.x;
  min.y = miny > min.y ? miny : min.y;
}

