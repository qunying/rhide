/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if !defined( tvutilFunctions__ )
#define tvutilFunctions__

class TWindow;
void AddReference(TWindow ** window);
void AddWindow(TWindow *, TWindow ** ref = NULL, Boolean before = False,
               Boolean remember_closed = False, Boolean is_removable = True,
               int old_number = 0);
void RemoveWindow(TWindow *, Boolean remember_closed = True);
void UpdateWindow(TWindow * window);
void ShowWindowList(const char *);
void ClearWindowList();
int IsClosedWindow(const char *);
TWindow *OpenClosedWindow(int);
class opstream;
void SaveClosedWindows(opstream &);
class ipstream;
void LoadClosedWindows(ipstream &);

class TSCollection;
extern TSCollection *windows;
extern TSCollection *closed_windows;
extern int max_closed;

#define DESKTOPWINDOW(index) (((DeskTopWindow *)windows->at(index))->window)

class TParamList;
ushort editParamList(TParamList * paramlist, const char *title, int hist_id);

class TDirList;
void editDirList(TDirList * dirlist, const char *title, int hist_id);

class TRect;
ushort BigmessageBox(const char *msg, ushort aOptions);
ushort BigmessageBox(ushort aOptions, const char *fmt, ...)
  __attribute__ ((format(printf, 2, 3)));
ushort BiginputBox(const char *Title, const char *aLabel, char *s, int limit);

class TInputLine;
extern TInputLine *(*CreateInputLine) (const TRect & rect, int aMaxLen);

ushort HistinputBox(const char *Title, const char *aLabel, char *s,

                    int limit, int hist_id);

ushort HistinputBoxRect(const TRect & bounds,

                        const char *Title,
                        const char *aLabel, char *s, int limit, int hist_id);

class TValidator;
ushort ValidInputBox(const char *Title, const char *aLabel, char *s,
                     int limit, TValidator * validator);

extern int hist_id_count;
extern int *hist_ids;
void InitHistoryID(int);
void ClearHistoryIDs();
class ipstream;
void LoadHistoryIDs(ipstream &);
class opstream;
void SaveHistoryIDs(opstream &);

int InputInt(int &retval, const char *title, const char *label_name);
int InputUInt(unsigned int &retval, const char *title,

              const char *label_name);

void add_command_constant(const char *name, unsigned short value);
void add_help_constant(const char *name, unsigned short value);

class TDialog;
ushort execDialog(TDialog * d, void *data);

extern void (*InitHistoryIDs) ();
#endif
