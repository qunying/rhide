/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define Uses_TStringCollection
#define Uses_TSortedListBox
#define Uses_MsgBox
#define Uses_TDialog
#define Uses_TLabel
#define Uses_TCheckBoxes
#define Uses_TRadioButtons
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TSItem
#define Uses_TScrollBar
#define Uses_TRangeValidator
#define Uses_TKeys
#define Uses_TNSSortedCollection
#define Uses_TStringCollection
#define Uses_SOStack
#define Uses_TNoCaseSOSStringCollection
#define Uses_TNoCaseStringCollection

#define Uses_TParamList
#define Uses_TLButton
#define Uses_TIntInputLine

#define Uses_TProject
#include <libide.h>
#include <settvuti.h>
#include <libtvuti.h>

#include <infbase.h>

#include "rhide.h"
#include <rhutils.h>

struct node_info;

struct file_info
{
  char *filename;
  node_info **nodes;
  node_info **inodes;
  int node_count;
  char *origname;
};

struct node_info
{
  char *name;
  char *iname;
  char *real_name;
  file_info *file;
};

static file_info **files = NULL;
static int file_count = 0;

static void
add_node_from_file(file_info * file, node_info * node)
{
  file->node_count++;
  file->nodes = (node_info **) realloc(file->nodes,
                                       file->node_count *
                                       sizeof(node_info *));
  file->inodes =
    (node_info **) realloc(file->inodes,
                           file->node_count * sizeof(node_info *));
  node->file = file;
  file->nodes[file->node_count - 1] = node;
  file->inodes[file->node_count - 1] = node;
}

static void
add_node_from_file(char *node_name, char *visible_name, file_info * file)
{
  node_info *node;

  node = (node_info *) malloc(sizeof(node_info));
  node->name = strdup(node_name);
  node->iname = strdup(node_name);
  node->real_name = NULL;
  string_down(node->iname);
  add_node_from_file(file, node);
  if (!visible_name)
    return;
  node = (node_info *) malloc(sizeof(node_info));
  node->name = strdup(visible_name);
  node->iname = strdup(visible_name);
  string_down(node->iname);
  node->real_name = strdup(node_name);
  add_node_from_file(file, node);
}

static int
node_compare(const void *key1, const void *key2)
{
  return strcmp(((node_info **) key1)[0]->name,
                ((node_info **) key2)[0]->name);
}

static int
node_compare_ignore(const void *key1, const void *key2)
{
  return strcasecmp(((node_info **) key1)[0]->iname,
                    ((node_info **) key2)[0]->iname);
}

static void
remove_nodes_from_file(int index)
{
  int i;
  file_info *file = files[index];

  if (file_count > index + 1)
    memcpy(files + index, files + index + 1,
           (file_count - index - 1) * sizeof(file_info *));
  files = (file_info **) realloc(files, file_count * sizeof(file_info *));
  for (i = 0; i < file->node_count; i++)
  {
    node_info *node = file->nodes[i];

    free(node->name);
    free(node->iname);
    if (node->real_name)
      free(node->real_name);
    free(node);
  }
  free(file->nodes);
  free(file->inodes);
  free(file->origname);
  free(file);
  file_count--;
  if (!file_count)
  {
    free(files);
    files = NULL;
  }
}

static int
add_nodes_from_topic(char *name)
{
  int i, count;
  TInfFile *inf;
  TInfTopic *topic;
  file_info *file;
  char *fname;

  fname = strchr(name, ')');
  if (!fname)
    return 0;
  *fname = 0;
  inf = new TInfFile(name + 1, 0);
  *fname = ')';
  if (!inf)
    return 0;
  if (inf->Status == True)
  {
    delete inf;

    return 0;
  }
// FIXME!!
/* Ther is currently no other way for me,
   to see if I'm compiling with the InfView
   from setedit 0.4.45.
*/
#ifdef InfV_UseBinaryFile
  int suggY;
  topic = inf->getTopic(fname + 1, 0, moinHideNodeLink, suggY);
#else
  topic = inf->getTopic(fname + 1, 0, moinHideNodeLink);
#endif
  if (!topic || inf->Status == True
      || (count = topic->getNumCrossRefs()) == 0)
  {
    if (topic)
      delete topic;
    delete inf;

    return 0;
  }
  count = topic->getNumCrossRefs();
  file = (file_info *) malloc(sizeof(file_info));
  file_count++;
  files = (file_info **) realloc(files, file_count * sizeof(file_info *));
  files[file_count - 1] = file;
  file->node_count = 0;
  *fname = 0;
  file->filename = strdup(name + 1);
  *fname = ')';
  file->nodes = NULL;
  file->inodes = NULL;
  file->origname = strdup(name);
  for (i = 0; i < count; i++)
  {
    add_node_from_file(topic->crossRefs[i].Name,
                       topic->crossRefs[i].Name2, file);
  }
  qsort(file->nodes, file->node_count, sizeof(node_info *), node_compare);
  qsort(file->inodes, file->node_count, sizeof(node_info *),
        node_compare_ignore);
  delete topic;
  delete inf;

  return 1;
}

