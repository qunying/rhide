/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
/* Modified by RH, to call abort() instead of raise(SIGABRT) */
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void	__dj_assert(const char *msg, const char *file, int line)
{
  /* Assertion failed at foo.c line 45: x<y */
  fprintf(stderr, "Assertion failed at %s line %d: %s\n", file, line, msg);
#if 0
  raise(SIGABRT);
  exit(1);
#else
  abort();
#endif
}

