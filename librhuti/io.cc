/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RHU for details */
#include <rhutils.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STDOUT 1
#define STDERR 2

static char *errname = NULL;
static char *outname = NULL;
static int h_out, h_outbak;
static int h_err, h_errbak;

/* returns a malloced unique tempname in $TMPDIR */
char *
unique_name(char *before, char *retval)
{
  char *name, *tmp = getenv("TMPDIR");

  if (!tmp)
    tmp = ".";
  if (retval)
  {
    strcpy(retval, tmp);
    strcat(retval, "/");
    strcat(retval, before);
    strcat(retval, "XXXXXX");
    name = string_dup(retval);
  }
  else
  {
    string_dup(name, tmp);
    string_cat(name, "/");
    string_cat(name, before);
    string_cat(name, "XXXXXX");
  }
  int handle = mkstemp(name);
  close(handle);
  return name;
}

char *
open_stderr(void)
{
  if (errname)
    free(errname);
  errname = unique_name("er");
#ifdef O_BINARY
  h_err = open(errname, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC,
               S_IREAD | S_IWRITE);
#else
  h_err = open(errname, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
#endif
  h_errbak = dup(STDERR);
  fflush(stderr);               /*
                                   so any buffered chars will be written out 
                                 */
  dup2(h_err, STDERR);
  return errname;
}

void
close_stderr(void)
{
  dup2(h_errbak, STDERR);
  close(h_err);
  close(h_errbak);
}

char *
open_stdout(void)
{
  if (outname)
    free(outname);
  outname = unique_name("ou");
#ifdef O_BINARY
  h_out = open(outname, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC,
               S_IREAD | S_IWRITE);
#else
  h_out = open(outname, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
#endif
  h_outbak = dup(STDOUT);
  fflush(stdout);               /*
                                   so any buffered chars will be written out 
                                 */
  dup2(h_out, STDOUT);
  return outname;
}

void
close_stdout(void)
{
  dup2(h_outbak, STDOUT);
  close(h_out);
  close(h_outbak);
}
