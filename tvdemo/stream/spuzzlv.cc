#define Uses_TStreamableClass
#define Uses_TWindow
#include <tv.h>

#include "puzzle.h"

TStreamableClass RPuzzleView(TPuzzleView::name,

                             TPuzzleView::build, __DELTA(TPuzzleView));
