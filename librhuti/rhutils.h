/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#ifndef __rhutils_h
#define __rhutils_h

#ifndef __cplusplus
#error You can use these functions only in C++ programs!!
#endif

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
   code also references to the standard expansion code. */
typedef char *(*token_func)(const char *);
typedef char *(*external_token_func)(const char *, token_func);
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
char *expand_spec(const char *spec, external_token_func ext_func);

/* Some helper functions for 'expand_spec' which expand _ONLY_ variables,
   and no string functions or external specs. The first simply calls the
   second with 'use_env=1' and the 'use_env' of the second functions can
   be used to disable the search in the real environment for the wanted
   variable by setting it to zero. */
const char * GetVariable(const char *variable);
const char * GetVariable(const char *variable,int use_env);
void set_variable(const char *variable, const char *contents);

/* Array of strings with the internal variables with the organization
   vars[i]   - variable name
   vars[i+1] - variable contents
*/
extern char **vars;
/* The count of the variables (Note that this is the variable count and
   _NOT_ the string count in 'vars') */
extern int var_count;

/* set this to nonzero to print to stderr debug information about expanding
   the specs */
extern int debug_specs;

/* Add a variable 'variable' with contents 'contents'. If 'contents==NULL',
   then the variable is removed. */
void insert_variable(const char *variable, const char *contents);

/* Some helper functions: 'open_brace' points to and opening brace
   or parenthesis and return the matching closing character or
   NULL of not found */
char *find_close_brace(char *open_brace);
/* Find the next comma in the string where it knows about quoting characters
   '\'' and '"' or NULL of no comma was found */
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
char *string_cat(char *&, const char *src, const char *src, ...);
/* Free 'str' and set it to NULL */
void string_free(char *&str);
/* Remove any directory part in 'name' and when 'with_suffix==0' also
   the suffix */
void BaseName(char *name, int with_suffix = 1);
/* The same as above, but store the result in the malloced 'bname' */
void BaseName(const char *name, char *&bname, int with_suffix = 1);
/* Convert any '\' to '/' in 'name' */
void BackslashToSlash(char * name);
/* call internally split_fname_fmt to return the results in the malloced
   arguments */
void split_fname(const char *fname, char *&dir, char *&name, char *&ext);
/* Expands an given name 'name' to an absolute filename relative to the
   current directory. If it was
   already absolute nothing is done. If 'new_alloc==0' the 'name' is
   not realloced */
void FExpand(char * & name,int new_alloc=1);

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
char * open_stderr(void);
/* the same for stdout */
char * open_stdout(void);
/* close the redirected stderr */
void close_stderr(void);
/* the same for stdout */
void close_stdout(void);

/* Return an unique filename in $TMPDIR which starts with 'before'.
   Store the result also in 'retval' if it is != NULL. This function
   creates the file to make sure that it is really unique. That means
   the application is responsible for deleting this file */
char *unique_name(char *before,char *retval = (char *)0);

#endif /* __rhutils_h */


