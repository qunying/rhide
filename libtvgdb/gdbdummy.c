/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/* This file is used only to reduce the size of the executable.
   It is a very strong hack.
*/

int gdbdummy;

#ifdef __DJGPP__
#include <string.h>
#include <stdio.h>
#define NULL 0

char *tilde_expand(char *string)
{
  return strdup(string);
}

#define D(x) void x()\
{\
  fprintf(stderr,\
"Internal error while calling %s()\n"\
"Please report it to robert.hoehne@mathematik.tu-chemnitz.de\n"\
"If you can't wait for a fix, get the sources of RHIDE,\n"\
"change the first '#if 1' in "__FILE__" to '#if 0' from the libtvgdb.a\n"\
"library and recompile the program\n",#x);\
  abort();\
}

#define DD(x) void x() {}
#define _D(t,x,r) t x() { return r; }

D(filename_completion_function)
D(add_history)
D(history_expand)
D(write_history)
D(read_history)
D(history_get)
D(stifle_history)
D(unstifle_history)
int history_base;
char *rl_completer_word_break_characters;
char *rl_completer_quote_characters;
D(readline)
int rl_point;
char *rl_line_buffer;
void *rl_completion_entry_function;
char *rl_readline_name;

DD(_initialize_copying)
DD(_initialize_dcache)
DD(_initialize_sr_support)
DD(_initialize_remote)
DD(_initialize_ser_dos)
DD(_initialize_mipsread)
DD(_initialize_os9kread)
DD(_initialize_nlmread)
DD(_initialize_m2_language)

/* gdb/remote.o */
D(putpkt)
D(getpkt)
D(remote_console_output)

/* bfd/cofflink.o */

D(_bfd_coff_link_add_symbols)
D(_bfd_coff_write_global_sym)
D(_bfd_coff_link_hash_table_create)
D(_bfd_coff_link_hash_table_init)
D(_bfd_coff_link_hash_newfunc)
D(_bfd_coff_generic_relocate_section)
D(_bfd_coff_reloc_link_order)
D(_bfd_coff_link_input_bfd)
D(_bfd_coff_final_link)
D(_bfd_coff_debug_merge_hash_newfunc)

/* bfd/archive.o */

D(_bfd_generic_read_ar_hdr)
D(_bfd_look_for_bfd_in_cache)
D(bfd_get_next_mapent)
D(_bfd_archive_coff_construct_extended_name_table)
D(_bfd_add_bfd_to_archive_cache)
D(bfd_gnu_truncate_arname)
D(bfd_generic_archive_p)
D(bfd_dont_truncate_arname)
D(bsd_write_armap)
D(bfd_bsd_truncate_arname)
D(bfd_openr_next_archived_file)
D(_bfd_archive_bsd_construct_extended_name_table)
D(_bfd_construct_extended_name_table)
D(_bfd_get_elt_at_filepos)
D(bfd_generic_stat_arch_elt)
D(bfd_generic_openr_next_archived_file)
D(_bfd_generic_read_ar_hdr_mag)
D(_bfd_slurp_extended_name_table)
D(_bfd_generic_mkarchive)
D(bfd_set_archive_head)
D(bfd_special_undocumented_glue)
D(_bfd_generic_get_elt_at_index)
D(bfd_slurp_armap)
D(_bfd_create_empty_archive_element_shell)
D(_bfd_write_archive_contents)
D(coff_write_armap)
D(_bfd_archive_bsd_update_armap_timestamp)
D(bfd_slurp_bsd_armap_f2)
D(_bfd_compute_and_write_armap)

/* bfd/linker.o */
D(bfd_link_hash_traverse)
D(_bfd_link_hash_newfunc)
D(bfd_link_hash_lookup)
D(_bfd_generic_link_output_symbols)
D(_bfd_generic_link_add_symbols_collect)
D(bfd_wrapped_link_hash_lookup)
D(bfd_new_link_order)
D(_bfd_generic_link_add_symbols)
D(_bfd_generic_final_link)
D(_bfd_count_link_order_relocs)
D(_bfd_default_link_order)
D(_bfd_generic_link_add_archive_symbols)
D(_bfd_generic_reloc_link_order)
D(bfd_link_add_undef)
D(_bfd_generic_link_hash_newfunc)
D(_bfd_link_hash_table_init)
D(_bfd_generic_link_hash_table_create)
D(_bfd_generic_link_write_global_symbol)
D(_bfd_generic_link_split_section)
D(_bfd_generic_link_add_one_symbol)

/* bfd/reloc.o */
D(bfd_get_reloc_size)
D(bfd_install_relocation)
D(bfd_generic_relax_section)
D(bfd_perform_relocation)
D(_bfd_final_link_relocate)
D(_bfd_relocate_contents)
D(bfd_get_reloc_code_name)
D(bfd_default_reloc_type_lookup)
D(bfd_generic_get_relocated_section_contents)
D(bfd_reloc_type_lookup)

DD(_initialize_maint_cmds)

/* no chill language */
DD(_initialize_chill_language)
char *chill_demangle(char *name __attribute__((unused)))
{
  return NULL;
}
void *builtin_type_chill_bool;

#if 1
DD(_initialize_source)
char *source_path;
int lines_to_list = 10;
int current_source_line;
void *current_source_symtab;
D(select_source_symtab)
DD(forget_cached_source_info)
D(mod_path)
D(symtab_to_filename)
D(init_source_path)
DD(print_source_lines)
D(identify_source_line)
D(directory_command)

extern char *current_directory;
extern char *savestring (const char *, int);
extern char *concat (const char *, ...);
extern int open();
#define DIRNAME_SEPARATOR ';'
#define SLASH_P(X) ((X)=='/' || (X)=='\\')
#define SLASH_CHAR '/'
#define SLASH_STRING "/"
#define ROOTED_P(X) (SLASH_P((X)[0]) || ((X)[0] && (X)[1] == ':'))

/* Open a file named STRING, searching path PATH (dir names sep by some char)
   using mode MODE and protection bits PROT in the calls to open.

   If TRY_CWD_FIRST, try to open ./STRING before searching PATH.
   (ie pretend the first element of PATH is ".").  This also indicates
   that a slash in STRING disables searching of the path (this is
   so that "exec-file ./foo" or "symbol-file ./foo" insures that you
   get that particular version of foo or an error message).

   If FILENAMED_OPENED is non-null, set it to a newly allocated string naming
   the actual file opened (this string will always start with a "/".  We
   have to take special pains to avoid doubling the "/" between the directory
   and the file, sigh!  Emacs gets confuzzed by this when we print the
   source file name!!! 

   If a file is found, return the descriptor.
   Otherwise, return -1, with errno set for the last name we tried to open.  */

/*  >>>> This should only allow files of certain types,
    >>>>  eg executable, non-directory */
int
openp (path, try_cwd_first, string, mode, prot, filename_opened)
     char *path;
     int try_cwd_first;
     char *string;
     int mode;
     int prot;
     char **filename_opened;
{
  register int fd;
  register char *filename;
  register char *p, *p1;
  register int len;
  int alloclen;

  if (!path)
    path = ".";

  if (try_cwd_first || SLASH_P (string[0]))
    {
      int i;
      filename = string;
      fd = open (filename, mode, prot);
      if (fd >= 0)
	goto done;
      for (i = 0; string[i]; i++)
	if (SLASH_P (string[i]))
	  goto done;
    }

  /* ./foo => foo */
  while (string[0] == '.' && SLASH_P (string[1]))
    string += 2;

  alloclen = strlen (path) + strlen (string) + 2;
  filename = (char *) alloca (alloclen);
  fd = -1;
  for (p = path; p; p = p1 ? p1 + 1 : 0)
    {
      p1 = (char *) strchr (p, DIRNAME_SEPARATOR);
      if (p1)
	len = p1 - p;
      else
	len = strlen (p);

      if (len == 4 && p[0] == '$' && p[1] == 'c'
	  && p[2] == 'w' && p[3] == 'd') {
	/* Name is $cwd -- insert current directory name instead.  */
	int newlen;

	/* First, realloc the filename buffer if too short. */
	len = strlen (current_directory);
	newlen = len + strlen (string) + 2;
	if (newlen > alloclen) {
	  alloclen = newlen;
	  filename = (char *) alloca (alloclen);
	}
	strcpy (filename, current_directory);
      } else {
	/* Normal file name in path -- just use it.  */
	strncpy (filename, p, len);
	filename[len] = 0;
      }

      /* Remove trailing slashes */
      while (len > 0 && SLASH_P (filename[len-1]))
	filename[--len] = 0;

      strcat (filename+len, SLASH_STRING);
      strcat (filename, string);

      fd = open (filename, mode);
      if (fd >= 0) break;
    }

 done:
  if (filename_opened)
    {
      if (fd < 0)
	*filename_opened = (char *) 0;
      else if (ROOTED_P (filename))
	*filename_opened = savestring (filename, strlen (filename));
      else
	{
	  /* Beware the // my son, the Emacs barfs, the botch that catch... */
	  
	  *filename_opened = concat (current_directory, 
				     SLASH_CHAR
				     == current_directory[strlen(current_directory)-1] 
  				     ? "": SLASH_STRING,
				     filename, NULL);
        }
    }

  return fd;
}

#endif

D(elfmdebug_build_psymtabs)
DD(_initialize_mdebugread)
D(mdebug_build_psymtabs)
D(fixup_sigtramp)
D(ecoff_relocate_efi)

DD(elfstab_offset_sections)
DD(_initialize_elfread)

D(scm_val_print)
D(scm_value_print)
D(scm_inferior_print)
D(scm_scmval_print)

D(scm_parse)
D(scm_istr2int)
D(scm_istring2number)

D(scm_lookup_name)
D(scm_printchar)
D(scm_evaluate_string)
DD(_initialize_scheme_language)
D(is_scmvalue_type)
D(scm_unpack)
D(scm_get_field)

#if 0
DD(_initialize_thread)
_D(int,in_thread_list,0)
D(init_thread_list)
D(save_infrun_state)
_D(int,pid_to_thread_id,0)
D(add_thread)
D(valid_thread_id)
D(load_infrun_state)
#endif

#endif /* __DJGPP__ */


