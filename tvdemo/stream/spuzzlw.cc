#define Uses_TStreamableClass
#define Uses_TWindow
#include <tv.h>

#include "puzzle.h"

__link(__RPuzzleView)
     TStreamableClass RPuzzleWindow(TPuzzleWindow::name,
                                    TPuzzleWindow::build,

                                    __DELTA(TPuzzleWindow));
