/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include "libgdbrh.h"
#include "rhgdbint.h"

/* This is a dummy function, which is called, by a file from
    libgdb.a but it is defined in main.c from gdb which does
    nothing */

void
proc_remove_foreign(int pid __attribute__ ((unused)))
{
}

int rh_annotate;
static int ignore_count_changed = 0;

void
breakpoints_changed()
{
  DEBUG("|breakpoints_changed|");
  ignore_count_changed = 0;     /*
                                   Avoid multiple break annotations. 
                                 */
}

void
annotate_ignore_count_change(void)
{
  ignore_count_changed = 1;
}

void
annotate_breakpoint(int num __attribute__ ((unused)))
{
  DEBUG("|breakpoint(%d)|", num);
}

void
annotate_watchpoint(int num __attribute__ ((unused)))
{
  DEBUG("|watchpoint(%d)|", num);
}

void
annotate_breakpoints_headers()
{
  DEBUG("|breakpoints_headers|");
}

void
annotate_breakpoints_table()
{
  DEBUG("|breakpoints_table|");
}

void
annotate_record()
{
  DEBUG("|record|");
}

void
annotate_breakpoints_table_end()
{
  DEBUG("|breakpoints_table_end|");
}

void
annotate_frames_invalid()
{
  DEBUG("|frames_invalid|");
}

void
annotate_quit()
{
  DEBUG("|quit|");
}

void
annotate_arg_begin()
{
  DEBUG("|arg_begin|");
}

void
annotate_arg_name_end()
{
  DEBUG("|arg_name_end|");
}

void
annotate_arg_value(struct type *type __attribute__ ((unused)))
{
  DEBUG("|arg_value|");
}

void
annotate_arg_end()
{
  DEBUG("|arg_end|");
}

void
annotate_source(char *filename __attribute__ ((unused)),
                int line __attribute__ ((unused)),
                int character __attribute__ ((unused)),
                int mid __attribute__ ((unused)),
                CORE_ADDR pc __attribute__ ((unused)))
{
  DEBUG("|source(%s,%d,%d,%d,%ld)|", filename, line, character, mid, pc);
}

void
annotate_function_call()
{
  DEBUG("|function_call|");
}

void
annotate_signal_handler_caller()
{
  DEBUG("|signal_handler_caller|");
}

void
annotate_array_section_begin(int index __attribute__ ((unused)),
                             struct type *elttype __attribute__ ((unused)))
{
  DEBUG("|array_section_begin(%d)|", index);
}

void
annotate_elt_rep(unsigned int repcount __attribute__ ((unused)))
{
  DEBUG("|elt_rep(%d)|", repcount);
}

void
annotate_elt_rep_end()
{
  DEBUG("|elt_rep_end|");
}

void
annotate_elt()
{
  DEBUG("|elt|");
}

void
annotate_array_section_end()
{
  DEBUG("|array_section_end|");
}

void
annotate_display_begin()
{
  DEBUG("|display_begin|");
}

void
annotate_display_number_end()
{
  DEBUG("|display_number_end|");
}

void
annotate_display_format()
{
  DEBUG("|display_format|");
}

void
annotate_display_expression()
{
  DEBUG("|display_expression|");
}

void
annotate_display_expression_end()
{
  DEBUG("|display_expression_end|");
}

void
annotate_display_value()
{
  DEBUG("|display_value|");
}

void
annotate_display_end()
{
  DEBUG("|display_end|");
}

void
annotate_catchpoint(int number __attribute__ ((unused)))
{
  DEBUG("|catchpoint(%d)|", number);
}

void
annotate_field(int num __attribute__ ((unused)))
{
  _DEBUG("a_field(%d)\n", num);
}

void
annotate_field_begin(struct type *type __attribute__ ((unused)))
{
  _DEBUG("a_field_begin\n");
}

void
annotate_field_name_end()
{
  _DEBUG("a_field_name_end\n");
}

void
annotate_field_value()
{
  _DEBUG("a_field_value\n");
}

void
annotate_field_end()
{
  _DEBUG("a_field_end\n");
}

void
annotate_value_history_begin(int histindex __attribute__ ((unused)),
                             struct type *type __attribute__ ((unused)))
{
  _DEBUG("a_value_history_begin(%d)\n", histindex);
}

void
annotate_value_begin(struct type *type __attribute__ ((unused)))
{
  _DEBUG("a_value_begin\n");
}

void
annotate_value_history_value()
{
  _DEBUG("a_value_history_value\n");
}

void
annotate_value_history_end()
{
  _DEBUG("a_value_history_end\n");
}

void
annotate_value_end()
{
  _DEBUG("a_value_end\n");
}

void
_initialize_annotate()
{
}

void _rhgdb_annotate_frame_begin(int level, CORE_ADDR pc);

void
annotate_frame_begin(int level __attribute__ ((unused)), CORE_ADDR pc)
{
  DEBUG("|frame_begin(%d,%ld)|", level, pc);
  _rhgdb_annotate_frame_begin(level, pc);
}

void
annotate_frame_address()
{
  /*
     The address is set frame_begin 
   */
  DEBUG("|frame_address|");
}

void
annotate_frame_address_end()
{
  DEBUG("|frame_address_end|");
}

void _rhgdb_annotate_frame_function_name();

void
annotate_frame_function_name()
{
  DEBUG("|frame_function_name|");
  _rhgdb_annotate_frame_function_name();
}

void _rhgdb_annotate_frame_args();

void
annotate_frame_args()
{
  DEBUG("|frame_args|");
  _rhgdb_annotate_frame_args();
}

void _rhgdb_annotate_frame_source_begin();

void
annotate_frame_source_begin()
{
  DEBUG("|frame_source_begin|");
  _rhgdb_annotate_frame_source_begin();
}

void _rhgdb_annotate_frame_source_file();

void
annotate_frame_source_file()
{
  DEBUG("|frame_source_file|");
  _rhgdb_annotate_frame_source_file();
}

void _rhgdb_annotate_frame_source_file_end();

void
annotate_frame_source_file_end()
{
  DEBUG("|frame_source_file_end|");
  _rhgdb_annotate_frame_source_file_end();
}

void _rhgdb_annotate_frame_source_line();

void
annotate_frame_source_line()
{
  DEBUG("|frame_source_line|");
  _rhgdb_annotate_frame_source_line();
}

void _rhgdb_annotate_frame_source_end();

void
annotate_frame_source_end()
{
  DEBUG("|frame_source_end|");
  _rhgdb_annotate_frame_source_end();
}

void
annotate_frame_where()
{
  DEBUG("|frame_where|");
}

void _rhgdb_annotate_frame_end();

void
annotate_frame_end()
{
  _rhgdb_annotate_frame_end();
}
