/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( tvgdbFunctions__ )
#define tvgdbFunctions__

void Evaluate(char *watch = NULL);
void AddWatchEntry(char *var = NULL);
void AddWatch(char *buffer, Boolean show_window = True);
void OpenWatchWindow();
void DeleteAllWatches();
void DeleteAllBreakPoints();
class TDialog;
extern TDialog *watchwindow;
class TWatchListBox;
extern TWatchListBox *watches;
class TSCollection;
extern TSCollection Watches;
void BreakDialog();
void UpdateGDBOutWin(char *buffer);
void InitGDBOutWin();
void UserWarning(int number, ...);
struct function_entry;
ushort SelectFunction(const char *title, const char *regex, char *full_name,
                      char *GDB_name, function_entry ** func = NULL);

void ShowCallStackWindow();
void UpdateCallStackWindow();
extern TDialog *CallStackWindow;

class TRect;
extern TRect WatchWindowRect;
extern TRect CallStackWindowRect;

void AddDataWindow();
void ShowStackWindow();

#endif
