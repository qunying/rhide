#define Uses_TWindow
#define Uses_TStaticText
#define Uses_TDialog
#define Uses_TEventQueue
#define Uses_TProgram
#define Uses_TDeskTop
#include <tv.h>
#include "ascii.h"
#include "puzzle.h"
#include "calendar.h"
#include "mousedlg.h"
#include <libtvdem.h>

TWindow *PuzzleWindow = NULL;
TWindow *CalendarWindow = NULL;
TWindow *AsciiWindow = NULL;

void
Puzzle()
{
  if (!PuzzleWindow)
    PuzzleWindow = new TPuzzleWindow();
}

void
Calendar()
{
  if (!CalendarWindow)
    CalendarWindow = new TCalendarWindow();
}

void
Ascii()
{
  if (!AsciiWindow)
    AsciiWindow = new TAsciiChart();
}

void
MouseDlg()
{
  unsigned short reverse = 0;
  TMouseDialog *mouseDlg = new TMouseDialog();

  if (TEventQueue::mouseReverse == True)
    reverse = 1;
  mouseDlg->setData(&reverse);
  if (TProgram::deskTop->execView(mouseDlg) != cmCancel)
  {
    mouseDlg->getData(&reverse);
    TEventQueue::mouseReverse = reverse ? True : False;
  }
  destroy(mouseDlg);
}
