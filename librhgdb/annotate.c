/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdb.h>
#include <rhgdbint.h>

/* This is a dummy function, which is called, by a file from
    libgdb.a but it is defined in main.c from gdb which does
    nothing */
    
void proc_remove_foreign(int pid)
{
}

int rh_annotate;

void
breakpoints_changed ()
{
DEBUG("|breakpoints_changed|");
}

void
annotate_breakpoint (num)
     int num;
{
DEBUG("|breakpoint(%d)|",num);
}

void
annotate_watchpoint (num)
     int num;
{
DEBUG("|watchpoint(%d)|",num);
}

void
annotate_breakpoints_headers ()
{
DEBUG("|breakpoints_headers|");
}

void
annotate_breakpoints_table ()
{
DEBUG("|breakpoints_table|");
}

void
annotate_record ()
{
DEBUG("|record|");
}

void
annotate_breakpoints_table_end ()
{
DEBUG("|breakpoints_table_end|");
}

void
annotate_frames_invalid ()
{
DEBUG("|frames_invalid|");
}

void
annotate_quit ()
{
DEBUG("|quit|");
}

void
annotate_arg_begin ()
{
DEBUG("|arg_begin|");
}

void
annotate_arg_name_end ()
{
DEBUG("|arg_name_end|");
}

void
annotate_arg_value (type)
     struct type *type;
{
DEBUG("|arg_value|");
}

void
annotate_arg_end ()
{
DEBUG("|arg_end|");
}

void
annotate_source (filename, line, character, mid, pc)
     char *filename;
     int line;
     int character;
     int mid;
     CORE_ADDR pc;
{
DEBUG("|source(%s,%d,%d,%d,%ld)|",filename,line,character,mid,pc);
}

void
annotate_function_call ()
{
DEBUG("|function_call|");
}

void
annotate_signal_handler_caller ()
{
DEBUG("|signal_handler_caller|");
}

void
annotate_array_section_begin (index, elttype)
     int index;
     struct type *elttype;
{
DEBUG("|array_section_begin(%d)|",index);
}

void
annotate_elt_rep (repcount)
     unsigned int repcount;
{
DEBUG("|elt_rep(%d)|",repcount);
}

void
annotate_elt_rep_end ()
{
DEBUG("|elt_rep_end|");
}

void
annotate_elt ()
{
DEBUG("|elt|");
}

void
annotate_array_section_end ()
{
DEBUG("|array_section_end|");
}

void
annotate_display_begin ()
{
DEBUG("|display_begin|");
}

void
annotate_display_number_end ()
{
DEBUG("|display_number_end|");
}

void
annotate_display_format ()
{
DEBUG("|display_format|");
}

void
annotate_display_expression ()
{
DEBUG("|display_expression|");
}

void
annotate_display_expression_end ()
{
DEBUG("|display_expression_end|");
}

void
annotate_display_value ()
{
DEBUG("|display_value|");
}

void
annotate_display_end ()
{
DEBUG("|display_end|");
}

void
_initialize_annotate ()
{
}

