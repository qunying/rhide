/* Copyright (C) 1996-2003 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_MsgBox
#include "rhide.h"

#define Uses_TDependency
#define Uses_TDepCollection
#define Uses_TProject
#define Uses_TOptions
#define Uses_ideFunctions
#define Uses_ideEnums
#include <libide.h>

#define Uses_TMsgCollection
#define Uses_MsgRec
#define Uses_TSCollection
#define Uses_TDirList
#define Uses_tvutilFunctions
#define Uses_TMsgListBox
#include <libtvuti.h>

#include <rhutils.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int already_maked = 0;

static char *DEPFILE_NAME = NULL;
static char DEP_ENV[PATH_MAX];

#ifndef __DJGPP__
/* DJGPP allocates the memory for the string when doing
   putenv, but at least the putenv from the standard
   linux libc does not so :-( */
static char DEP_ENV_USER[PATH_MAX];
#else
#define DEP_ENV_USER DEP_ENV
#endif

#define SET_ALL_DEP() \
do {\
  strcpy(DEP_ENV,"SUNPRO_DEPENDENCIES=");\
  strcat(DEP_ENV,DEPFILE_NAME);\
  putenv(DEP_ENV);\
} while (0)

#define SET_USER_DEP() \
do {\
  strcpy(DEP_ENV_USER,"DEPENDENCIES_OUTPUT=");\
  strcat(DEP_ENV_USER,DEPFILE_NAME);\
  putenv(DEP_ENV_USER);\
} while (0)

#define DEL_ALL_DEP() \
do {\
  putenv("SUNPRO_DEPENDENCIES");\
} while (0)

#define DEL_USER_DEP() \
do {\
  putenv("DEPENDENCIES_OUTPUT");\
} while (0)

#define SET_DEPFILE_NAME() \
do { \
  if (DEPFILE_NAME) string_free(DEPFILE_NAME); \
  DEPFILE_NAME = unique_name("de"); \
} while (0)

#define SET_DEP() \
do {\
  if (OnlyUserIncludes)\
    SET_USER_DEP();\
  else\
    SET_ALL_DEP();\
} while (0)

#define DEL_DEP() \
do {\
  if (OnlyUserIncludes)\
    DEL_USER_DEP();\
  else\
    DEL_ALL_DEP();\
} while (0)

static char *buffer = NULL;
static int bufsize = 0, bufpos;
static FILE *errfile;

char *cpp_outname;
char *cpp_errname;

static bool
open_errfile(const char *errname)
{
  errfile = fopen(errname, "rt");
  if (!errfile)
    return false;
  return true;
}

static void
close_errfile()
{
  fclose(errfile);
}

/*
  Reads the next line from the error file and returns
  the length of the line or -1 for EOF
*/
static int
next_error_line()
{
  int c;

  bufpos = 0;
  while ((c = fgetc(errfile)) != EOF && c != '\n')
  {
    if (bufpos == bufsize)
    {
      bufsize += 512;
      buffer = (char *) realloc(buffer, bufsize);
    }
    buffer[bufpos++] = c;
  }
  if (bufpos == bufsize)
  {
    bufsize += 512;
    buffer = (char *) realloc(buffer, bufsize);
  }
  buffer[bufpos] = 0;
  if (c == EOF && bufpos == 0)
    return -1;
  return bufpos;
}

static Boolean
check_user_messages(TMsgCollection & errs)
{
  int is_first;

  if (open_errfile(cpp_outname))
  {
    is_first = 1;
    while (next_error_line() >= 0)
    {
      if (is_first)
        errs.insert(new MsgRec(NULL, _("Messages to stdout:")));
      errs.insert(new MsgRec(NULL, buffer));
      is_first = 0;
    }
    close_errfile();
  }
  if (open_errfile(cpp_errname))
  {
    is_first = 1;
    while (next_error_line() >= 0)
    {
      if (is_first)
        errs.insert(new MsgRec(NULL, _("Messages to stderr:")));
      errs.insert(new MsgRec(NULL, buffer));
      is_first = 0;
    }
    close_errfile();
  }
  return True;
}


#define ASSEMBLER_MESSAGES "Assembler messages:"

