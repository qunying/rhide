/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/* This is an example, that gcc produces wrong line number information
   for code which is optimized out. (compile this with -O) */

int a = 1;
int b = 2;

int main()
{
  if (a == b) return 0;
  if ((a + 1) == b) return 1;
  return 0; /* The debugger should never come here, but it comes */
}
