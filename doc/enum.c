/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 * Automated chapter numbering for Texinfo files.
 *
 * Restrictions:
 *
 *   1. Only supports @chapter and @section directives.
 *   2. The chapters and sections MUST be in their correct
 *      order, and all the sections MUST be between their
 *      parent chapter and the next chapter.
 *   3. Conditional (@ifset, @ifclear) and file inclusion
 *      (@include) directives not supported.  If you must
 *      have file inclusion, feed the program with expanded
 *      file (use -E switch to Makeinfo).
 *
 * Author: Eli Zaretskii <eliz@is.elta.co.il>
 *
 * Version: 1.0
 *
 * Last updated: 15 October, 1995
 *
 * ----------------------------------------------------------
 *
 * You can do whatever you like with this program, except:
 * (1) preventing other people (including the author) do
 * whatever they like, and (2) removing the author and
 * version info above.
 *
 * ----------------------------------------------------------
 *
 * This was modified to use it for the documentation of RHIDE
 * by Robert H”hne <robert.hoehne@mathematik.tu-chemnitz.de>
 * 
 * I have changed it to accept also the @subsection, @subsubsection
 * directives.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#ifdef  __DJGPP__

/* Make so our start-up code is minimal: disable filename
   globbing, and don't load environment file.  */
#include <crt0.h>

char ** __crt0_glob_function(char *arg) { return (char **)0; }
void   __crt0_load_environment_file(char *app_name) {}

#endif  /* __DJGPP__ */

#define S(x) static char x##[] = "@"#x;\
static int x##_no = 0;
static int dummy_no = 0;
S(chapter)
S(section)
S(subsection)
S(subsubsection)

static char **directories = NULL;
static int dir_count = 0;

static void add_directory(char *dir)
{
  dir_count++;
  directories = (char **)realloc(directories,dir_count);
  directories[dir_count-1] = strdup(dir);
}

FILE *open_file(char *name)
{
  FILE *f;
  char tmp[512];
  int i;
  f = fopen(name,"r");
  if (f) return f;
  for (i=0;i<dir_count;i++)
  {
    strcpy(tmp,directories[i]);
    strcat(tmp,"/");
    strcat(tmp,name);
    f = fopen(tmp,"r");
    if (f) return f;
  }
  return NULL;
}

static int read_file(char *fname,FILE *fp_out)
{
      size_t maxline = 100;     /* not necessarily a limit, see below */
      char *linebuf = (char *)malloc(maxline);
      FILE *fp_in;
      if (strcmp(fname,"-") == 0) fp_in = stdin;
      else fp_in = open_file(fname);
      if (linebuf == (char *)0)
        {
          errno = ENOMEM;
          perror("line storage allocation");
          return 2;
        }
      if (fp_in == (FILE *)0)
        {
          perror(fname);
          return 2;
        }
      while (fgets(linebuf, maxline, fp_in))
        {
          size_t linelen = strlen(linebuf);

          /* If this line is longer than linebuf[], enlarge the
             buffer and read until end of this line.  */
          while (linebuf[linelen - 1] != '\n')
            {
              maxline *= 2;
              linebuf = (char *)realloc(linebuf, maxline);
              if (linebuf == (char *)0)
                {
                  errno = ENOMEM;
                  perror("line storage re-allocation");
                  fclose(fp_in);
                  return 2;
                }

              while (linelen < maxline && linebuf[linelen - 1] != '\n')
                {
                  linebuf[linelen++] = fgetc(fp_in);

                  if (feof(fp_in))
                    linebuf[linelen-1] = '\n';
                }

              linebuf[linelen] = '\0';
            }
            if (strncmp(linebuf,"@include {",sizeof("@include {")-1) == 0)
            {
              char *name = linebuf + sizeof("@include {")-1;
              char c,*end;
              int retval;
              end = name;
              while (*end != '}') end++;
              c = *end;
              *end = 0;
              retval = read_file(name,fp_out);
              strcpy(linebuf,"\n");
              if (retval != 0)
              {
                fclose(fp_in);
                return retval;
              }
            }
#define SCAN(name,reset,format...)\
          if (memcmp(linebuf, name, sizeof(name) - 1) == 0)\
            {\
              name##_no++;\
              reset##_no = 0;\
              fputs(name, fp_out);\
              fprintf(fp_out, " "##format);\
              fputs(linebuf + sizeof(name) - 1, fp_out);\
            }

SCAN(chapter,section,"%d.",chapter_no)
else
SCAN(section,subsection,"%d.%d",chapter_no,section_no)
else
SCAN(subsection,subsubsection,"%d.%d.%d",chapter_no,section_no,subsection_no)
else
SCAN(subsubsection,dummy,"%d.%d.%d.%d",chapter_no,section_no,subsection_no,subsubsection_no)
          else
            fputs(linebuf, fp_out);
        }
    fclose(fp_in);
    return 0;
}

int
main(int argc, char *argv[])
{
  while (argc > 3)
  {
    argc--;
    if (strcmp(argv[1],"-I") == 0)
    {
      argc--;
      argv++;
      add_directory(argv[1]);
    }
    else
    ;
    argv++;
  }
  if (argc == 3)
    {
      FILE *fp_out;
      if (strcmp(argv[2],"-") == 0)
      {
        fp_out = stdout;
      }
      else
      {
        fp_out = fopen(argv[2], "w");

        if (fp_out == (FILE *)0)
          {
            perror(argv[2]);
            return 2;
          }
      }

      read_file(argv[1],fp_out);

      fclose(fp_out);
      return 0;
    }
  else
    {
      fprintf(stderr, "Usage: %s infile outfile\n", *argv);
      return 1;
    }
}
