/* Copyright (C) 1996 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#ifndef __DJGPP__

#include <unistd.h>
#include <string.h>
#include <limits.h>

/* 
   Here now the very usefull function _fixpath() from DJGPP's
   libc 'fixpath.c'
   I have modified it to be used on unix systems (like linux).
*/


__inline__ static int
is_slash(int c)
{
  return c == '/';
}

__inline__ static int
is_term(int c)
{
  return c == '/' || c == '\0';
}

/* Takes as input an arbitrary path.  Fixes up the path by:
   1. Removing consecutive slashes
   2. Removing trailing slashes
   3. Making the path absolute if it wasn't already
   4. Removing "." in the path
   5. Removing ".." entries in the path (and the directory above them)
 */
void
__fixpath(const char *in, char *out)
{
  const char *ip = in;
  char *op = out;

  /*
     Convert relative path to absolute 
   */
  if (!is_slash(*ip))
  {
    getcwd(op, PATH_MAX);
    op += strlen(op);
  }

  /*
     Step through the input path 
   */
  while (*ip)
  {
    /*
       Skip input slashes 
     */
    if (is_slash(*ip))
    {
      ip++;
      continue;
    }

    /*
       Skip "." and output nothing 
     */
    if (*ip == '.' && is_term(*(ip + 1)))
    {
      ip++;
      continue;
    }

    /*
       Skip ".." and remove previous output directory 
     */
    if (*ip == '.' && *(ip + 1) == '.' && is_term(*(ip + 2)))
    {
      ip += 2;
      /*
         Don't back up over root '/' 
       */
      if (op > out)
        /*
           This requires "/" to follow drive spec 
         */
        while (!is_slash(*--op));
      continue;
    }

    /*
       Copy path component from in to out 
     */
    *op++ = '/';
    while (!is_term(*ip))
      *op++ = *ip++;
  }

  /*
     If root directory, insert trailing slash 
   */
  if (op == out)
    *op++ = '/';

  /*
     Null terminate the output 
   */
  *op = '\0';

}

/*
  End of modified code from DJGPP's libc 'fixpath.c'
*/
#endif
