/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __DJGPP__
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crt0.h>
#include <sys/stat.h>

#define Uses_MsgBox
#include <tv.h>

#define Uses_tvutilFunctions
#include <libtvuti.h>

#include "rhide.h"
#include <rhutils.h>

int
CheckIDE()
{
  char *djgpp = expand_spec("$(DJGPP)", NULL);
  int djgpp_auto = 0;

  if (!*djgpp || !__file_exists(djgpp))
  {
    char *tmp =
      expand_spec("$(word 1,$(foreach file,$(addsuffix /../djgpp.env"
                  ",$(subst ;, ,$(PATH))),$(wildcard $(file))))", NULL);

    if (*tmp)
    {
      FExpand(tmp);
      string_free(djgpp);
      string_cat(djgpp, "DJGPP=", tmp, NULL);
      putenv(djgpp);
      djgpp = NULL;
      __crt0_load_environment_file("rhide");
      djgpp_auto = 1;
    }
    string_free(tmp);
  }
  string_free(djgpp);
  if (djgpp_auto)
  {
    BigmessageBox(mfWarning | mfOKButton,
                  _("RHIDE has detected that your DJGPP environment variable "
                    "was not set. It is set now automatically. If RHIDE did "
                    "the right choice, please modify your autoexec.bat by adding "
                    "the following line to fix this problem:"
                    "\n\nSET DJGPP=%s\n"), getenv("DJGPP"));
  }
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
    if (BigmessageBox(mfError | mfYesButton | mfNoButton,
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

  if (*language && (strncasecmp(language, "en", 2) != 0))
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
    if (BigmessageBox(mfWarning | mfYesButton | mfNoButton,
                      _("RHIDE could not access the direcory '%s', where "
                        "it searches for language specific strings. Please "
                        "read the file 'RHIDE.BIN' how to install RHIDE "
                        "correct. Should I continue?"), lcdir) == cmNo)
    {
      ret = 1;
      goto end;
    }
  }

  {
    char *tmp = string_dup(djdir);

    string_cat(tmp, "/lang/cxx/stdiostream.h");
    if (!__file_exists(tmp))
    {
      string_free(tmp);
      string_cat(tmp, djdir, "/lang/cxx/stdios~1.h", NULL);
      if (__file_exists(tmp))
      {
        if (BigmessageBox(mfWarning | mfYesButton | mfNoButton,
                          _
                          ("RHIDE has detected, that you are running on a System "
                           "where long filenames might be supported "
                           "and you have installed the C++ compiler. Are you "
                           "sure, you have read the DJGPP FAQ and the installing "
                           "instructions for gcc when running under such a system? "
                           "(like Windows 95/Windows NT) "
                           "There might be a problem with the standard C++ libraries and "
                           "the standard C++ include files because "
                           "of a LFN-conflict. Should I continue?")) == cmNo)
        {
          ret = 1;
          string_free(tmp);
          goto end;
        }
      }
      string_free(tmp);
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

int
CheckIDE()
{
  return 0;
}

#endif
