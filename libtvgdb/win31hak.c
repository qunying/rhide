/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __DJGPP__

#include <libgdb.h>

/* Here comes the hack for Windows 3.1x.
   The case for setting breakpoints s handled in librhgdb by allowing
   only up to 3 hw breakpoints. But GDB uses for executing the step/next
   commands internal breakpoints, which are set by target_insert_breakpoint
   and deleteted by target_delete_breakpoint.
   Under DJGPP I can assume that we are using only the go32 target, so I
   overwrite the target ops to inserting/deleting every time a hw
   breakpoint.
*/

#include <dpmi.h>

extern int win31; /* in librhgdb */
static struct target_ops *go32_ops;
int go32_insert_hw_breakpoint(CORE_ADDR,CORE_ADDR);
int go32_remove_hw_breakpoint(CORE_ADDR,CORE_ADDR);

static int _win31_memory_insert_breakpoint(CORE_ADDR addr,
                                    char *shadow __attribute__((unused)))
{
  return go32_insert_hw_breakpoint(addr,0 /* this is not used */);
}

static int _win31_memory_remove_breakpoint(CORE_ADDR addr,
                                    char *shadow __attribute__((unused)))
{
  return go32_remove_hw_breakpoint(addr,0 /* this is not used */);
}

/* Pointer to array of target architecture structures; the size of the
   array; the current index into the array; the allocated size of the 
   array.  */
extern struct target_ops **target_structs;
extern unsigned target_struct_size;

void
__win31_hack()
{
  __dpmi_regs r;
  unsigned i;
  go32_ops = NULL;
  r.x.ax = 0x160a;
  __dpmi_int(0x2f,&r);
  if (r.x.ax == 0 && r.h.bh == 3) win31 = 1;
  else win31 = 0;
  if (!win31) return;
  for (i=0; i<target_struct_size; i++)
  {
    if ((target_structs[i]->to_shortname) &&
        (strcmp(target_structs[i]->to_shortname, "djgpp") == 0))
    {
      go32_ops = target_structs[i];
      break;
    }
  }
  if (!go32_ops)
    return;
  go32_ops->to_insert_breakpoint = _win31_memory_insert_breakpoint;
  go32_ops->to_remove_breakpoint = _win31_memory_remove_breakpoint;
}

#endif /* __DJGPP__ */


