/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TStreamable
#define Uses_TScreen
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TApplication

#define Uses_TParamList
#include <libtvuti.h>

#include <rhutils.h>

#define Uses_TProject
#define Uses_TOptions
#define Uses_TDepCollection
#include <libide.h>

#include <string.h>

expand_function expand_filenames = NULL;

static ushort TProject_Version = 8;

TProject::TProject():TDependency()
{
  options = new TOptions();
  string_dup(defaultprojectmask, "*.[acfghimnops]*");
  dependencies = new TDepCollection(5, 5);
  screen_mode = 3;
  options2 = 0;
  options1 = 0;
  private_options = 0;
  info_files = new TParamList();
#ifdef __linux__
  info_files->insert(strdup("(libc)Function Index"));
#else
  info_files->insert(strdup("libc"));
#endif
  info_search_mode = INFO_MODE_FUZZY;
  info_search_options = 0;
  fuzzy_value = 600;
  string_dup(main_function, "main");
  max_closed_windows = 16;
}

TProject & TProject::operator = (const TProject & prj)
{
  *options = *prj.options;
  string_free(defaultprojectmask);
  string_dup(defaultprojectmask, prj.defaultprojectmask);
  screen_mode = prj.screen_mode;
  options2 = prj.options2;
  options1 = prj.options1;
  private_options = prj.private_options;
  *info_files = *prj.info_files;
  info_search_mode = prj.info_search_mode;
  info_search_options = prj.info_search_options;
  fuzzy_value = prj.fuzzy_value;
  string_free(main_function);
  string_dup(main_function, prj.main_function);
  max_closed_windows = prj.max_closed_windows;
  return *this;
}

TProject::~TProject()
{
  destroy(options);
  destroy(info_files);
  string_free(defaultprojectmask);
  string_free(main_function);
}

TStreamable *
TProject::build()
{
  return new TProject(streamableInit);
}

void *
TProject::read(ipstream & is)
{
  ushort version;

  is >> version;
  TDependency::read(is);
  is >> options;
  defaultprojectmask = is.readString();
  if (version >= 2)
  {
    is >> options2;
    if (version < 6)
      options2 = 0;
    is >> screen_mode;
  }
  else
  {
    options2 = 8;
    screen_mode = 3;
  }

  if (version > 2)
    is >> options1;
  else
    options1 = 0;

  if (version > 3)
  {
    is >> info_files;
    is >> info_search_mode;
    is >> info_search_options;
    is >> fuzzy_value;
  }
  else
  {
    info_files = new TParamList();
    info_files->insert(strdup("libc"));
    info_search_mode = INFO_MODE_FUZZY;
    info_search_options = 0;
    fuzzy_value = 600;
  }

  if (version < 5)
  {
    string_dup(main_function, "main");
  }
  else
  {
    main_function = is.readString();
  }

  if (version < 7)
  {
    max_closed_windows = 16;
  }
  else
  {
    is >> max_closed_windows;
  }

  if (version < 8)
  {
    private_options = 0;
  }
  else
  {
    is >> private_options;
  }

  if (pr.pr_VarFilenames && expand_filenames)
  {
    expand_filenames(this);
    pr.pr_VarFilenames = 0;
  }

  return this;
}

void
TProject::write(opstream & os)
{
  os << TProject_Version;
  TDependency::write(os);
  os << options;
  os.writeString(defaultprojectmask);
  os << options2;
  os << screen_mode;
  os << options1;
  os << info_files;
  os << info_search_mode;
  os << info_search_options;
  os << fuzzy_value;
  os.writeString(main_function);
  os << max_closed_windows;
  os << private_options;
}
