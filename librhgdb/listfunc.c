/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdbrh.h>
#include <librhgdb.h>
#include <rhgdbint.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#define block_size 16

static char **filenames = NULL;
static int file_count = 0;
static int file_size = 0;
static struct function_entry *Functions = NULL;
static int function_count = 0;
static int function_size = 0;

static void
free_functions()
{
  int i;

  for (i = 0; i < function_count; i++)
  {
    free(Functions[i].demangled_name);
    free(Functions[i].linkage_name);
    free(Functions[i].return_name);
  }
  function_count = 0;
  for (i = 0; i < file_count; i++)
  {
    free(filenames[i]);
  }
  file_count = 0;
}

static struct function_entry *
insert_function_common(char *file_name)
{
  int i = file_count;
  char *fname = NULL;

  while (i > 0)
  {
    i--;
    if (strcmp(filenames[i], file_name))
      continue;
    fname = filenames[i];
    break;
  }
  if (!fname)
  {
    fname = strdup(file_name);
    if (file_count == file_size)
    {
      file_size += block_size;
      filenames = (char **) realloc(filenames, file_size * sizeof(char **));
    }
    filenames[file_count++] = fname;
  }
  if (function_count == function_size)
  {
    function_size += block_size;
    Functions = (struct function_entry *) realloc(Functions,
                                                  function_size *

                                                  sizeof(struct
                                                         function_entry));
  }
  Functions[function_count].file_name = fname;
  function_count++;
  return Functions + function_count - 1;
}

static void
insertFunction(char *file_name, struct symbol *sym, int bl,
               CORE_ADDR pc, struct objfile *objfile)
{
  char *tmp, *temp;
  struct function_entry *Function;	// = insert_function_common(file_name);
  int d_len = 0;
  char *dname;
  int template_level = 0;
  int is_cplusplus = 0;

  if (bl == STATIC_BLOCK)
  {
    d_len += 7;                 /*
                                   strlen("static ") 
                                 */
  }

  /*
     The following is needed, until GDB will set the correct language for
     symbol, when it is read from a coff file with SDB debug info
   */
  if (SYMBOL_LANGUAGE(sym) == language_unknown)
  {
    SYMBOL_INIT_LANGUAGE_SPECIFIC(sym,
                                  deduce_language_from_filename(file_name));
    SYMBOL_INIT_DEMANGLED_NAME(sym, &objfile->symbol_obstack);
  }
  if (SYMBOL_LANGUAGE(sym) == language_cplus)
    is_cplusplus = 1;
  reset_gdb_output();
  type_print(SYMBOL_TYPE(sym), SYMBOL_SOURCE_NAME(sym), gdb_stdout, 0);
  d_len += get_gdb_output_buffer();
  dname = (char *) alloca(d_len + 1);
  if (bl == STATIC_BLOCK)
    strcpy(dname, "static ");
  else
    *dname = 0;
  strcat(dname, gdb_output_buffer);

  if (strstr(dname, "virtual function thunk") && strstr(dname, "(delta:"))
    return;

  Function = insert_function_common(file_name);
  tmp = strchr(dname, '(');
  temp = tmp - 1;
  while (*temp != ' ' || template_level > 0)
  {
    if (is_cplusplus)
    {
      if (*temp == '>')
        template_level++;
      if (*temp == '<')
        template_level--;
    }
    temp--;
  }
  temp++;
  while (*temp == '*' || *temp == '&')
    temp++;
  Function->demangled_name = strdup(temp);

  *temp = 0;
  Function->return_name = strdup(dname);
  Function->linkage_name = strdup(SYMBOL_NAME(sym));
  Function->line_number = find_pc_line(pc, 0).line;
}

#if 0
static void
insertFunctionPsym(char *file_name, struct partial_symbol *sym, int bl,
                   CORE_ADDR pc, struct objfile *objfile)
{
  char *tmp, *temp;
  struct function_entry *Function = insert_function_common(file_name);
  int d_len = 0;
  char *dname;

  if (bl == STATIC_BLOCK)
  {
    d_len += 7;                 /*
                                   strlen("static ") 
                                 */
  }

  /*
     The following is needed, until GDB will set the correct language for
     symbol, when it is read from a coff file with SDB debug info
   */
  if (SYMBOL_LANGUAGE(sym) == language_unknown)
  {
    SYMBOL_INIT_LANGUAGE_SPECIFIC(sym,
                                  deduce_language_from_filename(file_name));
    SYMBOL_INIT_DEMANGLED_NAME(sym, &objfile->symbol_obstack);
  }
  reset_gdb_output();
  type_print(builtin_type_void, SYMBOL_SOURCE_NAME(sym), gdb_stdout, 0);
  d_len += strlen(gdb_output_buffer);
  dname = (char *) alloca(d_len + 1);
  if (bl == STATIC_BLOCK)
    strcpy(dname, "static ");
  else
    *dname = 0;
  strcat(dname, gdb_output_buffer);
  tmp = strchr(dname, '(');
  temp = tmp - 1;
  while (*temp != ' ')
    temp--;
  temp++;
  while (*temp == '*' || *temp == '&')
    temp++;
  Function->demangled_name = strdup(temp);
  *temp = 0;
  Function->return_name = strdup(dname);
  Function->linkage_name = strdup(SYMBOL_NAME(sym));
  Function->line_number = find_pc_line(pc, 0).line;
}
#endif

