/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if 1
#define TCFileEditor TCEditor
#endif

#if defined( Uses_ideCommands )
#define __INC_IDECOMMA__
#endif

#if defined (Uses_RawKeys)
#include <rawkeys.h>
#endif

#if defined( Uses_ideFunctions)
#define Uses_ideEnums
#define __INC_IDEFUNCT__
#endif

#define Uses_TStreamable
#define Uses_ipstream
#define Uses_opstream

#if defined( Uses_TIDEEditWindow )
#define Uses_TCEditWindow
#define __INC_TIDEEDIT__
#endif

#if defined( Uses_TIDEFileEditor )
#define Uses_TCFileEditor
#define Uses_TCEditor_Internal
#define __INC_TIDEFILE__
#endif

#if defined( Uses_TCEditWindow )
#define Uses_TWindow
#define __INC_CEDITOR__
#endif

#if defined( Uses_TCFileEditor )
#define Uses_TCEditor
#define Uses_TView
#define __INC_CEDITOR__
#endif

#if defined( Uses_TProjectWindow )
#define Uses_TDialog
#define __INC_TPRJWIND__
#endif

#if defined( Uses_TProjectListBox )
#define Uses_TSortedListBox
#define __INC_TPRJLIST__
#endif

#if defined( Uses_TProject )
#define Uses_TDependency
#define __INC_TPROJECT__
#endif

#if defined( Uses_TDepCollection )
#define Uses_TSortedCollection
#define __INC_TDEPENDE__
#endif

#if defined( Uses_TDependency )
#define Uses_TObject
#define Uses_ideEnums
#define Uses_TFileName
#define __INC_TDEPENDE__
#endif

#if defined( Uses_TOptions )
#define Uses_TObject
#define __INC_TOPTIONS__
#endif

#if defined( Uses_TFlagListBox )
#define Uses_TListBox
#define __INC_TFLAGLIS__
#endif

#if defined( Uses_TFlagCollection )
#define Uses_TCollection
#define __INC_TFLAGCOL__
#endif

#if defined( Uses_TFlagEntry )
#define Uses_TObject
#define __INC_TFLAGENT__
#endif

#if defined( Uses_TFileName )
#define _INC_TFILENAME
#endif

#if 0
/*
  with SET's new compatlayer header this is not
  longer possible, since it can be include only once
  and tv.h includes it but for instance ceditor.h
  defines some other needed Uses_CLY... which are then
  ignored :-(
*/
#include <tv.h>
#endif

#if defined( __INC_CEDITOR__ )
#include <ceditor.h>
#endif

/*
  always include libtvuti.h
*/
#include <libtvuti.h>

#if defined( _INC_TFILENAME )
#include <tfname.h>
#endif

#if defined( Uses_ideEnums )
#include <ideenums.h>
#endif

#if defined( __INC_IDECOMMA__ )
#include <idecomma.h>
#endif

#if defined( __INC_CEDITOR__ )
#include <ceditor.h>
#endif

#if defined( __INC_TFLAGENT__ )
#include <tflagent.h>
#endif

#if defined( __INC_TFLAGCOL__ )
#include <tflagcol.h>
#endif

#if defined( __INC_TFLAGLIS__ )
#include <tflaglis.h>
#endif

#if defined( __INC_TOPTIONS__ )
#include <toptions.h>
#endif

#if defined( __INC_TDEPENDE__ )
#include <tdepende.h>
#endif

#if defined( __INC_TPROJECT__ )
#include <tproject.h>
#endif

#if defined( __INC_TPRJLIST__ )
#include <tprjlist.h>
#endif

#if defined( __INC_TPRJWIND__ )
#include <tprjwind.h>
#endif

#if defined( __INC_TIDEEDIT__ )
#include <tideedit.h>
#endif

#if defined( __INC_TIDEFILE__ )
#include <tidefile.h>
#endif

#if defined( __INC_IDEFUNCT__ )
#include <idefunct.h>
#endif

