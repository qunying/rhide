/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TMemoryStream )
#define Uses_iopstream
#define _INC_TMEMORYSTREAM
#endif

#if defined( Uses_TMsgCollection )
#define Uses_TNSCollection
#define _INC_TMSGCOLLECTION
#endif

#if defined( Uses_TMsgListBox )
#define Uses_TEnterListBox
#define _INC_TMSGLISTBOX
#endif

#if defined( Uses_MsgRec )
#define _INC_MSGREC
#endif

#if defined( Uses_THintStatusLine )
#define Uses_TStatusLine
#define _INC_THINTSTATUSLINE
#endif

#if defined( Uses_TCheckDialog )
#define Uses_TDialog
#define _INC_TCHECKDIALOG
#endif

#if defined( Uses_TLButton )
#define Uses_TButton
#define _INC_TLBUTTON
#endif

#if defined( Uses_TEnterRadioButtons )
#define Uses_TRadioButtons
#define _INC_TENTERRADIOBUTTONS
#endif

#if defined( Uses_TEnterCheckBoxes )
#define Uses_TCheckBoxes
#define _INC_TENTERCHECKBOXES
#endif

#if defined( Uses_TIntInputLine )
#define Uses_TEnterInputLine
#define _INC_TINTINPUTLINE
#endif

#if defined( Uses_TMemInfo )
#define Uses_TView
#define _INC_TMEMINFO
#endif

#if defined( Uses_TParamList )
#define Uses_TCollection
#define _INC_TPARAMLIST
#endif

#if defined( Uses_TDirList )
#define Uses_TCollection
#define _INC_TDIRLIST
#endif

#if defined( Uses_TWindowList )
#define Uses_TEnterListBox
#define _INC_TWINDOWLIST
#endif

#if defined( Uses_TEnterListBox )
#define Uses_TListViewer
#define _INC_TENTERLISTBOX
#endif

#if defined( Uses_TEnterInputLine )
#define Uses_TInputLine
#define _INC_TENTERINPUTLINE
#endif

#if defined( Uses_TSCollection )
#define Uses_TCollection
#define _INC_TSCOLLECTION
#endif

#if defined( Uses_tvutilCommands )
#define _INC_TVUTILCOMMANDS
#endif

#define s(TYPE)\
  TStreamableClass R##TYPE( T##TYPE::name, T##TYPE::build, __DELTA(T##TYPE));

#define STREAMIO(CLASS)\
public:\
    static const char * const name;\
    static TStreamable *build();\
protected:\
    CLASS(StreamableInit);\
private:\
    virtual const char *streamableName() const\
	{ return name; }

#define STREAMINLINES(CLASS)\
inline ipstream& operator >> ( ipstream& is, CLASS& cl )\
    { return is >> (TStreamable&)cl; }\
inline ipstream& operator >> ( ipstream& is, CLASS*& cl )\
    { return is >> (void *&)cl; }\
inline opstream& operator << ( opstream& os, CLASS& cl )\
    { return os << (TStreamable&)cl; }\
inline opstream& operator << ( opstream& os, CLASS* cl )\
    { return os << (TStreamable *)cl; }

#define STDSTREAMFUNCS(CLASS,BASE)\
CLASS::CLASS(StreamableInit) :\
  BASE(streamableInit)\
{\
}\
TStreamable * CLASS::build()\
{\
  return new CLASS(streamableInit);\
}


#include <tv.h>

#if defined( _INC_TENTERRADIOBUTTONS )
#include <tenterra.h>
#endif

#if defined( _INC_TENTERCHECKBOXES )
#include <tenterch.h>
#endif

#if defined( _INC_TVUTILCOMMANDS )
#include <tvutilco.h>
#endif

#if defined( _INC_TMEMINFO )
#include <tmeminfo.h>
#endif

#if defined( _INC_TDIRLIST )
#include <tdirlist.h>
#endif

#if defined( _INC_TPARAMLIST )
#include <tparamli.h>
#endif

#if defined( _INC_TENTERINPUTLINE )
#include <tenterin.h>
#endif

#if defined( _INC_TINTINPUTLINE )
#include <tintinpu.h>
#endif

#if defined( _INC_TENTERLISTBOX )
#include <tenterli.h>
#endif

#if defined( _INC_TWINDOWLIST )
#include <twindowl.h>
#endif

#if defined( _INC_TSCOLLECTION )
#include <tscollec.h>
#endif

#if defined( _INC_TLBUTTON )
#include <tlbutton.h>
#endif

#if defined( Uses_tvutilFunctions )
#include <tvutilfu.h>
#endif

#if defined( _INC_TCHECKDIALOG )
#include <tcheckdi.h>
#endif

#if defined( _INC_THINTSTATUSLINE )
#include <thintsta.h>
#endif

#if defined( _INC_TMSGCOLLECTION )
#include <tmsgcoll.h>
#endif

#if defined( _INC_TMSGLISTBOX )
#include <tmsglist.h>
#endif

#if defined( _INC_MSGREC )
#include <msgrec.h>
#endif

#if defined( _INC_TMEMORYSTREAM )
#include <tmemstrm.h>
#endif
