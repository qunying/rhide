/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TInspector )
#define Uses_TDialog
#define _INC_TINSPECTOR
#endif

#if defined( Uses_TInspectList )
#define Uses_TListBox
#define _INC_TINSPECTLIST
#endif

#if defined( Uses_TDisassemblerWindow )
#define Uses_TDialog
#define _INC_TDISASSEMBLERWINDOW
#endif

#if defined( Uses_TDataWindow )
#define Uses_TDialog
#define _INC_TDATAWINDOW
#endif

#if defined( Uses_TRegisterWindow )
#define Uses_TWindow
#define _INC_TREGISTERWINDOW
#endif

#if defined( Uses_TFunctionInputLine )
#define Uses_TEnterInputLine
#define _INC_LIBTVUTIL
#define _INC_TFUNCTIONINPUTLINE
#endif

#if defined( Uses_TFunctionListBox )
#define Uses_TSortedListBox
#define _INC_TFUNCTIONLISTBOX
#endif

#if defined( Uses_TGDBWindow )
#define Uses_TWindow
#define _INC_TGDBWINDOW
#endif

#if defined( Uses_tvgdbCommands )
#define Uses_tvutilFunctions
#define _INC_LIBTVUTIL
#define _INC_TVGDBCOMMANDS
#endif

#if defined( Uses_TWatchDialog )
#define Uses_TDialog
#define _INC_TWATCHDIALOG
#endif

#if defined( Uses_TInspectDialog )
#define Uses_TDialog
#define _INC_TINSPECTDIALOG
#endif

#if defined( Uses_TBreakListBox )
#define Uses_TEnterListBox
#define _INC_TBREAKLISTBOX
#define _INC_LIBTVUTIL
#endif

#if defined( Uses_TWatchListBox )
#define Uses_TEnterListBox
#define _INC_LIBTVUTIL
#define _INC_TWATCHLISTBOX
#endif

#if defined( Uses_TWatchWindow )
#define Uses_TWindow
#define _INC_TWATCHWINDOW
#endif

#include <tv.h>

#if defined( _INC_LIBTVUTIL )
#include <libtvuti.h>
#endif

#if defined( _INC_TVGDBCOMMANDS )
#include <tvgdbcom.h>
#endif

#if defined( _INC_TGDBWINDOW )
#include <tgdbwind.h>
#endif

#if defined( _INC_TWATCHDIALOG )
#include <twatchdi.h>
#endif

#if defined( _INC_TINSPECTDIALOG)
#include <inspectd.h>
#endif

#if defined( _INC_TWATCHLISTBOX )
#include <twatchli.h>
#endif

#if defined( _INC_TWATCHWINDOW )
#include <twatchwi.h>
#endif

#if defined( _INC_TBREAKLISTBOX )
#include <tbreakli.h>
#endif

#if defined( _INC_TFUNCTIONLISTBOX )
#include <tfunctio.h>
#endif

#if defined( _INC_TFUNCTIONINPUTLINE )
#include <tfuncinp.h>
#endif

#if defined( _INC_TREGISTERWINDOW )
#include <tregwin.h>
#endif

#if defined( _INC_TDISASSEMBLERWINDOW )
#include <tdiswin.h>
#endif

#if defined( Uses_tvgdbFunctions )
#include <tvgdbfun.h>
#endif

#if defined( _INC_TDATAWINDOW )
#include <tdatawin.h>
#endif

#if defined( _INC_TINSPECTLIST )
#include <inspectl.h>
#endif

#if defined( _INC_TINSPECTOR )
#include <inspect.h>
#endif
