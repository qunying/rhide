/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdbrh.h>
#include <librhgdb.h>
#include <rhgdbint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static char _sprintf_buf[2048];
static char sprintf_buf[2048];
static char *disass_buf = NULL;
static int disass_size = 0;
static int disass_len = 0;
static int mark_functions = 1;
static int mark_source = 1;

static int
my_print_address_symbolic(CORE_ADDR addr, int only_exact,
                          int with_source, char *leading, char *trailing);
static void my_fprintf(FILE * stream, const char *fmt, ...);

static void
my_fprintf(FILE * stream, const char *fmt, ...)
{
  va_list va;
  int len;

  va_start(va, fmt);
  if (stream && mark_functions)
    vsprintf(_sprintf_buf, fmt, va);
  else
    vsprintf(sprintf_buf, fmt, va);
  va_end(va);
  if (stream && mark_functions)
  {
    char *tmp, *tmp1 = _sprintf_buf;

    while ((tmp = strstr(tmp1, "0x")) != NULL)
    {
      unsigned long addr;

      *tmp = 0;
      my_fprintf(NULL, "%s", tmp1);
      *tmp = 0;
      tmp += 2;
      sscanf(tmp, "%lx", &addr);
      while (isxdigit(*tmp))
        tmp++;
      if (!my_print_address_symbolic(addr, 1, 0, "", ""))
      {
        my_fprintf(NULL, "0x%lx", addr);
      }
      tmp1 = tmp;
    }
    my_fprintf(NULL, "%s", tmp1);
    return;
  }
  len = strlen(sprintf_buf);
  while (disass_len + len + 1 > disass_size)
  {
    disass_size += 1024;
    disass_buf = (char *) realloc(disass_buf, disass_size);
  }
  strcpy(disass_buf + disass_len, sprintf_buf);
  disass_len += len;
}

#define max_symbolic_offset UINT_MAX

static int
my_print_address_symbolic(CORE_ADDR addr, int only_exact,
                          int with_source, char *leading, char *trailing)
{
  struct minimal_symbol *msymbol;
  struct symbol *symbol;
  struct symtab *symtab = 0;
  CORE_ADDR name_location = 0;
  char *name = "";
  struct symtab_and_line sal;

  /*
     First try to find the address in the symbol table, then
     in the minsyms.  Take the closest one.  
   */

  /*
     This is defective in the sense that it only finds text symbols.  So
     really this is kind of pointless--we should make sure that the
     minimal symbols have everything we need (by changing that we could
     save some memory, but for many debug format--ELF/DWARF or
     anything/stabs--it would be inconvenient to eliminate those minimal
     symbols anyway).  
   */
  symbol = find_pc_function(addr);
  if (symbol)
    name_location = BLOCK_START(SYMBOL_BLOCK_VALUE(symbol));

  if (symbol)
  {
    name = SYMBOL_SOURCE_NAME(symbol);
  }

  msymbol = lookup_minimal_symbol_by_pc(addr);
  if (msymbol != NULL)
  {
    if (SYMBOL_VALUE_ADDRESS(msymbol) > name_location || symbol == NULL)
    {
      /*
         The msymbol is closer to the address than the symbol;
         use the msymbol instead.  
       */
      symbol = 0;
      symtab = 0;
      name_location = SYMBOL_VALUE_ADDRESS(msymbol);
      name = SYMBOL_SOURCE_NAME(msymbol);
    }
  }
  if (symbol == NULL && msymbol == NULL)
    return 0;

  if (only_exact && addr != name_location)
    return 0;

  /*
     If the nearest symbol is too far away, don't print anything symbolic.  
   */

  /*
     For when CORE_ADDR is larger than unsigned int, we do math in
     CORE_ADDR.  But when we detect unsigned wraparound in the
     CORE_ADDR math, we ignore this test and print the offset,
     because addr+max_symbolic_offset has wrapped through the end
     of the address space back to the beginning, giving bogus comparison.  
   */
  if (addr > name_location + max_symbolic_offset
      && name_location + max_symbolic_offset > name_location)
    return 0;

  my_fprintf(NULL, "%s%s", leading, name);
  if (addr != name_location)
    my_fprintf(NULL, "+%u", (unsigned int) (addr - name_location));

  if (with_source)
  {
    /*
       Append source filename and line number if desired.  Give specific
       line # of this addr, if we have it; else line # of the nearest symbol.  
     */

    sal = find_pc_line(addr, 0);
    if (sal.symtab)
      my_fprintf(NULL, " (at %s:%d)", sal.symtab->filename, sal.line);
    else if (symtab && symbol && symbol->line)
      my_fprintf(NULL, " (at %s:%d)", symtab->filename, symbol->line);
    else if (symtab)
      my_fprintf(NULL, " (in %s)", symtab->filename);
  }
  my_fprintf(NULL, "%s", trailing);
  return 1;
}

static void
my_print_address(bfd_vma addr,
                 struct disassemble_info *info __attribute__ ((unused)))
{
  if (!mark_functions || !my_print_address_symbolic(addr, 0, 0, "", ""))
    my_fprintf(NULL, "0x%lx", addr);
}

static int
print_insn(memaddr, stream)
     CORE_ADDR memaddr;
     RH_GDB_FILE *stream;
{
  disassemble_info info;
  int retval;

  INIT_DISASSEMBLE_INFO(info, stream, (fprintf_ftype) my_fprintf);
  info.read_memory_func = dis_asm_read_memory;
  info.memory_error_func = dis_asm_memory_error;
  info.print_address_func = my_print_address;

  /*
     If there's no disassembler, something is very wrong.  
   */
  if (tm_print_insn == NULL)
    abort();

  if (mark_functions)
    my_print_address_symbolic(memaddr, 1, mark_source, " ", ":\n");
  my_fprintf(NULL, "%08lx:  ", memaddr);

  retval = (*tm_print_insn) (memaddr, &info);
  my_fprintf(NULL, "\n");
  return retval;
}

static char **disass_lines = NULL;
static int disass_lines_count = 0;
static int disass_lines_size = 0;

const char *const *
disassemble(unsigned long address, int *line_count,
            int mark_start, int with_source, unsigned long *next_address)
{
  int i;
  char *tmp, *tmp1;

  if (!debugger_started)
  {
    *line_count = 0;
    *next_address = 0;
    return NULL;
  }
  disass_len = 0;
  mark_functions = mark_start;
  mark_source = with_source;
  for (i = 0; i < *line_count; i++)
  {
    address += print_insn(address, stdout);
  }
  *next_address = address;
  for (i = 0; i < disass_lines_count; i++)
  {
    free(disass_lines[i]);
  }
  tmp = disass_buf;
  *line_count = 0;
  while ((tmp = strchr(tmp, '\n') + 1) != (char *) 1)
    (*line_count)++;
  if (*line_count > disass_lines_size)
  {
    disass_lines =

      (char **) realloc(disass_lines, *line_count * sizeof(char *));
    disass_lines_size = *line_count;
  }
  tmp = disass_buf;
  i = 0;
  while (i < *line_count)
  {
    tmp1 = strchr(tmp, '\n');
    *tmp1 = 0;
    disass_lines[i] = strdup(tmp);
    *tmp1 = '\n';
    tmp = tmp1 + 1;
    i++;
  }
  disass_lines_count = *line_count;
  return (const char *const *) disass_lines;
}
