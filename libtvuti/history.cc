/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_opstream
#define Uses_ipstream
#include <tv.h>

#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <stdlib.h>

int hist_id_count = 0;
int *hist_ids = NULL;

static int
id_index(int id)
{
  int i;

  for (i = 0; i < hist_id_count; i++)
  {
    if (hist_ids[i] == id)
      return i;
  }
  return -1;
}

static int
insert_id(int id)
{
  hist_id_count++;
  hist_ids = (int *) realloc(hist_ids, hist_id_count * sizeof(int));

  hist_ids[hist_id_count - 1] = id;
  return hist_id_count - 1;
}

void
InitHistoryID(int id)
{
  int index = id_index(id);

  if (index != -1)
    return;
  insert_id(id);
}

void
ClearHistoryIDs()
{
  hist_id_count = 0;
  free(hist_ids);
  hist_ids = NULL;
  InitHistoryIDs();
}

static void
SaveHistoryID(opstream & os, int id)
{
  int i, count;

  count = historyCount(id);
  os << count;
  for (i = count; i > 0; i--)
    os.writeString(historyStr(id, i - 1));
}

void
SaveHistoryIDs(opstream & os)
{
  int i;

  os << hist_id_count;
  for (i = 0; i < hist_id_count; i++)
  {
    os << hist_ids[i];
    SaveHistoryID(os, hist_ids[i]);
  }
}

static void
LoadHistoryID(ipstream & is, int id)
{
  int i, count;

  is >> count;
  for (i = 0; i < count; i++)
  {
    char *tmp = is.readString();

    historyAdd(id, tmp);
    delete(tmp);
  }
}

void initHistory();
void doneHistory();

void
LoadHistoryIDs(ipstream & is)
{
  doneHistory();
  initHistory();
  ClearHistoryIDs();
  int i, count, id;

  is >> count;
  for (i = 0; i < count; i++)
  {
    is >> id;
    InitHistoryID(id);
    LoadHistoryID(is, id);
  }
}