static int
add_nodes_from_file(char *fname)
{
  int i, count;
  TInfFile *inf;
  file_info *file;

  if (*fname == '(')
    return add_nodes_from_topic(fname);
  inf = new TInfFile(fname, 0);
  if (!inf)
    return 0;
  if (inf->Status == True || (count = inf->index->coll->getCount()) == 0)
  {
    delete inf;

    return 0;
  }
  file = (file_info *) malloc(sizeof(file_info));
  file_count++;
  files = (file_info **) realloc(files, file_count * sizeof(file_info *));
  files[file_count - 1] = file;
  file->node_count = 0;
  file->filename = strdup(fname);
  file->nodes = NULL;
  file->inodes = NULL;
  file->origname = strdup(fname);
  for (i = 0; i < count; i++)
  {
    add_node_from_file(inf->index->coll->GetString(inf->index->coll->at(i)),
                       NULL, file);
  }
  qsort(file->nodes, file->node_count, sizeof(node_info *), node_compare);
  qsort(file->inodes, file->node_count, sizeof(node_info *),
        node_compare_ignore);
  delete inf;

  return 1;
}

static node_info *
find_exact_node(const char *word, int &cancel)
{
  node_info info;
  node_info *_info = &info;
  node_info **result = NULL;
  int i, lw = strlen(word);

  cancel = 0;
  info.name = (char *) word;
  info.iname = (char *) alloca(lw + 1);
  strcpy(info.iname, word);
  string_down(info.iname);
  for (i = 0; i < file_count; i++)
  {
    if ((result = (node_info **) bsearch(&_info,
                                         Project.INFO_OPTIONS_CASE ?
                                         files[i]->nodes : files[i]->inodes,
                                         files[i]->node_count,
                                         sizeof(node_info *),
                                         Project.INFO_OPTIONS_CASE ?
                                         node_compare : node_compare_ignore))
        != NULL)
      break;
  }
  return result ? *result : (node_info *) NULL;
}

class TNodeCollection:public TSortedCollection
{
public:
  TNodeCollection(int);
  virtual void *keyOf(void *item);
private:
  int (*STRCMP) (const char *, const char *);
  int ignore_case;
  virtual void *readItem(ipstream &)
  {
    return this;
  };
  virtual void writeItem(void *, opstream &)
  {
  };
  virtual int compare(void *, void *);
  virtual void freeItem(void *)
  {
  }
};

TNodeCollection::TNodeCollection(int _ignore_case):
TSortedCollection(15, 15), ignore_case(_ignore_case)
{
  if (ignore_case)
    STRCMP = strcasecmp;
  else
    STRCMP = strcmp;
  duplicates = True;
}

void *
TNodeCollection::keyOf(void *item)
{
  return ignore_case ?
    ((node_info *) item)->iname : ((node_info *) item)->name;
}

int
TNodeCollection::compare(void *n1, void *n2)
{
  return STRCMP((char *) n1, (char *) n2);
}

class TNodeListBox:public TSortedListBox
{
public:
  TNodeListBox(const TRect & bounds, ushort aNumCols,
               TScrollBar * aScrollBar):TSortedListBox(bounds, aNumCols,
                                                       aScrollBar)
  {
  }
  virtual void getText(char *dest, ccIndex item, short maxLen);
  virtual void handleEvent(TEvent & event);
  virtual void selectItem(ccIndex);
};

void
TNodeListBox::selectItem(ccIndex item)
{
  TSortedListBox::selectItem(item);
  endModal(cmOK);
}

