/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#define Uses_TApplication
#define Uses_MsgBox
#define Uses_TDeskTop
#define Uses_TFileDialog
#define Uses_TLabel
#define Uses_TRect
#define Uses_TCheckBoxes
#define Uses_TRadioButtons
#define Uses_THistory
#define Uses_TSItem
#define Uses_TButton
#define Uses_TScrollBar
#define Uses_IDEConst
#include <rhide.h>
#include <rhidehis.h>
#include <ideapp.h>

#define Uses_TProject
#include <libide.h>

#define Uses_TLButton
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#define Uses_TCEditor_External
#define Uses_TCEditor_Commands
#include <ceditor.h>
#include <calcu.h>

#define Uses_TInputLinePiped
#define Uses_TSOSSortedListBox
#include <settvuti.h>

#include <string.h>
#include <errno.h>
#include <strstream.h>


#define Uses_SETAppDialogs
#include <../setedit/include/setapp.h>
int SelectFunctionToJump(char *b, unsigned l);

static void ApplyBroadcast(TView *p, void *e)
{
  p->handleEvent(*(TEvent *)e);
}

unsigned doEditDialog( int dialog, ... )
{
    va_list arg;

    char buf[256]; // 80 is not enough
    ostrstream os( buf, sizeof( buf ) );
    switch( dialog )
        {
        case edOutOfMemory:
            return messageBox( _("Not enough memory for this operation"),
                               mfError | mfOKButton );
        case edReadError:
            {
            char *fname;
            va_start( arg, dialog );
            fname = va_arg(arg,char *);
            va_end(arg);
            return messageBox(mfError|mfOKButton,"%s %s. %s(%d)",
                  _("Error reading file "),fname,strerror(errno),errno);
            }
        case edWriteError:
            {
            va_start( arg, dialog );
            os << _("Error writing file ") << va_arg(arg, char *)
               << "." << ends;
            va_end( arg );
            return messageBox( buf, mfError | mfOKButton );
            }
        case edCreateError:
            {
            va_start( arg, dialog );
            os << _("Error creating file ") << va_arg( arg, char *)
               << "." << ends;
            va_end( arg );
            return messageBox( buf, mfError | mfOKButton );
            }
        case edSaveModify:
            {
            va_start( arg, dialog );
            os << va_arg( arg, char *)
               << _(" has been modified. Save?") << ends;
            va_end( arg );
            return messageBox( buf, mfInformation | mfYesNoCancel );
            }
        case edSaveUntitled:
            return messageBox( _("Save untitled file?"),
                               mfInformation | mfYesNoCancel );
        case edSaveAs:
            {
            va_start( arg, dialog );
            ushort retval = execDialog(
             FileOpenDialog(Project.defaultprojectmask,
                    _("Save file as"),_("~N~ame"),fdOpenButton|fdHelpButton,
                    RHIDE_History_source_file,default_directory),
                    va_arg(arg, char *));
            va_end(arg);
            return retval;
            }

        case edFind:
            {
            va_start( arg, dialog );
            return execDialog( createFindDialog(), va_arg(arg, char *));
            }

        case edSearchFailed:
            return messageBox(_("Search string not found."),
                               mfError | mfOKButton );
        case edReplace:
            {
            va_start(arg, dialog);
            return execDialog(createReplaceDialog(), va_arg(arg, char *));
            }

        case edReplacePrompt:
            {
            //  Avoid placing the dialog on the same line as the cursor
            TRect r( 0, 1, 40, 8 );
            r.move( (TProgram::deskTop->size.x-r.b.x)/2, 0 );
            TPoint t = TProgram::deskTop->makeGlobal( r.b );
            t.y++;
            va_start( arg, dialog );
            TPoint *pt = va_arg( arg, TPoint *);
            if( pt->y <= t.y )
                r.move( 0, TProgram::deskTop->size.y - r.b.y - 2 );
            va_end( arg );
            return messageBoxRect( r, _("Replace this occurence?"),
                                   mfYesNoCancel | mfInformation );
            }

        case edJumpToFunction:
            {
             int *p;
             char *bu;
             unsigned l;

             va_start( arg, dialog );
             p=va_arg(arg,int *);
             bu=va_arg(arg,char *);
             l=va_arg(arg,unsigned);

             *p=SelectFunctionToJump(bu,l);

             return (*p!=-1);
            }

        case edSetLocalOptions:
            {
             void *p;
             va_start( arg, dialog );
             p=va_arg(arg,void *);
             return (execDialog( createSetLocalOptions(), p ) == cmOK);
            }

        case edSetGlobalOptions:
            {
             void *p;
             va_start( arg, dialog );
             p=va_arg(arg,void *);
             ushort ret=execDialog( createSetGlobalOptions(), p );
             if (ret==cmYes)
               {
                TEvent event;
                event.what = evBroadcast;
                event.message.command = cmcSetGlobalOptions;
                event.message.infoPtr = p;
                TProgram::deskTop->forEach(ApplyBroadcast,&event);
               }
             return (ret==cmYes) || (ret==cmOK);
            }

        case edLineOverflow:
            return messageBox( _("Line too long, cut it?"),
                               mfError | mfYesButton | mfNoButton);
        }
 return 0;
}

extern "C" int eval(char *mit,char **out);


void ShowCalculator()
{
#if 1
  TRect r(10,2,72,11);
  TDialog *dialog;
  TInputLine *input;
  TInputLine *result;
  ushort retval;
  char *startval;
  dialog = new TDialog(r,_("Calculator"));
  dialog->helpCtx = hcCalculatorDialog;
  r.a.x = 2;
  r.a.y = 2;
  r.b.x = dialog->size.x - 5;
  r.b.y = r.a.y + 1;
  input = new TInputLinePiped(r,255);
  startval = WUC();
  if (startval)
  {
    input->setData(startval);
    string_free(startval);
  }
  dialog->insert(input);
  InitHistoryID(RHIDE_History_Calculator);
  dialog->insert(new THistory(TRect(r.b.x,r.a.y,r.b.x+3,r.b.y), input,
                      RHIDE_History_Calculator));
  r.move(0,-1);
  dialog->insert(new TLabel(r,_("~E~xpression"),input));
  r.move(0,3);
  result = new TInputLinePiped(r,255,tilpNoPipe | tilpNoPaste);
  dialog->insert(result);
  r.move(0,-1);
  dialog->insert(new TLabel(r,_("~R~esult"),result));
  r.move(0,3);
  r.b.x = r.a.x + 12;
  r.b.y = r.a.y + 2;
  dialog->insert(new TLButton(r,_("E~v~al"),cmOK,bfDefault));
  r.a.x = r.b.x + 2;
  r.b.x = r.a.x + 12;
  dialog->insert(new TLButton(r,_("Cancel"),cmCancel,bfNormal));
  r.a.x = r.b.x + 2;
  r.b.x = r.a.x + 12;
  dialog->insert(new TLButton(r,_("~H~elp"),cmHelp,bfNormal));
  input->select();
  dialog->options |= ofCentered;
  TProgram::deskTop->insert(dialog);
  do
  {
    dialog->setState(sfModal, True);
    retval = dialog->execute();
    if (retval == cmOK)
    {
      char input_buffer[256],*ret;
      input->getData(input_buffer);
      int err=eval(input_buffer,&ret);
      if (err)
        {
         sprintf(input_buffer,_("Error in expression (%d)"),err);
         messageBox(input_buffer,mfError | mfOKButton);
        }
      result->setData(ret);
      input->selectAll(True);
    }
  } while (retval == cmOK);
  destroy(dialog);
#else
  executeCalc(NULL);
#endif
}

