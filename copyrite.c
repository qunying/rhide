/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <limits.h>

#define CP \
"%s Copyright (C) 1996-%d Robert H”hne, see COPYING.RH for details %s\n" \
"%s This file is part of RHIDE. %s\n"
#define CP1 \
"%s Copyright (C) 1996-%d Robert H”hne, see COPYING.RHU for details %s\n" \

#define CO(start,end) sprintf(cline, CP, start, y, end, start, end); \
                      sprintf(cline1, CP1, start, y, end)

  int i,y,j;
  char pathp[PATH_MAX], fname[PATH_MAX], ext[PATH_MAX];
  struct stat st;
  char cline[200];
  char cline1[200];
  struct tm *tm;
  char line1[2000];
  char tmp[300];
  FILE *f,*wf;
  struct utimbuf ut;

void split_fname(char *_fname,char *path,char *name, char *ext)
{
  char *fname = alloca(strlen(_fname)+1);
  char *tmp;
  strcpy(fname,_fname);
  tmp = strrchr(fname,'.');
  if (!tmp)
    *ext = 0;
  else
  {
    strcpy(ext,tmp);
    *tmp = 0;
  }
  tmp = strrchr(fname,'/');
  if (!tmp)
  {
    strcpy(name,fname);
    *fname = 0;
  }
  else
  {
    strcpy(name,tmp+1);
    tmp[1] = 0;
  }
  if (!*fname)
    strcpy(path,"./");
  else
    strcpy(path,fname);
}
   

int
main(int argc, char *argv[])
{
  int l;
  for (i = 1; i<argc; i++)
  {
    split_fname(argv[i], pathp, fname, ext);
    
    if (stat(argv[i], &st) < 0)
      continue;
    tm = localtime(&st.st_mtime);
    y = tm->tm_year;
    if (y<80) y += 2000;
    if (y<200) y += 1900;
    /* Compute what we'd like the first line to be */
    if (!strcmp(ext, ".c")
	|| !strcmp(ext, ".h")
	|| !strcmp(ext, ".cc")
	|| !strcmp(ext, ".cpp")
	|| !strcmp(ext, ".y")
	|| !strcmp(ext, ".s"))
    {
      CO("/*","*/");
    }
    else if (!strcmp(ext, ".def") || !strcmp(ext, ".mak") ||
             (!strcmp(fname, "makefile") && (!strcmp(ext, "") || !strcmp(ext, ".src"))) ||
             !strcmp(ext, ".sed") ||
             !strcmp(ext, ".env") || !strcmp(ext, ".var")
             )
    {
      CO("#","");
    }
    else if (!strcmp(ext, ".asm"))
    {
      CO(";","");
    }
    else if (!strcmp(ext, "txi"))
    {
      CO("@c","");
    }
    else if (!strcmp(ext, ".tx") || !strcmp(ext, ".tex"))
    {
      CO("\x25","");
    }
    else
      continue;

    /* read first line */
    f = fopen(argv[i], "rt");
    fgets(line1, 2000, f);

    /* It's OK */
    if (strncmp(line1, cline, strlen(line1)) == 0)
    {
      fclose(f);
      continue;
    }

    /* Is it copyright someone else? */
    if (strstr(line1, "Copyright") && !strstr(line1, "H”hne"))
    {
      printf("%s: %s", argv[i], line1);
      fclose(f);
      continue;
    }

    /* We need to append the right copyright notice. */

    if (pathp[0] == 0)
      strcpy(tmp, "tempxxx.crn");
    else
      sprintf(tmp, "%stempxxx.crn", pathp);
    printf("updating %s\n", argv[i]);

    wf = fopen(tmp, "w+");
    if (strstr(line1, "COPYING.RHU"))
    {
      fputs(cline1, wf);
      l = 1;
    }
    else
    {
      fputs(cline, wf);
      l = 0;
    }
    while (fgets(line1, 2000, f))
    {
      l++;
      if (l == 1) /* forget the second line if it starts with
                     the same comment character */
      {
        if (line1[0] == cline[0])
          continue;
      }
      fputs(line1, wf);
    }
    fclose(wf);

    fclose(f);

    rename(tmp, argv[i]);

    ut.actime = st.st_atime;
    ut.modtime = st.st_mtime;
    utime(argv[i], &ut);
  }
  return 0;
}