static Boolean
check_compile_c_errors(TMsgCollection & errs)
{
  Boolean retval = True;
  static char _buffer[512];
  char *tmp, *fname, *temp;
  int lineno;
  msgType error;

  if (open_errfile(cpp_errname) == False)
    return False;
/* Scan the file for messages. Each message is of the form:
   filename:[lineno:] [Warning] [message]
   or
   In file included from FILE:LINE[,:]
   or
                    from FILE:LINE[,:]
   Any other message is not converted (except Control-Break Pressed)
*/
  while (next_error_line() >= 0)
  {
    error = msgError;
    if (strncmp(buffer, "Control-Break Pressed", 21) == 0)
    {
      errs.insert(new MsgRec(NULL, _("Control-Break Pressed"), msgError));
      retval = False;
      continue;
    }
    if (buffer[0] && buffer[1] == ':')
    /*
      filename starts with a drive letter
    */
    {
      tmp = buffer + 2;
    }
    else
    {
      tmp = buffer;
    }
    /*
       The special case "In file included from FILE:LINE" or
       "                 from FILE:LINE" 
     */
    if (strncmp(buffer, "In file included from ", 22) == 0 ||
        strncmp(buffer, "                 from ", 22) == 0 ||
        strncmp(buffer, "In instantiation of ", 20) == 0 ||
        strncmp(buffer, "  instantiated from ", 20) == 0 ||
        strncmp(buffer, "  instantiated from here", 24) == 0)
    {
      char *last, _last, *l;

      fname = buffer + 22;
      last = buffer + strlen(buffer) - 1;
      _last = *last;
      *last = 0;
      l = strrchr(buffer, ':'); // the ':' before the line
      *l = 0;
      sscanf(l + 1, "%d", &lineno);
      strcpy(_buffer, fname);
      fname = _buffer;
      *l = ':';
      *last = _last;
      temp = buffer;
      error = msgMessage;
    }
    else
    {
      if (buffer[0] != ' ')
        tmp = strchr(tmp, ':');
      else
        tmp = NULL;
      if (tmp)
      {
        temp = strchr(buffer, ' ');
        if (temp && temp < tmp)
        {
          temp = buffer;
          fname = NULL;
          lineno = 0;
        }
        else
        {
          *tmp = 0;
          fname = buffer;
          tmp++;
          temp = tmp;
          while (rh_isdigit(*tmp))
            tmp++;
          if (*tmp == ':')
          {
            *tmp = 0;
            sscanf(temp, "%d", &lineno);
            temp = tmp + 1;
          }
          else
          {
            lineno = 0;
          }
        }
      }
      else
      {
        temp = buffer;
        fname = NULL;
        lineno = 0;
      }
    }
    /*
       Now points fname to the filename or NULL and lineno is set correct.
       temp points to the message, if there is one 
     */
    if (fname)
    {
#if 0
      char *bname, *full_name;

      BaseName(fname, bname);
      if (FindFile(bname, full_name) == True)
      {
        strcpy(_buffer, bname);
        fname = _buffer;
      }
      string_free(bname);
      string_free(full_name);
#endif
    }
    else
    {
      /*
         if no filename was found, make all to a normal message 
       */
      char *tmp = buffer;

      while (*tmp == ' ' || *tmp == '\t')
        tmp++;
      if (!*tmp)
        continue;               // empty message
      errs.insert(new MsgRec(NULL, buffer, msgMessage));
      continue;
    }
    /*
       error == msgError I should check it if it is a warning 
     */
    if (error == msgError)
    {
      /*
         Is this message a warning? 
       */
      /*
         GCC says "warning:" here, but GAS - "Warning:". Therefore
         compare ignoring symbol case  
       */
      while (*temp && *temp == ' ')
        temp++;
      if (strncasecmp(temp, "warning:", 8) == 0)
      {
        error = msgWarning;
        temp += 8;
        /*
           skip whitespaces 
         */
        while (*temp && *temp == ' ')
          temp++;
        if (*temp)
          temp--;
      }
      /*
         We should also test for template instantiation messages
         and assembler messages title line  
       */
      else if (strncmp(temp, ASSEMBLER_MESSAGES, sizeof(ASSEMBLER_MESSAGES))
               == 0)
      {
        error = msgMessage;
        while (*--temp == ' ');
        temp++;
        temp++;
      }
      else if (strncmp(temp, "instantiated from here", 22) == 0)
      {
        error = msgMessage;
        while (*--temp == ' ');
        temp++;
        *temp = ':';
      }
    }
    /*
       if there was no lineno found, assume it is only a message, not an error 
     */
    if (lineno == 0 && error == msgError)
      error = msgMessage;
    errs.insert(new MsgRec(fname, temp, error, lineno));
    if (error == msgError)
      retval = False;
  }
  close_errfile();
  return retval;
}

