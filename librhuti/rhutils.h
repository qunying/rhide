/* Copyright (C) 1996-2003 Robert Höhne, see COPYING.RHU for details */
#ifndef __rhutils_h
#define __rhutils_h

#ifndef __cplusplus
#error You can use these functions only in C++ programs!!
#endif

#include <cstdio>

/* return a malloced string 'src'. When scr=NULL return malloced "" */
char *string_dup(const char *src);

/* the same as above, but store the result also in 'dest' */
char *string_dup(char *&dest, const char *src);

/* convert 'str' to lowercase */
char *string_down(char *str);

/* convert 'str' to uppercase */
char *string_up(char *str);

/* expand_spec expands the spec 'spec' which can be $(VARIABLE) and
   or $(function [args]). Additionally you can pass a function which
   is called when the builtin specs do not match to define your own
   specs and/or special variables. The 'ext_func' function is called
   in the second argument with the internal function of
   'expand_spec' which does the real expansion so you can use in your
   code also references to the standard expansion code.

   If you set the last argument to nonzero, the external function
   is called before any builtin function, otherwise it is called
   last.
   */
typedef char *(*token_func) (char *);
typedef char *(*external_token_func) (const char *, token_func);

/* This returns a malloced string ever!!
   Currently it knows about the following string_functions. (For
   a description of them please look in the GNU Make doc, because
   the functionallity is the same (new functions have a small comment):

  $(strip TEXT)
  $(addsuffix suffix,TEXT)
  $(addprefix prefix,TEXT)
  $(notdir filename)
  $(dir filename)
  $(subst from,to,TEXT)
  $(suffix filename)
  $(nosuffix filename) this removes the suffix (if any) from 'filename'
  $(setsuffix suffix,filename) thsi replaces (or add) the 'suffix' for 'filename'
  $(sort TEXT)
  $(foreach variable,list,command)
  $(shell command)
  $(wildcard WILDCARD)
  $(word nr,TEXT)
*/
char *expand_spec(const char *spec, external_token_func ext_func,
                  int expand_extern_first = 0);

/*
  Set this to a function, which is called for each newline '\n'
  character.
*/
extern void (*handle_newline) ();

/*
  Set this variable to 0, if variable-names itself should not
  be expanded, i.e. treat $(var$(other)) as a variable with the
  name 'var$(other)', otherwise $(other) is expande to lets say
  'foo' and $(var$(other)) becomes to $(varfoo) */
extern int expand_variable_names;

/* Some helper functions for 'expand_spec' which expand _ONLY_ variables,
   and no string functions or external specs. The first simply calls the
   second with 'use_env=1' and the 'use_env' of the second functions can
   be used to disable the search in the real environment for the wanted
   variable by setting it to zero. */
const char *GetVariable(const char *variable);
const char *GetVariable(const char *variable, int use_env);
void set_variable(const char *variable, const char *contents);
void set_user_variable(const char *variable, const char *contents);

/* Array of strings with the internal variables with the organization
   vars[i]   - variable name
   vars[i+1] - variable contents
*/
extern char **vars;

/* The count of the variables (Note that this is the variable count and
   _NOT_ the string count in 'vars') */
extern int var_count;

/* Array of strings with the user defined variables with the organization
   user_vars[i]   - variable name
   user_vars[i+1] - variable contents
*/
extern char **user_vars;

/* The count of the user defined variables (Note that this is the variable
   count and  _NOT_ the string count in 'user_vars') */
extern int user_var_count;

/* set this to nonzero to print to stderr debug information about expanding
   the specs */
extern int debug_specs;

/* Add a variable 'variable' with contents 'contents'. If 'contents==NULL',
   then the variable is removed. */
void insert_variable(const char *variable, const char *contents);

/* Add a variable 'variable' with contents 'contents' to the user defined
   variables. If 'contents==NULL', then the variable is removed. */
void insert_user_variable(const char *variable, const char *contents);

/* Some helper functions: 'open_brace' points to an opening brace
   or parenthesis and return the matching closing character or
   NULL if not found */
char *find_close_brace(char *open_brace);

/* Find the next comma in the string where it knows about quoting characters
   '\'' and '"' or return NULL if no comma was found */
char *find_next_comma(char *arg);

/* Split a given filename in its parts given in the format string 'fmt'
   similar to scanf. The results are stored in the paramters you have
   to pass in '...' (for each format spec one 'char *'). Possible format
   codes are:

  %D - drive + directory (drive of course only on DJGPP)
  %d - directory
  %f - filename + suffix
  %F - filename without suffix
  %e - suffix
*/
void split_fname_fmt(const char *filename, const char *fmt, ...);

/* Append 'src' to 'dest' and return 'dest' (malloced or realloced
   strings) */
char *string_cat(char *&dest, const char *src);

/* The same as above, but with variable count of arguments.
   _NOTE_: The last argument must be NULL!!!! */
char *string_cat(char *&, const char *src, const char *src1, ...);

/* Free 'str' and set it to NULL */
void string_free(char *&str);

