/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <stdio.h>
#include <string.h>

static char chap[] = "chapentry";
static char sec[] = "secentry";
static char subsec[] = "subsecentry";
static char subsubsec[] = "subsubsecentry";

#define UL "<MENU>"
#define ul "</MENU>"

static char *levels[] = {
  chap,
  sec,
  subsec,
  subsubsec,
  NULL
};

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
char chap_file[100];

int main(int argc,char *argv[])
{
  int level;
  char *name,*number,*name_end,*number_end,*chap_end;
  FILE *fi,*fo;
  if (argc > 2)
  {
    fo = fopen(argv[2],"w");
    if (!fo) return -3;
  } else fo = stdout;
  if (argc > 1)
  {
    fi = fopen(argv[1],"r");
    if (!fi) return -2;
  }
  else fi = stdin;
  while (fgets(buffer,999,fi))
  {
    level = get_level(buffer);
    if (level < 0) return -1;
    while (level > current_level)
    {
      fputs(UL"\n",fo);
      current_level++;
    }
    while (current_level > level)
    {
      fputs(ul"\n",fo);
      current_level--;
    }
    name = strchr(buffer,'{');
    name++;
    name_end = strchr(name,'}');
    number = strchr(name,'{');
    number++;
    chap_end = strchr(number,'.');
    *chap_end = 0;
    strcpy(chap_file,"chap");
    strcat(chap_file,number);
    strcat(chap_file,".html");
    *chap_end = '.';
    number_end = strchr(number,'}');
    *name_end = 0;
    *number_end = 0;
    fprintf(fo,"<LI> <A HREF=\"%s#%s\">%s %s</A>\n",chap_file,name,number,name);
  }
  while (current_level >= 0)
  {
    fputs(ul,fo);
    current_level--;
  }
  if (argc > 2) fclose(fo);
  if (argc > 1) fclose(fi);
  return 0;
}
