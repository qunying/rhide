/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RHU for details */
#include <rhutils.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

void
split_fname(const char *fname, char &drive, char *&dir,
            char *&name, char *&ext)
{
  const char *slash;
  const char *dot = NULL;
  const char *filename = fname;

  if (rh_isalpha(fname[0]) && fname[1] == ':')
  {
    drive = fname[0];
    filename = fname + 2;
  }
  else
    drive = 0;
  slash = strrchr(filename, '/');
  if (slash)
    dot = strrchr(slash, '.');
  else
    dot = strrchr(filename, '.');
  if (slash)
  {
    int dirlen = (slash - filename) + 1;

    dir = (char *) malloc(dirlen + 1);
    strncpy(dir, filename, dirlen);
    dir[dirlen] = 0;
  }
  else
    dir = string_dup("");
  if (dot)
  {
    if (!slash)
      slash = filename;
    else
      slash++;
    int namelen = (dot - slash);

    name = (char *) malloc(namelen + 1);
    strncpy(name, slash, namelen);
    name[namelen] = 0;
    ext = string_dup(dot);
  }
  else
  {
    ext = string_dup("");
    if (slash)
      slash++;
    else
      slash = filename;
    name = string_dup(slash);
  }
}

/*
  %D - drive + directory
  %d - directory
  %f - filename + suffix
  %F - filename without suffix
  %e - suffix
*/

void
split_fname_fmt(const char *filename, const char *fmt, ...)
{
  char drive, *dir, *name, *ext, c, *arg = NULL;
  va_list va;

  va_start(va, fmt);
  split_fname(filename, drive, dir, name, ext);
  if (*fmt != '%')
    goto end;
  while ((c = *fmt++))
  {
    switch (c)
    {
      case '%':
        arg = va_arg(va, char *);

        if (!arg)
          goto end;
        *arg = 0;
        break;
      case 'D':
        arg[0] = drive;
        arg[1] = 0;
        if (drive)
        {
          arg[1] = ':';
          arg[2] = 0;
        }
      case 'd':
        strcat(arg, dir);
        break;
      case 'F':
        strcat(arg, name);
        break;
      case 'f':
        strcat(arg, name);
      case 'e':
        strcat(arg, ext);
        break;
      default:
        break;
    }
  }
end:
  va_end(va);
  string_free(dir);
  string_free(name);
  string_free(ext);
}

void
split_fname(const char *fname, char *&dir, char *&name, char *&ext)
{
  char drive;

  split_fname(fname, drive, dir, name, ext);
  if (drive)
  {
    char dr[2] = { drive, 0 };
    char *_dir = string_dup(dr);

    string_cat(_dir, ":", dir, NULL);
    string_free(dir);
    dir = _dir;
  }
}
