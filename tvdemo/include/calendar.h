/*-------------------------------------------------------*/
/*                                                       */
/*   Turbo Vision 1.0                                    */
/*   Copyright (c) 1991 by Borland International         */
/*                                                       */
/*   Calendar.h: Header file for Calendar.cpp            */
/*-------------------------------------------------------*/

#if !defined( __CALENDAR_H )
#define __CALENDAR_H

class TCalendarView:public TView
{

public:

  TCalendarView(TRect & r);
  virtual void handleEvent(TEvent & event);
  virtual void draw();

private:

  unsigned days, month, year;
  unsigned curDay, curMonth, curYear;
};

class TCalendarWindow:public TWindow
{

public:

  TCalendarWindow();

};

#endif // __CALENDAR_H
