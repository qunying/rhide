#define Uses_TStreamableClass
#define Uses_TWindow
#include <tv.h>

#include "ascii.h"

__link(RReport) __link(RTable)
     TStreamableClass
     RAsciiChart(TAsciiChart::name, TAsciiChart::build, __DELTA(TAsciiChart));
