/*-------------------------------------------------------*/
/*                                                       */
/*   Turbo Vision 1.0                                    */
/*   Copyright (c) 1991 by Borland International         */
/*                                                       */
/*   Ascii.h: Header file for Ascii.cpp                  */
/*-------------------------------------------------------*/

#if !defined( __ASCII_H )
#define __ASCII_H

const int cmAsciiTableCmdBase = 590;
const int cmCharFocused = 0;


class TTable:public TView
{

public:

  TTable(TRect & r);
  virtual void draw();
  virtual void handleEvent(TEvent & event);
  void charFocused();

};


class TReport:public TView
{

public:

  TReport(TRect & r);
  virtual void draw();
  virtual void handleEvent(TEvent & event);

private:

  unsigned char asciiChar;

};

class TAsciiChart:public TWindow
{

public:

  TAsciiChart();

};

#endif // __ASCII_H
