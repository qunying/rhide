/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#define Uses_TDialog
#define Uses_TScrollBar
#define Uses_TIDEEditWindow
#define Uses_TMsgCollection
#define Uses_TMsgListBox
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_LangTexte
#define Uses_TWindowList
#define Uses_TProject
#include <libide.h>

#define Uses_TSCollection
#define Uses_TMsgCollection
#define Uses_MsgRec
#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#include <rhide.h>
#include <string.h>
#include <stdio.h>

TDialog *msg_window = NULL;
TMsgListBox *msg_list = NULL;
TRect MsgWindowRect(-1,-1,-1,-1);

TCEditWindow *is_on_desktop(const char * fname,Boolean is_full_name)
{
  int i,count;
  char *name1,*bname;
  int file_found;
  if (!windows) return NULL;
  file_found = FindFile(fname,name1);
  bname = strrchr(name1,'/');
  if (!bname)
    bname = name1;
  else
    bname++;
  count = windows->getCount();
  for (i=0;i<count;i++)
  {
    DeskTopWindow *w = (DeskTopWindow *)windows->at(i);
    if ((is_full_name == True && strcmp(fname,w->full_name)==0)
         || (!file_found && (strcmp(bname,w->base_name)==0)))
    {
      string_free(name1);
      return (TCEditWindow *)w->window;
    }
  }
  for (i=0;i<count;i++)
  {
    DeskTopWindow *w = (DeskTopWindow *)windows->at(i);
    if (strcmp(name1,w->full_name) == 0)
    {
      string_free(name1);
      return (TCEditWindow *)w->window;
    }
  }
  string_free(name1);
  return NULL;
}

class TMsgDialog : public TDialog
{
public:
  TMsgDialog(const TRect &r,const char *t) :
    TDialog(r,t),
    TWindowInit(TMsgDialog::initFrame)
  {
  }
  virtual void shutDown();
  virtual void changeBounds(const TRect &);
};

void TMsgDialog::shutDown()
{
  msg_list->hide();
  remove(msg_list);
  msg_list->hScrollBar->hide();
  remove(msg_list->hScrollBar);
  msg_list->vScrollBar->hide();
  remove(msg_list->vScrollBar);
  TDialog::shutDown();
}

void TMsgDialog::changeBounds(const TRect &r)
{
  TDialog::changeBounds(r);
  MsgWindowRect = r;
}

void ShowMessages(TMsgCollection *msgs,Boolean remove_old,
                  Boolean select_first_err, Boolean select_it)
{
  TView *old_focused;
  int old_count = 0;
  TProgram::deskTop->lock();
  old_focused = TProgram::deskTop->current;
  if (!msg_window)
  {
    TScrollBar *scrollbar;
    TRect r;
    if (MsgWindowRect.a.x == -1)
    {
      MsgWindowRect = TProgram::deskTop->getExtent();
      MsgWindowRect.a.y = MsgWindowRect.b.y-7;
    }
    msg_window = new TMsgDialog(MsgWindowRect,_("Message Window"));
    msg_window->flags = wfMove | wfGrow | wfZoom | wfClose;
    msg_window->growMode = gfGrowLoY | gfGrowHiX | gfGrowHiY;
    if (!msg_list)
    {
      r = msg_window->getExtent();
      r.grow(-1,-1);
      scrollbar = msg_window->standardScrollBar(sbVertical | sbHandleKeyboard);
      msg_list = new TMsgListBox(r,1,scrollbar);
      msg_list->growMode = gfGrowHiX | gfGrowHiY;
      scrollbar = msg_window->standardScrollBar(sbHorizontal | sbHandleKeyboard);
      scrollbar->setParams(0,0,255-(r.b.x-r.a.x),40,1);
      msg_list->hScrollBar = scrollbar;
    }
    else
    {
      msg_window->insert(msg_list->hScrollBar);
      msg_window->insert(msg_list->vScrollBar);
    }
    msg_window->insert(msg_list);
    msg_list->show();
    msg_list->hScrollBar->show();
    AddWindow(msg_window,(TWindow **)&msg_window);
  }
  msg_window->hide();
  if ((remove_old==True && !SaveMessages) ||
      msg_list->list() == NULL)
  {
    msg_list->newList(msgs);
  }
  else if (msgs)
  {
    int i, old;
    old = old_count = msg_list->list()->getCount();
    if (msgs->getCount() == 0) old--;
    for (i=0;i<msgs->getCount();i++)
    {
      msg_list->list()->insert(msgs->at(i));
    }
    msgs->removeAll();
    destroy(msgs);
    msg_list->setRange(msg_list->list()->getCount());
    msg_list->focusItem(old);
    msg_list->drawView();
  }
  if (msg_list->list() != NULL && msg_list->list()->getCount()>0)
  {
    if (msg_list->hScrollBar)
    {
      int i,count=msg_list->list()->getCount(),maxlen=0;
      for (i=0;i<count;i++)
      {
        int len = strlen(msg_list->getText(i));
        if (len > maxlen)
          maxlen = len;
      }
      maxlen = maxlen-msg_list->hScrollBar->size.x+2;
      if (maxlen < 0)
        maxlen = 0;
      msg_list->hScrollBar->setRange(0, maxlen);
      msg_list->hScrollBar->setValue(0);
    }
  }
  if (msg_list->list() != NULL && select_first_err == True)
  {
    int i,count=msg_list->list()->getCount();
    for (i=old_count;i<count;i++)
    {
      MsgRec *rec = (MsgRec *)msg_list->list()->at(i);
      if (rec->type == msgError || rec->type == msgWarning)
      {
        msg_list->focusItemNum(i);
        break;
      }
    }
  }
  if (msg_window)
    msg_window->show();
  if (msg_window && (msgs || (select_it == True)))
    msg_window->select();
  else if (old_focused)
    old_focused->select();
  TProgram::deskTop->unlock();
}