static void
list_symbols(char *regexp)
{
  register struct symtab *s;
  register struct blockvector *bv;
  struct blockvector *prev_bv = NULL;
  register struct block *b;
  register int i, j;
  register struct symbol *sym;
  struct objfile *objfile;
  register struct partial_symtab *ps;
  struct partial_symbol **psym;

  if (regexp != NULL)
    re_comp(regexp);

  /*
     Search through the partial symtabs *first* for all symbols
     matching the regexp.  That way we don't have to reproduce all of
     the machinery below. 
   */

  ALL_PSYMTABS(objfile, ps)
  {
    struct partial_symbol **bound, **gbound, **sbound;
    int keep_going = 1;

    if (ps->readin)
      continue;

    gbound =
      objfile->global_psymbols.list + ps->globals_offset + ps->n_global_syms;
    sbound =
      objfile->static_psymbols.list + ps->statics_offset + ps->n_static_syms;
    bound = gbound;

    /*
       Go through all of the symbols stored in a partial
       symtab in one loop. 
     */
    psym = objfile->global_psymbols.list + ps->globals_offset;
    while (keep_going)
    {
      if (psym >= bound)
      {
        if (bound == gbound && ps->n_static_syms != 0)
        {
          psym = objfile->static_psymbols.list + ps->statics_offset;
          bound = sbound;
        }
        else
          keep_going = 0;
        continue;
      }
      else
      {
/*	      QUIT; */

        /*
           If it would match (logic taken from loop below)
           load the file and go on to the next one 
         */
        if ((regexp == NULL || SYMBOL_MATCHES_REGEXP(*psym))
            && SYMBOL_CLASS(*psym) == LOC_BLOCK)
        {
#if 1
          PSYMTAB_TO_SYMTAB(ps);
          keep_going = 0;
#else
          insertFunctionPsym(ps->filename, *psym, 0,
                             SYMBOL_VALUE_ADDRESS(*psym), objfile);
#endif
        }
      }
      psym++;
    }
  }

  ALL_SYMTABS(objfile, s)
  {
    bv = BLOCKVECTOR(s);
    /*
       Often many files share a blockvector.
       Scan each blockvector only once so that
       we don't get every symbol many times.
       It happens that the first symtab in the list
       for any given blockvector is the main file.  
     */
    if (bv != prev_bv)
      for (i = GLOBAL_BLOCK; i <= STATIC_BLOCK; i++)
      {
        b = BLOCKVECTOR_BLOCK(bv, i);
        /*
           Skip the sort if this block is always sorted.  
         */
        if (!BLOCK_SHOULD_SORT(b))
          sort_block_syms(b);
        for (j = 0; j < BLOCK_NSYMS(b); j++)
        {
          sym = BLOCK_SYM(b, j);
          if (SYMBOL_CLASS(sym) == LOC_BLOCK &&
              (regexp == NULL || SYMBOL_MATCHES_REGEXP(sym)))
          {
            insertFunction(s->filename, sym, i,
                           BLOCK_START(SYMBOL_BLOCK_VALUE(sym)), objfile);
          }
        }
      }
    prev_bv = bv;
  }
}

static int
function_compare(const void *key1, const void *key2)
{
#define F1 ((struct function_entry *)key1)
#define F2 ((struct function_entry *)key2)
  return strcmp(F1->demangled_name, F2->demangled_name);
#undef F1
#undef F2
}

static void
sort_functions()
{
  qsort(Functions, function_count, sizeof(struct function_entry),

        function_compare);
}

void
ListFunctions(const char *regex, struct function_entry **list, int *count)
{
  free_functions();
  if (!debugger_started)
  {
    if (!InitRHGDB())
    {
      *count = 0;
      *list = NULL;
      return;
    }
  }
  list_symbols((char *) regex);
  if (function_count > 0)
    sort_functions();
  *count = function_count;
  *list = Functions;
}

#ifdef TEST

#include <stdio.h>

static char *progname;

static char *
GetProgName()
{
  return progname;
}

int
main(int argc, char *argv[])
{
  int count, i;
  struct function_entry *functions;

  progname = argv[1];
  _GetProgName = GetProgName;
  InitRHGDB();
  ListFunctions(argc > 2 ? argv[2] : NULL, &functions, &count);
  for (i = 0; i < count; i++)
  {
    fprintf(stderr, "demangled: %s linkage: %s file: %s\n",
            functions[i].demangled_name,
            functions[i].linkage_name, functions[i].file_name);
  }
  return 0;
}

#endif
