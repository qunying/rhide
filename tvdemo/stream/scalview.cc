#define Uses_TStreamableClass
#define Uses_TWindow
#include <tv.h>

#include "calendar.h"

TStreamableClass RCalendarView(TCalendarView::name,
                               TCalendarView::build, __DELTA(TCalendarView));
