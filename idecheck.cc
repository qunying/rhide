/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __DJGPP__
#include <stdlib.h>
#include <sys/stat.h>

#define Uses_MsgBox
#include <tv.h>

#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhide.h>
#include <rhutils.h>

int CheckIDE()
{
  char *djdir = expand_spec("$(DJDIR)", NULL);
  char *language = expand_spec("$(LANGUAGE)", NULL);
  char *localedir = expand_spec("$(LOCALEDIR)", NULL);
  char *lcdir = string_dup("");
  struct stat st;
  int ret = 0;
  int djdir_valid = 0;
  int lcdir_valid = 0;
  if (*djdir)
  {
    if (stat(djdir, &st) == 0)
    {
      if (S_ISDIR(st.st_mode))
        djdir_valid = 1;
    }
  }
  if (!djdir_valid)
  {
    if (BigmessageBox(mfError|mfYesButton|mfNoButton,
         _("RHIDE has detected, that the environment variable "
           "DJDIR has not been set (or not correct). This is a fatal mistake. "
           "For information about fixing this, please read the "
           "File README.1ST from the DJGPP distribution. "
           "(about setting the DJGPP environment variable!!!) "
           "Should I continue?")) == cmNo)
    {
      ret = 1;
      goto end;
    }
    goto end;
  }

  if (*language)
  {
    if (!*localedir)
      string_cat(localedir, djdir, "/share/locale", NULL);
    string_cat(lcdir, localedir, "/", language, "/LC_MESSAGES", NULL);
    if (stat(lcdir, &st) == 0)
    {
      if (S_ISDIR(st.st_mode))
        lcdir_valid = 1;
    }
  }
  else
    lcdir_valid = 1;

  if (!lcdir_valid)
  {
    if (BigmessageBox(mfWarning|mfYesButton|mfNoButton,
         _("RHIDE could not access the direcory '%s', where "
           "it searches for language specifc strings. Please "
           "read the file 'RHIDE.BIN' how to install RHIDE "
           "correct. Should I continue?"), lcdir) == cmNo)
    {
      ret = 1;
      goto end;
    }
  }
      
end:
  string_free(djdir);
  string_free(language);
  string_free(localedir);
  string_free(lcdir);
  return ret;
}

#else

int CheckIDE()
{
  return 0;
}

#endif