/* Remove any directory part in 'name' and when 'with_suffix==0' also
   the suffix */
void BaseName(char *name, int with_suffix = 1);

/* The same as above, but store the result in the malloced 'bname' */
void BaseName(const char *name, char *&bname, int with_suffix = 1);

/* Convert any '\' to '/' in 'name' */
void BackslashToSlash(char *name);

/* split the given filename 'fname' in its components and return them
   malloc'd. If the filename contained a drive letter and colon, it will
   be part of 'dir'. */
void split_fname(const char *fname, char *&dir, char *&name, char *&ext);

/* The same as above, except that the drive letter is stored in 'drive'.
   If there is no drive letter, 'drive' will be zero. 'dir' will contain
   only the directory and not (if any) drive and colon. */
void split_fname(const char *fname, char &drive, char *&dir, char *&name,

                 char *&ext);
/* Expands an given name 'name' to an absolute filename relative to the
   current directory. If it was
   already absolute nothing is done. If 'new_alloc==0' the 'name' is
   not realloced */
void FExpand(char *&name, int new_alloc = 1);

/* Very usefull function from DJGPP's libc, which "fixes" the path 'in'
   and placing it in 'out'. If 'in' is a relative filename it is made
   absolute to the current directory. Example:

     char Out[256];
     __fixpath("/foo/foo1/../.././foo",Out);

   will return "/foo" in 'Out' */
#ifndef __DJGPP__
void __fixpath(const char *in, char *out);
#endif

/* Some functions for redirecting stdout and stderr. They cannot
   be called nested!!! */
/* redirect stderr to a file and return the name of that file */
char *open_stderr(void);

/* the same for stdout */
char *open_stdout(void);

/* close the redirected stderr */
void close_stderr(void);

/* the same for stdout */
void close_stdout(void);

/* SET: that's for stderr and stdout redirected to the same file */
char *open_stderr_out(void);
void close_stderr_out(void);

/* Return an unique filename in $TMPDIR which starts with 'before'.
   Store the result also in 'retval' if it is != NULL. This function
   creates the file to make sure that it is really unique. That means
   the application is responsible for deleting this file */
char *unique_name(char *before, char *retval = (char *) 0);


/*
  This functions makes ret_path relative to ref_path
  It assumes that ref_path is an absolute path which can
  optionally have a slash appended. When the files/directories
  were total different, 0 is returned, otherwise 1.
  If ref_path was completely inside ret subst != NULL, then the result
  will be ret where ref_path is substituted by subst.
  If allow_prevdirs == 0, then it returns 1 only, if ref is
  completely part of ref_path.
  max_up_count defines the maximum count of ../'s in the returned
  relative name. If it needs more, it returns 0 and the unmodified
  path.
*/

int AbsToRelPath(const char *ref_path, char *&ret, const char *subst = 0,
                 int allow_prevdirs = 0, int max_up_count = 0);


/*
  Read a line of text from a file. CR's are ignored, LF and EOF
  terminate the line. It returns the number of chars in the
  read line. It returns -1 in case of EOF when calling getline
*/
int getline(char *&line, int &buf_count, std::FILE *f);

/*
  This macros declares a function to be called before main.
  Normally with gcc you can use __attribute__((constructor)), but
  for some reason it didn`t work for me on DJGPP with gcc 2.95.2
  (wit gcc 2.81 it worked). So I designed the following macro, with
  creates a (kopefully) unique class with a constructor where the
  function is called and the it declares a static variable of this
  class.
*/

#define CONSTRUCTOR_FUNCTION(x) \
static void x(); \
\
class __rh_construct_##x\
{\
public:\
  __rh_construct_##x()\
  {\
    x();\
  }\
};\
\
static __rh_construct_##x ___rh_construct_##x;\
static void x()

/*
  The following macros are used to cast the arguments of the ctype
  functions to the proper type without getting negative arguments */

#define rh_isalnum(a)  isalnum((int)(unsigned char)(a))
#define rh_isalpha(a)  isalpha((int)(unsigned char)(a))
#define rh_isascii(a)  isascii((int)(unsigned char)(a))
#define rh_iscntrl(a)  iscntrl((int)(unsigned char)(a))
#define rh_isdigit(a)  isdigit((int)(unsigned char)(a))
#define rh_isgraph(a)  isgraph((int)(unsigned char)(a))
#define rh_islower(a)  islower((int)(unsigned char)(a))
#define rh_isprint(a)  isprint((int)(unsigned char)(a))
#define rh_ispunct(a)  ispunct((int)(unsigned char)(a))
#define rh_isspace(a)  isspace((int)(unsigned char)(a))
#define rh_isupper(a)  isupper((int)(unsigned char)(a))
#define rh_isxdigit(a) isxdigit((int)(unsigned char)(a))
#define rh_toascii(a)  toascii((int)(unsigned char)(a))
#define rh_tolower(a)  tolower((int)(unsigned char)(a))
#define rh_toupper(a)  toupper((int)(unsigned char)(a))

#endif /*
          __rhutils_h 
        */
