/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#ifdef WIN31_HACK

int use_win31hak;

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
extern struct target_ops go32_ops;
int go32_insert_hw_breakpoint(CORE_ADDR,CORE_ADDR);
int go32_remove_hw_breakpoint(CORE_ADDR,CORE_ADDR);

static int _win31_memory_insert_breakpoint(CORE_ADDR addr, char *shadow)
{
  return go32_insert_hw_breakpoint(addr,0 /* this is not used */);
}

static int _win31_memory_remove_breakpoint(CORE_ADDR addr, char *shadow)
{
  return go32_remove_hw_breakpoint(addr,0 /* this is not used */);
}

static void __attribute__ ((__constructor__))
__win31_hack()
{
  __dpmi_regs r;
  r.x.ax = 0x160a;
  __dpmi_int(0x2f,&r);
  if (r.x.ax == 0 && r.h.bh == 3) win31 = 1;
  else win31 = 0;
  if (!win31) return;
  go32_ops.to_insert_breakpoint = _win31_memory_insert_breakpoint;
  go32_ops.to_remove_breakpoint = _win31_memory_remove_breakpoint;
}

#endif /* __DJGPP__ */

#endif