static Boolean
check_compile_pascal_errors(TMsgCollection & errs)
{
  return check_compile_c_errors(errs);
}

static bool
check_compile_fpc_errors_(TMsgCollection & errs, const char *_fname)
{
  bool retval = true;
  static char _buffer[512];
  char *tmp, *fname, *temp;
  int lineno;
  msgType error;

  if (open_errfile(_fname) == false)
    return false;
/* Scan the file for messages. Each message is of the form:
   filename:[lineno:] [Warning] [message]
   or
   In file included from FILE:LINE[,:]
   or
                    from FILE:LINE[,:]
   or filename(LINE) TYPE: message
   or filename(LINE,COLUMN) TYPE: message
   
   Any other message is not converted (except Control-Break Pressed)
   and ignored.
*/
  while (next_error_line() >= 0)
  {
    int column = 1;
    error = msgError;
    if (strncmp(buffer, "Control-Break Pressed", 21) == 0)
    {
      errs.insert(new MsgRec(NULL, _("Control-Break Pressed"), msgError));
      retval = false;
      continue;
    }
    if (buffer[0] && buffer[1] == ':')
    /*
      filename starts with a drive letter
    */
    {
      tmp = buffer + 2;
    }
    else
    {
      tmp = buffer;
    }
    /*
       The special case "In file included from FILE:LINE" or
       "                 from FILE:LINE" 
     */
    if (strncmp(buffer, "In file included from ", 22) == 0 ||
        strncmp(buffer, "                 from ", 22) == 0)
    {
      char *last, _last, *l;

      fname = buffer + 22;
      last = buffer + strlen(buffer) - 1;
      _last = *last;
      *last = 0;
      l = strrchr(buffer, ':'); // the ':' before the line
      *l = 0;
      sscanf(l + 1, "%d", &lineno);
      strcpy(_buffer, fname);
      fname = _buffer;
      *l = ':';
      *last = _last;
      temp = buffer;
      error = msgMessage;
    }
    else
    {
      char *brace = NULL;
      char *colon = NULL;
      if (buffer[0] != ' ')
        brace = strchr(tmp, '(');
      if (buffer[0] != ' ')
        colon = strchr(tmp, ':');
      if (brace != NULL)
        tmp = brace;
      else if (colon != NULL)
        tmp = colon;
      else
        tmp = NULL;
      if (tmp)
      {
        temp = strchr(buffer, ' ');
        if (temp && temp < tmp)
        {
          temp = buffer;
          fname = NULL;
          lineno = 0;
        }
        else
        {
          *tmp = 0;
          fname = buffer;
          tmp++;
          temp = tmp;
          while (rh_isdigit(*tmp))
            tmp++;
          if ((*tmp == ':') || (*tmp == ',') || (*tmp == ')' && brace != NULL))
          {
            *tmp = 0;
            sscanf(temp, "%d", &lineno);
            temp = tmp + 1;
          }
          else
          {
            lineno = 0;
          }
          if (brace != NULL)
          {
            tmp = temp;
            while (rh_isdigit(*tmp))
              tmp++;
            if (*tmp == ')')
            {
              *tmp = 0;
              sscanf(temp, "%d", &column);
              temp = tmp+1;
            }
          }
        }
      }
      else
      {
        temp = buffer;
        fname = NULL;
        lineno = 0;
      }
    }
    /*
       Now points fname to the filename or NULL and lineno is set correct.
       temp points to the message, if there is one 
     */
    if (!fname)
    {
#if 0
      /*
         if no filename was found, make all to a normal message 
       */
      char *tmp = buffer;

      while (*tmp == ' ' || *tmp == '\t')
        tmp++;
      if (!*tmp)
        continue;               // empty message
      errs.insert(new MsgRec(NULL, buffer, msgMessage));
#endif
      continue;
    }
    /*
       error == msgError I should check it if it is a warning 
     */
    if (error == msgError)
    {
      /*
         Is this message a warning? 
       */
      while (*temp && *temp == ' ')
        temp++;
      if (strncmp(temp, "warning:", 8) == 0)
      {
        error = msgWarning;
        temp += 8;
      }
      else if (strncasecmp(temp, "error:", 6) == 0)
      {
        error = msgError;
        temp += 6;
      }
      else if (strncmp(temp, "Fatal:", 6) == 0)
      {
        error = msgError;
        temp += 6;
      }
      /*
         skip whitespaces 
       */
      while (*temp && *temp == ' ')
        temp++;
      tmp = temp;
      while (rh_isdigit(*tmp))
        tmp++;
      if (*tmp == ':')
      {
        *tmp = 0;
        sscanf(temp, "%d", &column);
        if (lineno == 0)
        {
          lineno = column;
          column = 1;
        }
        temp = tmp+1;
      }
//      if (*temp)
//        temp--;
    }
    /*
       if there was no lineno found, assume it is only a message, not an error 
     */
    if (lineno == 0 && error == msgError)
      error = msgMessage;
    errs.insert(new MsgRec(fname, temp, error, lineno, column));
    if (error == msgError)
      retval = false;
  }
  close_errfile();
  return retval;
}


