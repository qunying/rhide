/* Copyright (C) 1996-1998 Robert H�hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdb.h>
#include <rhgdbint.h>
#include <librhgdb.h>
#include <stdlib.h>

static void init_frame_entry(frame_entry *fe)
{
  fe->file_name = NULL;
  fe->function_name = NULL;
  fe->args = NULL;
  fe->line_number = 0;
  fe->address = 0;
}

static void clear_frame_entry(frame_entry *fe)
{
  if (fe->file_name) free(fe->file_name);
  if (fe->function_name) free(fe->function_name);
  if (fe->args) free(fe->args);
  init_frame_entry(fe);
}

static frame_entry *new_frame_entry()
{
  frame_entry *fe;
  fe = (frame_entry *)malloc(sizeof(frame_entry));
  init_frame_entry(fe);
  return fe;
}

frame_entry **frames = NULL;
int frame_count = 0;
static int frame_size = 0;

static void resize_frames()
{
  if (frame_count >= frame_size)
  {
    int i;
    frame_size = frame_count + 1;
    frames = (frame_entry **)realloc(frames,frame_size*sizeof(frame_entry *));
    for (i=frame_count;i<frame_size;i++)
      frames[i] = new_frame_entry();
  }
}

static frame_entry *add_frame_entry()
{
  resize_frames();
  return frames[frame_count++];
}

static void clear_frames()
{
  int i;
  for (i=0;i<frame_count;i++)
    clear_frame_entry(frames[i]);
  frame_count = 0;
}

static int frame_begin_seen = 0;
static int record_frames;
static unsigned long current_address;
static int current_line_number;
static int function_start;
static int function_end;
static int args_start;
static int args_end;
static int file_start;
static int file_end;
static int line_start;
static int line_end;

void
annotate_frame_begin (level, pc)
     int level;
     CORE_ADDR pc;
{
  DEBUG("|frame_begin(%d,%ld)|",level,pc);
  frame_begin_seen = 1;
  current_address = pc;
  current_line_number = -1;
  function_start = -1;
  function_end = -1;
  args_start = -1;
  args_end = -1;
  file_start = -1;
  file_end = -1;
  line_start = -1;
  line_end = -1;
}

void
annotate_frame_address ()
{
  /* The address is set frame_begin */
  DEBUG("|frame_address|");
}

void
annotate_frame_address_end ()
{
  DEBUG("|frame_address_end|");
}

void
annotate_frame_function_name ()
{
  DEBUG("|frame_function_name|");
  /* remember the start of the function name */
  function_start = gdb_output_pos;
}

void
annotate_frame_args ()
{
  DEBUG("|frame_args|");
  function_end =
  args_start = gdb_output_pos;
}

void
annotate_frame_source_begin ()
{
  DEBUG("|frame_source_begin|");
  args_end = gdb_output_pos;
}

void
annotate_frame_source_file ()
{
  DEBUG("|frame_source_file|");
  file_start = gdb_output_pos;
}

void
annotate_frame_source_file_end ()
{
  DEBUG("|frame_source_file_end|");
  file_end = gdb_output_pos;
}

void
annotate_frame_source_line ()
{
  DEBUG("|frame_source_line|");
  line_start = gdb_output_pos;
}

void
annotate_frame_source_end ()
{
  DEBUG("|frame_source_end|");
  line_end = gdb_output_pos;
}

void
annotate_frame_where ()
{
  DEBUG("|frame_where|");
}

void
annotate_frame_end ()
{
  frame_entry *fe;
  char c;
  DEBUG("|frame_end|");
  if (!record_frames || !frame_begin_seen) return;
  /* This can happen, when the function has no debugging information */
  if (args_start >= 0 && args_end < 0)
    args_end = gdb_output_pos;
  frame_begin_seen = 0;
  fe = add_frame_entry();
  fe->address = current_address;
  if (function_start >= 0)
  {
    c = gdb_output_buffer[function_end];
    gdb_output_buffer[function_end] = 0;
    fe->function_name = strdup(gdb_output_buffer+function_start);
    gdb_output_buffer[function_end] = c;
  }
  if (file_start >= 0)
  if (function_start >= 0)
  {
    c = gdb_output_buffer[file_end];
    gdb_output_buffer[file_end] = 0;
    fe->file_name = strdup(gdb_output_buffer+file_start);
    gdb_output_buffer[file_end] = c;
  }
  if (args_start >= 0)
  {
    if (gdb_output_buffer[args_end-1] == '\n') args_end--;
    c = gdb_output_buffer[args_end];
    gdb_output_buffer[args_end] = 0;
    fe->args = strdup(gdb_output_buffer+args_start);
    gdb_output_buffer[args_end] = c;
  }
  if (line_start >= 0)
  {
    c = gdb_output_buffer[line_end];
    gdb_output_buffer[line_end] = 0;
    sscanf(gdb_output_buffer+line_start,"%d",&fe->line_number);
    gdb_output_buffer[line_end] = c;
  }
}

void BackTrace()
{
  clear_frames();
  record_frames = 1;
  frame_begin_seen = 0;
  Command("backtrace",0);
  record_frames = 0;
}

void Finish()
{
  Command("finish",0);
}

void SetFrame(level)
  int level;
{
  char command[256];
  sprintf(command,"f %d",level);
  Command(command,0);
}


