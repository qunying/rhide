/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <stdio.h>
#include <string.h>

static char chap[] = "chapentry";
static char sec[] = "secentry";
static char subsec[] = "subsecentry";
static char subsubsec[] = "subsubsecentry";

static char *levels[] = {
  chap,
  sec,
  subsec,
  subsubsec,
  NULL
};

#define UL "<MENU>"
#define ul "</MENU>"

int get_level(char *what)
{
  int i = 0;
  while (levels[i])
  {
    if (strncmp(what,levels[i],strlen(levels[i])) == 0)
      return i;
    i++;
  }
  return -1;
}

int current_level = -1;
char buffer[1000];

int main()
{
  int level;
  char *name,*number,*name_end,*number_end;
  while (fgets(buffer,999,stdin))
  {
    level = get_level(buffer);
    if (level < 0) return -1;
    while (level > current_level)
    {
      fputs(UL"\n",stdout);
      current_level++;
    }
    while (current_level > level)
    {
      fputs(ul"\n",stdout);
      current_level--;
    }
    name = strchr(buffer,'{');
    name++;
    name_end = strchr(name,'}');
    number = strchr(name,'{');
    number++;
    number_end = strchr(number,'}');
    *name_end = 0;
    *number_end = 0;
    fprintf(stdout,"@w{<LI> <A HREF=\"#%s\">%s %s</A>}\n",name,number,name);
  }
  while (current_level >= 0)
  {
    fputs(ul,stdout);
    current_level--;
  }
  return 0;
}