static bool
check_compile_fpc_errors(TMsgCollection & errs)
{
  bool ret = check_compile_fpc_errors_(errs, cpp_errname);
  ret &= check_compile_fpc_errors_(errs, cpp_outname);
  return ret;
}


static bool
check_link_errors(TMsgCollection & errs)
{
  bool retval = true;
  bool last_maybe_error = false;
  if (open_errfile(cpp_errname) == false)
    return false;
  while (next_error_line() >= 0)
  {
    /*
      the linker produces suche message for functions, which
      should not be used for instance on GNU/Linux. The
      messages are stored in the object files in the
      gnu.warning section and do not include the warning
      string :-(((
    */
    if (strstr(buffer, "the use of `") != NULL)
    {
      /*
        there was already a line parsed out to be an error
        message, replace it now to be a warning.
      */
      if (last_maybe_error)
      {
        int last_err = errs.getCount();
        ((MsgRec *)errs.at(last_err-1))->type = msgWarning;
        errs.insert(new MsgRec(NULL, buffer, msgWarning));
        last_maybe_error = false;
        continue;
      }
    }
    retval &= (!last_maybe_error);
    last_maybe_error = false;
    char *tmp = strchr(buffer, ' ');

    if (tmp && tmp[-1] == ':')
    {
      char *temp;

      tmp--;
      temp = tmp - 1;
      while (rh_isdigit(*temp))
        temp--;
      if (*temp != ':')
      {
        if (strstr(tmp, ": warning: ") != NULL)
          errs.insert(new MsgRec(NULL, buffer, msgWarning));
        else
        {
          errs.insert(new MsgRec(NULL, buffer, msgError));
          last_maybe_error = true;
        }
      }
      else
      {
        *tmp = 0;
        int line;
        char *msg;

        sscanf(temp + 1, "%d", &line);
        *tmp = ':';
        msg = tmp + 2;
        *temp = 0;
        temp--;
        /*
           I scan back for the filename, because when using stabs debugging,
           the linker puts out a line like:
           o:/rhide/s:rhide/idemain.cc:345: ...
         */
        while (temp > buffer && *temp != ':')
          temp--;
        if (*temp == ':')
          temp--;
        char *fname, *full_name;

        BaseName(temp, fname);
        if (FindFile(fname, full_name) == False)
        {
          string_free(fname);
          string_dup(fname, temp);
        }
        string_free(full_name);
        errs.insert(new MsgRec(fname, msg, msgError, line));
        string_free(fname);
        last_maybe_error = true;
      }
    }
    else
    {
      errs.insert(new MsgRec(NULL, buffer));
    }
  }
  close_errfile();
  return retval;
}

static Boolean
check_ar_errors(TMsgCollection & errs)
{
  Boolean retval = True;

  if (open_errfile(cpp_errname) == False)
    return False;
  while (next_error_line() >= 0)
  {
    // treat any line as an error message, except the message
    // "...: creating ..." (after checking the sources for
    // ar this is the only message printed to stderr which
    // is not an error)
    if (strstr(buffer, ": creating ") != NULL)
    {
      errs.insert(new MsgRec(NULL, buffer, msgMessage));
    }
    else
    {
      /*
         If the line contains a `warning` somewhere, treat it
         not as an error. This can happen for instance, if the
         user has overwritten the spec for the archive to run
         the linker.
       */
      if (strstr(buffer, "warning:") == NULL)
      {
        errs.insert(new MsgRec(NULL, buffer, msgError));
        retval = False;
      }
      else
        errs.insert(new MsgRec(NULL, buffer, msgMessage));
    }
  }
  close_errfile();
  return retval;
}

