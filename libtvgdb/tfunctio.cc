/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TGroup
#define Uses_TSortedCollection
#define Uses_TScrollBar

#define Uses_TFunctionListBox
#define Uses_tvgdbCommands
#include <libtvgdb.h>

#include <librhgdb.h>
#include <string.h>
#include <stdio.h>

class TFunctionCollection:public TSortedCollection
{
public:
  TFunctionCollection(int init_count);
  virtual void freeItem(void *)
  {
  };
  virtual void *keyOf(void *item);
private:
    virtual int compare(void *key1, void *key2);
  virtual void *readItem(ipstream &)
  {
    return NULL;
  }
  virtual void writeItem(void *, opstream &)
  {
  };
};

void *
TFunctionCollection::keyOf(void *item)
{
  return ((function_entry *) item)->demangled_name;
}

TFunctionCollection::TFunctionCollection(int init_count):
TSortedCollection(init_count, 8)
{
  duplicates = True;
}

int
TFunctionCollection::compare(void *key1, void *key2)
{
  return strcmp((char *) key1, (char *) key2);
}

static function_entry *function_list;
static int function_count = 0;
static TFunctionCollection *functions = NULL;

TFunctionListBox::TFunctionListBox(const TRect & bounds, ushort aNumCols,
                                   TScrollBar * aScrollBar,
                                   const char *regex):
TSortedListBox(bounds, aNumCols, aScrollBar)
{
  int i;

  ListFunctions(regex, &function_list, &function_count);
  destroy(functions);
  functions = new TFunctionCollection(function_count);
  for (i = 0; i < function_count; i++)
  {
    functions->atInsert(i, function_list + i);
  }
  newList(functions);
  helpCtx = hcFunctionListBox;
}

#define INDEX(i) (*((function_entry *)list()->at(i)))

void
TFunctionListBox::getText(char *dest, ccIndex item, short maxlen)
{
  static char _dest[1024];
  int len;

  strcpy(_dest, INDEX(item).demangled_name);
  strcat(_dest, " | ");
  strcat(_dest, _("return type: "));
  strcat(_dest, INDEX(item).return_name);
  strcat(_dest, " | ");
  strcat(_dest, _("file: "));
  strcat(_dest, INDEX(item).file_name);
  strncpy(dest, _dest, maxlen);
  if (hScrollBar)
  {
    len = strlen(_dest);
    len -= (size.x - 1);
    if (len < 0)
      len = 0;
    if (len > hScrollBar->maxVal)
    {
      hScrollBar->setParams(hScrollBar->value, hScrollBar->minVal,
                            len, size.x / 4, hScrollBar->arStep);
    }
  }
}

void
TFunctionListBox::handleEvent(TEvent & event)
{
  TSortedListBox::handleEvent(event);
  switch (event.what)
  {
    case evKeyDown:
      switch (event.keyDown.keyCode)
      {
        case kbEnter:
          if (range > 0)
            selectItem(focused);
          clearEvent(event);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void
TFunctionListBox::selectItem(ccIndex item)
{
  TListBox::selectItem(item);
  owner->endModal(cmOK);
}
