/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#define Uses_TDrawBuffer
#define Uses_TStaticText

#define Uses_TCheckDialog
#include <libtvuti.h>

class MyStaticText:public TStaticText
{
public:
  MyStaticText(const TRect & r, const char *t):TStaticText(r, t)
  {
  }
  void update(const char *);
  virtual void draw();
};

void
MyStaticText::draw()
{
  uchar color;
  Boolean center;
  int i, j, l, p, y;
  TDrawBuffer b;
  const char *s = text;

  color = getColor(1);
  l = strlen(s);
  p = 0;
  y = 0;
  center = False;
  while (y < size.y)
  {
    b.moveChar(0, ' ', color, size.x);
    if (p < l)
    {
      if (s[p] == 3)
      {
        center = True;
        ++p;
      }
      i = p;
      do
      {
        j = p;
        while ((p < l) && (s[p] == ' '))
          ++p;
        while ((p < l) && (s[p] != ' ') && (s[p] != '\n'))
          ++p;
      }
      while ((p < l) && (p < i + size.x) && (s[p] != '\n'));
      if (p > i + size.x)
        if (j > i)
          p = j;
        else
          p = i + size.x;
      if (center == True)
        j = (size.x - p + i) / 2;
      else
        j = 0;
      b.moveBuf(j, &s[i], color, (p - i));
      while ((p < l) && (s[p] == ' '))
        p++;
      if ((p < l) && (s[p] == '\n'))
      {
        center = False;
        p++;
        if ((p < l) && (s[p] == 10))
          p++;
      }
    }
    writeLine(0, y++, size.x, 1, b);
  }
}

void
MyStaticText::update(const char *fname)
{
  delete text;

  text = newStr(fname);
}

TCheckDialog::TCheckDialog(const TRect & r, const char *what):
TDialog(r, NULL), TWindowInit(&TCheckDialog::initFrame)
{
  TRect rr(1, 2, size.x - 1, size.y - 1);

  flags &= ~wfClose;
  text = new MyStaticText(rr, "");
  insert(text);
  rr.a.y--;
  rr.b.y = rr.a.y + 1;
  rr.b.x = rr.a.x + 25;
  insert(new TStaticText(rr, what));
}

void
TCheckDialog::update(const char *fname)
{
  text->update(fname);
  text->drawView();
  drawView();
}