static int
RunCompiler(char *cmd, const char *source_name, const char *dest_name)
{
  TMsgCollection *errs = new TMsgCollection();
  char *tmp, *dname;

  if (source_name)
  {
    string_dup(tmp, _("Compiling: "));
    string_cat(tmp, source_name);
  }
  else
  {
    string_dup(tmp, _("Creating: "));
    string_cat(tmp, dest_name);
  }
  errs->insert(new MsgRec(NULL, tmp));
  string_free(tmp);
  ShowMessages(errs, False);
  FindFile(dest_name, dname);
  remove(dname);
  int run_ret = RunProgram(cmd, True, True);

  if (run_ret != 0)
    remove(dname);
  TimeOfFile(dname, True);      // update the file-time-table
  string_free(dname);
  return run_ret;
}

Boolean user_check_errors(TDependency * dep, TMsgCollection & args);
static void check_c_deps(TDependency *);

static int
CompileDep(TDependency * dep, char *spec_name)
{
  char *compiler = BuildCompiler(dep, spec_name);
  int run_ret =

    RunCompiler(compiler, FName(dep->source_name), FName(dep->dest_name));
  string_free(compiler);
  return run_ret;
}

static Boolean
compile_user(TDependency * dep, char *spec)
{
  int run_ret;
  Boolean retval;

  if (!dep)
    return False;
  ERROR_TYPE error = dep->error_type;

  if (error == ERROR_BUILTIN_C)
  {
    SET_DEPFILE_NAME();
    SET_DEP();
  }
  run_ret = CompileDep(dep, spec);
  if (error == ERROR_BUILTIN_C)
  {
    DEL_DEP();
    check_c_deps(dep);
  }
  TMsgCollection *errs = new TMsgCollection();

  switch (error)
  {
    case ERROR_AUTO:
      switch (how_to_compile(dep->source_file_type, dep->dest_file_type))
      {
        case COMPILE_C:
        case COMPILE_C_LIKE:
        case COMPILE_CC:
        case COMPILE_CC_LIKE:
        case COMPILE_OBJC:
        case COMPILE_PASCAL:
        case COMPILE_FORTRAN:
        case COMPILE_NASM:
          error = ERROR_BUILTIN_C;
          break;
        case COMPILE_ASM:
          error = ERROR_BUILTIN_ASM;
          break;
        case COMPILE_FPC:
        case COMPILE_LINK_FPC_AUTOMAKE:
          error = ERROR_BUILTIN_FPC;
          break;
        case COMPILE_LINK:
          error = ERROR_BUILTIN_LINK;
          break;
        case COMPILE_LINK_DLL:
          error = ERROR_BUILTIN_LINK;
          break;
        default:
          error = ERROR_RETVAL;
          break;
      }
      break;
    default:
      break;
  }
  switch (error)
  {
    case ERROR_BUILTIN_FPC:
      retval = check_compile_fpc_errors(*errs);
      break;
    case ERROR_USER:
      retval = user_check_errors(dep, *errs);
      break;
    case ERROR_BUILTIN_C:
      retval = check_compile_c_errors(*errs);
      break;
    case ERROR_BUILTIN_LINK:
      retval = check_link_errors(*errs);
      break;
    default:
      check_user_messages(*errs);
      retval = True;
      break;
  }
  ShowMessages(errs, False);
  if (!debug_tempfiles)
  {
    remove(cpp_errname);
    remove(cpp_outname);
  }
  if (run_ret != 0)
    retval = False;
  return retval;
}

static Boolean
compile_s_to_obj(TDependency * dep, char *spec)
{
  int run_ret = CompileDep(dep, spec);
  TMsgCollection *errs = new TMsgCollection();
  Boolean retval = check_compile_c_errors(*errs);

  ShowMessages(errs, False);
  if (!debug_tempfiles)
  {
    remove(cpp_errname);
    remove(cpp_outname);
  }
  if (run_ret != 0)
    retval = False;
  return retval;
}

