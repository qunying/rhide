/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_tvutilFunctions
#include <libtvuti.h>

static void
__InitHistoryIDs()
{
}

void (*InitHistoryIDs) () = __InitHistoryIDs;
