#define Uses_TStreamableClass
#define Uses_TWindow
#include <tv.h>

#include "calendar.h"

__link(RCalendarView)
     TStreamableClass RCalendarWindow(TCalendarWindow::name,
                                      TCalendarWindow::build,

                                      __DELTA(TCalendarWindow));