static Boolean
compile_link(TDependency * dep, char *spec)
{
  int run_ret = CompileDep(dep, spec);
  TMsgCollection *errs = new TMsgCollection();
  Boolean retval;

  if (dep->compile_id == COMPILE_LINK_PASCAL_AUTOMAKE)
  {
    if (UseFPC)
      retval = check_compile_fpc_errors(*errs);
    else
      retval = check_compile_pascal_errors(*errs);
  }
  else if (dep->compile_id == COMPILE_LINK_FPC_AUTOMAKE)
    retval = check_compile_fpc_errors(*errs);
  else
    retval = check_link_errors(*errs);
  if (!debug_tempfiles)
  {
    remove(cpp_errname);
    remove(cpp_outname);
  }
  ShowMessages(errs, False);
  if (run_ret != 0)
    retval = False;
  return retval;
}

static Boolean
compile_archive(TDependency * dep, char *spec)
{
  int run_ret = CompileDep(dep, spec);
  TMsgCollection *errs = new TMsgCollection();
  Boolean retval = check_ar_errors(*errs);

  if (!debug_tempfiles)
  {
    remove(cpp_errname);
    remove(cpp_outname);
  }
  ShowMessages(errs, False);
  if (run_ret != 0)
    retval = False;
  return retval;
}

static int
isStandardHeader(const char *depfile)
{
  int i, count = Options.StdInc->getCount();

  for (i = 0; i < count; i++)
  {
    char *dir = expand_rhide_spec((char *) Options.StdInc->at(i));

    if (!*dir)
      continue;
    char *tok, *res = NULL;

    /*
       dir may be more than one directory 
     */
    /*
       it fails of course, if the directoy has spaces as part of the name :-( 
     */
    for (tok = strtok(dir, " "); tok; tok = strtok(NULL, " "))
    {
      res = strstr(depfile, tok);
      if (res == depfile)
        break;
    }
    string_free(dir);
    if (res == depfile)
      return 1;
  }
  return 0;
}

static void
check_c_deps(TDependency * dep)
{
  FILE *f;
  char x[256], depfile[PATH_MAX], *temp;

  if (dep->dependencies)
    destroy(dep->dependencies);
  dep->dependencies = NULL;
  f = fopen(DEPFILE_NAME, "r");
  if (f)
  {
    char *tmp;

    fgets(x, 255, f);
    temp = strchr(x, ':');
    if (temp)
    {
      temp++;
      while (*temp == ' ')
        temp++;
      if (OnlyUserIncludes)
        temp = strchr(temp, ' ');
      do
      {
        while (temp)
        {
          while (*temp == ' ')
            temp++;
          tmp = temp;
          if (*temp == '\n' || *temp == '\\' && temp[1] == '\n')
            temp = NULL;
          else
          {
            char c;
            TDependency *tmp_dep;

            while (*temp != ' ' && *temp != '\n')
              temp++;
            c = *temp;
            *temp = 0;
            strcpy(depfile, tmp);
            *temp = c;
            char *_depfile = string_dup(depfile);

            FExpand(_depfile);
            if (!isStandardHeader(depfile) && !isStandardHeader(_depfile))
            {
              tmp_dep = new TDependency();
              InitFName(tmp_dep->dest_name, _depfile);
              tmp_dep->source_name = NULL;
              tmp_dep->source_file_type = FILE_UNKNOWN;
              tmp_dep->dest_file_type = FILE_HEADER;
              tmp_dep->compile_id = COMPILE_NONE;
              if (!dep->dependencies)
                dep->dependencies = new TDepCollection(7, 8);
              dep->dependencies->insert(tmp_dep);
            }
            string_free(_depfile);
          }
        }
      }
      while ((temp = fgets(x, 255, f)));
    }
    fclose(f);
  }
}

static Boolean
compile_c_to_obj(TDependency * dep, char *spec)
{
  SET_DEPFILE_NAME();
  SET_DEP();
  int run_ret = CompileDep(dep, spec);

  DEL_DEP();
  check_c_deps(dep);
  TMsgCollection *errs = new TMsgCollection();
  Boolean retval = check_compile_c_errors(*errs);

  ShowMessages(errs, False);
  if (!debug_tempfiles)
  {
    remove(DEPFILE_NAME);
    remove(cpp_errname);
    remove(cpp_outname);
  }
  if (run_ret != 0)
    retval = False;
  return retval;
}