void
TNodeListBox::handleEvent(TEvent & event)
{
  TSortedListBox::handleEvent(event);
  if (!(state & sfSelected))
    return;
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
TNodeListBox::getText(char *dest, ccIndex item, short maxLen)
{
  node_info *node = (node_info *) list()->at(item);
  int fpos = maxLen - 17;
  int nlen = strlen(node->name);

  if (nlen > fpos - 1)
    nlen = fpos - 1;
  memset(dest, ' ', maxLen - 2);
  dest[maxLen - 1] = 0;
  strncpy(dest, node->name, nlen);
  dest[nlen] = ' ';
  strncpy(dest + fpos + 1, node->file->filename, maxLen - fpos - 3);
}

static node_info *
find_node(const char *word,
          int (*find_fkt) (const char *, const char *), int &cancel)
{
  int i;
  char *_word = NULL;
  node_info *first_exact = NULL;
  TNodeCollection coll(!Project.INFO_OPTIONS_CASE);

  cancel = 0;
  if (!Project.INFO_OPTIONS_CASE)
  {
    _word = (char *) alloca(strlen(word) + 1);
    strcpy(_word, word);
    string_down(_word);
  }
  for (i = 0; i < file_count; i++)
  {
    file_info *file = files[i];
    int count = file->node_count;
    int j;

    for (j = 0; j < count; j++)
    {
      node_info *node = file->nodes[j];
      int found = Project.INFO_OPTIONS_CASE ?
        find_fkt(node->name, word) : find_fkt(node->iname, _word);

      if (found)
      {
        coll.insert(node);
        if (found == 1 && !first_exact)
          first_exact = node;
      }
    }
  }
  if (coll.getCount() == 1)
    return (node_info *) coll.at(0);
  if (coll.getCount() > 0)
  {
    node_info *result = NULL;
    TDialog *d = new TDialog(TRect(0, 0, 78, 22), _("Select a topic"));

    d->options |= ofCentered;
    TRect r = d->getExtent();

    r.grow(-1, -1);
    TNodeListBox *tnl = new TNodeListBox(r, 1,
                                         d->

                                         standardScrollBar(sbVertical |
                                                           sbHandleKeyboard));
    d->insert(tnl);
    tnl->newList(&coll);
    if (first_exact)
    {
      ccIndex exact = coll.indexOf(first_exact);

      tnl->focusItem(exact);
    }
    if (TProgram::deskTop->execView(d) == cmOK)
    {
      result = (node_info *) tnl->list()->at(tnl->focused);
    }
    else
      cancel = 1;
    destroy(d);
    return result;
  }
  else
    return NULL;
}

static int
substring_fkt(const char *s1, const char *s2)
{
  return strstr(s1, s2) ? 1 : 0;
}

static node_info *
find_substring_node(const char *word, int &cancel)
{
  return find_node(word, substring_fkt, cancel);
}

// declared in fstrcmp.h from gettext
extern "C" double fstrcmp(const char *, const char *);

#ifndef __DJGPP__
#ifndef INTERNAL_DEBUGGER       // because it is in libgdb
extern "C" void *
xrealloc(void *ptr, int new_size)
{
  return realloc(ptr, new_size);
}
#endif
#endif


static int
fuzzy_fkt(const char *s1, const char *s2)
{
#define MAX_FUZZY 1000
  int result = (int) (fstrcmp(s1, s2) * MAX_FUZZY);

  return result >= Project.fuzzy_value ? (result == MAX_FUZZY ? 1 : 2) : 0;
}

static node_info *
find_fuzzy_node(const char *word, int &cancel)
{
  return find_node(word, fuzzy_fkt, cancel);
}

static int
any_fkt(const char *, const char *)
{
  return 1;
}

static node_info *
find_any_node(const char *word, int &cancel)
{
  return find_node(word, any_fkt, cancel);
}

void
SyntaxIndex()
{
  syntaxhelp("", 1);
}

void
syntaxhelp(const char *word, int create_index)
{
  int i, count, cancel;
  node_info *node = NULL;

  if (!*word)
    create_index = 1;
  count = Project.info_files->getCount();
  if (!files)
  {
    for (i = 0; i < count; i++)
    {
      if (!add_nodes_from_file((char *) Project.info_files->at(i)))
      {
        if (messageBox(mfError | mfYesButton | mfNoButton,
                       _
                       ("Error creating the indices for %s (or nothing found). Remove that file from the list?"),
                       (char *) Project.info_files->at(i)) == cmYes)
        {
          Project.info_files->atFree(i);
          i--;
          count--;
        }
      }
    }
  }
  else
  {
    for (i = 0; i < count; i++)
    {
      int j;
      int found = 0;

      for (j = 0; j < file_count; j++)
      {
        if (strcmp((char *) Project.info_files->at(i), files[j]->origname) ==
            0)
        {
          found = 1;
          break;
        }
      }
      if (!found)
      {
        if (!add_nodes_from_file((char *) Project.info_files->at(i)))
        {
          if (messageBox(mfError | mfYesButton | mfNoButton,
                         _
                         ("Error creating the indices for %s (or nothing found). Remove that file from the list?"),
                         (char *) Project.info_files->at(i)) == cmYes)
          {
            Project.info_files->atFree(i);
            i--;
            count--;
          }
        }
      }
    }
    for (i = 0; i < file_count; i++)
    {
      int found = 0;
      int j;

      for (j = 0; j < count; j++)
      {
        if (strcmp((char *) Project.info_files->at(j), files[i]->origname) ==
            0)
        {
          found = 1;
          break;
        }
      }
      if (!found)
      {
        remove_nodes_from_file(i);
      }
    }
  }
  if (!files)
  {
    messageBox(_("No INFO files for syntax help defined"),
               mfError | mfOKButton);
    return;
  }
  if (create_index)
    node = find_any_node(word, cancel);
  else
    switch (Project.info_search_mode)
    {
      case INFO_MODE_FUZZY:
        node = find_fuzzy_node(word, cancel);
        break;
      case INFO_MODE_SUBSTRING:
        node = find_substring_node(word, cancel);
        break;
      case INFO_MODE_EXACT:
      default:
        node = find_exact_node(word, cancel);
        break;
    }
  if (!node)
  {
    if (!cancel)
      messageBox(_("Couldn't find any matching help"), mfError | mfOKButton);
  }
  else
  {
    if (node->real_name)
      SyntaxHelp(node->real_name, node->file->filename);
    else
      SyntaxHelp(node->name, node->file->filename);
  }
}

void
SyntaxOptions()
{
  TDialog *d;
  TRect r, r1;
  TRadioButtons *radio;
  TCheckBoxes *check;
  TIntInputLine *inp;
  char *tmp;

  d = new TDialog(TRect(0, 0, 42, 15), _("Options for syntax help"));
  d->options |= ofCentered;
  r1 = r = TRect(2, 2, 40, 2 + 3);
  radio = new TRadioButtons(r, new TSItem(_("E~x~act"),
                                          new TSItem(_("~S~ubstring"),
                                                     new TSItem(_("~F~uzzy"),
                                                                NULL))));
  r.move(0, -1);
  tmp = _("Search ~m~ethode");
  r.b.x = r.a.x + cstrlen(tmp) + 1;
  r.b.y = r.a.y + 1;
  d->insert(new TLabel(r, tmp, radio));
  d->insert(radio);
  radio->setData(&Project.info_search_mode);
  r1.a.y = r1.b.y + 2;
  r1.b.y = r1.a.y + 1;
  r = r1;
  check = new TCheckBoxes(r1, new TSItem(_("~C~ase sensitive"), NULL));
  r1.move(0, -1);
  tmp = _("Search o~p~tions");
  r1.b.x = r1.a.x + cstrlen(tmp) + 1;
  d->insert(new TLabel(r1, tmp, check));
  d->insert(check);
  check->setData(&Project.info_search_options);

  r1.a.y = r.b.y + 2;
  r1.b.y = r1.a.y + 1;
  r1.b.x = r1.a.x + 7;
  inp = new TIntInputLine(r1, 4);
  inp->SetValidator(new TRangeValidator(1, 1000));
  r = r1;
  r1.move(0, -1);
  tmp = _("Fu~z~zy value");
  r1.b.x = r1.a.x + cstrlen(tmp) + 1;
  d->insert(new TLabel(r1, tmp, inp));
  d->insert(inp);
  inp->setData(&Project.fuzzy_value);

  r1.a.y = r.b.y + 1;
  r1.b.y = r1.a.y + 2;
  r1.b.x = r1.a.x + 10;
  d->insert(new TLButton(r1, _("~O~K"), cmOK, bfDefault));
  r1.a.x = r1.b.x + 3;
  r1.b.x = r1.a.x + 10;
  d->insert(new TLButton(r1, _("Cancel"), cmCancel, bfNormal));

  radio->select();

  if (TProgram::deskTop->execView(d) == cmOK)
  {
    radio->getData(&Project.info_search_mode);
    check->getData(&Project.info_search_options);
    inp->getData(&Project.fuzzy_value);
  }

  destroy(d);

}