static Boolean
check_compile_ada_errors(TMsgCollection & errs)
{
  int is_first;

  if (open_errfile(cpp_outname))
  {
    is_first = 1;
    while (next_error_line() >= 0)
    {
      if (is_first)
        errs.insert(new MsgRec(NULL, _("Messages to stdout:")));
      errs.insert(new MsgRec(NULL, buffer));
      is_first = 0;
    }
    close_errfile();
  }
  if (open_errfile(cpp_errname))
  {
    while (next_error_line() >= 0)
    {
      int handled = 0;
      char *_line = strchr(buffer, ':');

      if (_line)
      {
        char *_column = strchr(_line + 1, ':');

        if (_column)
        {
          int line, column;
          char *msg = strchr(_column + 1, ':');

          if (msg)
          {
            msgType error = msgError;

            msg++;
            while (*msg == ' ' || *msg == '\t')
              msg++;
            *_line = 0;
            sscanf(_line + 1, "%d:%d", &line, &column);
            if (strncmp(msg, "warning: ", 10) == 0)
            {
              msg += 10;
              while (*msg == ' ' || *msg == '\t')
                msg++;
              error = msgWarning;
            }
            errs.insert(new MsgRec(buffer, msg, error, line, column));
            handled = 1;
          }
        }
      }
      if (!handled)
        errs.insert(new MsgRec(NULL, buffer));
    }
    close_errfile();
  }
  return True;
}

static Boolean
compile_ada_to_obj(TDependency * dep, char *spec)
{
  int run_ret = CompileDep(dep, spec);
  char *ali_name;

  BaseName(FName(dep->dest_name), ali_name, 0);
  string_cat(ali_name, ".ali");
  if (open_errfile(ali_name) == True)
  {
    if (dep->dependencies)
      destroy(dep->dependencies);
    dep->dependencies = NULL;
    while (next_error_line() >= 0)
    {
      if (strncmp(buffer, "D ", 2) == 0)
      {
        char *space = strchr(buffer + 2, ' ');

        if (!space)
          space = strchr(buffer + 2, '\t');
        if (space)
        {
          *space = 0;
          char *depfile;

          FindFile(buffer + 2, depfile);
          TDependency *tmp_dep = new TDependency();

          InitFName(tmp_dep->dest_name, depfile);
          tmp_dep->source_name = NULL;
          tmp_dep->source_file_type = FILE_UNKNOWN;
          tmp_dep->dest_file_type = get_file_type(depfile);
          tmp_dep->compile_id = COMPILE_NONE;
          if (!dep->dependencies)
            dep->dependencies = new TDepCollection(7, 8);
          dep->dependencies->insert(tmp_dep);
        }
      }
    }
    close_errfile();
  }
  string_free(ali_name);
  TMsgCollection *errs = new TMsgCollection();
  Boolean retval = check_compile_ada_errors(*errs);

  ShowMessages(errs, False);
  if (!debug_tempfiles)
  {
    remove(cpp_errname);
    remove(cpp_outname);
  }
  if (run_ret != 0)
    retval = False;
  return retval;
}

void
CreateDependencies()
{
  char *spec = expand_rhide_spec("gcc -v -E -o /dev/null $(strip \
$(RHIDE_INCLUDES) \
$(foreach item,$(PROJECT_ITEMS),\
$(subst $(RHIDE_INCLUDES),,\
$(subst gcc,,$(subst -o $(outfile $(item)),,$(compilespec $(item)))))))");

  fprintf(stderr, "%s\n", spec);
  SET_DEPFILE_NAME();
  SET_DEP();
  RunProgram(spec, True, True);
  DEL_DEP();
  string_free(spec);
}

static Boolean
compile_cc_to_obj(TDependency * dep, char *spec)
{
  return compile_c_to_obj(dep, spec);
}

static Boolean
compile_pascal_to_obj(TDependency * dep, char *spec)
{
  return compile_c_to_obj(dep, spec);
}

/* must be changed */
static Boolean
compile_fpc_to_obj(TDependency * dep, char *spec)
{
  int run_ret = CompileDep(dep, spec);

  TMsgCollection *errs = new TMsgCollection();
  Boolean retval = check_compile_fpc_errors(*errs);

  ShowMessages(errs, False);
  if (run_ret != 0)
    retval = False;
  return retval;
}

static Boolean
compile_objc_to_obj(TDependency * dep, char *spec)
{
  return compile_c_to_obj(dep, spec);
}

static Boolean
compile_f_to_obj(TDependency * dep, char *spec)
{
  return compile_c_to_obj(dep, spec);
}

static Boolean
compile_nsm_to_obj(TDependency * dep, char *spec)
{
  return compile_c_to_obj(dep, spec);
}

Boolean
compile_dep(TDependency * dep)
{
  Boolean retval;
  int old_msg_count = -1;

  if (msg_list && msg_list->range > 0)
    old_msg_count = msg_list->list()->getCount();
  char *sname = NULL, *dname = NULL;

  if (dep->compile_id == COMPILE_NONE)
    return True;
  Boolean user_spec;
  Boolean was_found = True, is_rcs_file = False;
  char *spec = GetCompilerSpec(dep, user_spec);

  if (!spec || dep->compile_id == COMPILE_UNKNOWN && user_spec == False)
  {
    BigmessageBox(mfError | mfOKButton,
                  _("Don't know how to build %s from %s"),
                  FName(dep->dest_name), FName(dep->source_name));
    return False;
  }
  if (UseRCS)
  {
    if (dep->source_name
        && (was_found = FindFile(FName(dep->source_name), sname)) == False)
    {
      char *rcs_name = NULL;

      string_free(sname);
      is_rcs_file = was_found =
        FindRCSFile(FName(dep->source_name), rcs_name, sname);
      if (was_found)
      {
        CheckoutRCSFile(rcs_name, sname, 0);
        string_free(rcs_name);
      }
    }
  }
  else
  {
    if (dep->source_name)
      was_found = FindFile(FName(dep->source_name), sname);
  }
  if (dep->source_name && was_found == False)
  {
    BigmessageBox(mfError | mfOKButton, _("Could not find the source file "
                                          "'%s'. Make sure, that the file "
                                          "exist or check the settings in "
                                          "'Options/Directories' for the "
                                          "correct paths."),
                  FName(dep->source_name));
    string_free(sname);
    return False;
  }
  FindFile(FName(dep->dest_name), dname);
  already_maked = 0;
  if (DEBUGGER_STARTED())
    RESET();
  CLEARSYMBOLS();
  if (user_spec == True)
  {
    retval = compile_user(dep, spec);
  }
  else
  {
    switch (dep->compile_id)
    {
      case COMPILE_ADA:
        retval = compile_ada_to_obj(dep, spec);
        break;
      case COMPILE_ASM:
        retval = compile_s_to_obj(dep, spec);
        break;
      case COMPILE_PASCAL:
        retval = compile_pascal_to_obj(dep, spec);
        break;
      case COMPILE_FPC:
        retval = compile_fpc_to_obj(dep, spec);
        break;
      case COMPILE_C:
      case COMPILE_C_LIKE:
        retval = compile_c_to_obj(dep, spec);
        break;
      case COMPILE_CC:
      case COMPILE_CC_LIKE:
        retval = compile_cc_to_obj(dep, spec);
        break;
      case COMPILE_OBJC:
        retval = compile_objc_to_obj(dep, spec);
        break;
      case COMPILE_NASM:
        retval = compile_nsm_to_obj(dep, spec);
        break;
      case COMPILE_FORTRAN:
        retval = compile_f_to_obj(dep, spec);
        break;
      case COMPILE_LINK:
      case COMPILE_LINK_PASCAL_AUTOMAKE:
      case COMPILE_LINK_FPC_AUTOMAKE:
      case COMPILE_LINK_DLL:
        retval = compile_link(dep, spec);
        break;
      case COMPILE_ARCHIVE:
        retval = compile_archive(dep, spec);
        break;
      default:
        if (spec)
          retval = compile_user(dep, spec);
        else
          retval = False;
        break;
    }
  }
  if (retval == True)
  {
    TMsgCollection *errs = new TMsgCollection();;
    errs->insert((new MsgRec(NULL, _("no errors"))));
    ShowMessages(errs, False);
    if (PRJSTACKCOUNT == 0 &&
        (dep->compile_id == COMPILE_ARCHIVE ||
         dep->compile_id == COMPILE_LINK ||
	 dep->compile_id == COMPILE_LINK_DLL))
    {
      already_maked = 1;
    }
  }
  else
  {
    TMsgCollection *errs = new TMsgCollection();;
    errs->insert((new MsgRec(NULL, _("There were some errors"))));
    ShowMessages(errs, False, old_msg_count);
    remove(dname);
  }
  if (UseRCS && is_rcs_file)
  {
    remove(sname);
  }
  string_free(sname);
  string_free(dname);
  string_free(spec);
  return retval;
}

