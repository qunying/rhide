/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int chapter = 0;
char chap_file[100];
char line[1000];
char line1[1000];
char **start_of_file = NULL;
int start_lines = 0;
int start_ended = 0;
FILE *fi,*fo;
char *base;

#define HTML "html"

int start_new_file()
{
  int i;
  fprintf(fo,"<p><hr><p>\n");
  fprintf(fo,"<a href=%s0."HTML">Start of doc</a> | ",base);
  fprintf(fo,"<a href=%s%d."HTML">Next chapter</a>",base,chapter+1);
  if (chapter>0)
    fprintf(fo," | <a href=%s%d."HTML">Previous chapter</a>",base,chapter-1);
  fprintf(fo,"\n</body>\n</html>\n");
  fclose(fo);
  chapter++;
  sprintf(chap_file,"%s%d.html",base,chapter);
  fo = fopen(chap_file,"w");
  if (!fo) return -2;
  for (i=0;i<start_lines;i++)
  {
    fprintf(fo,"%s",start_of_file[i]);
  }
  return 0;
}

void add_to_start()
{
  start_lines++;
  start_of_file = (char **)realloc(start_of_file,sizeof(char *)*start_lines);
  start_of_file[start_lines-1] = strdup(line);
}

int main(int argc,char *argv[])
{
  if (argc > 1)
  {
    fi = fopen(argv[1],"r");
    if (!fi) return -2;
    argv++;
  } else fi = stdin;
  base = argv[1];
  sprintf(chap_file,"%s%d.html",base,chapter);
  fo = fopen(chap_file,"w");
  while (fgets(line,999,fi))
  {
    if (!start_ended)
    {
      if (strncmp(line,"<body>",6) == 0)
      {
        add_to_start();
        start_ended = 1;
        fprintf(fo,"%s",line);
        continue;
      }
      add_to_start();
      fprintf(fo,"%s",line);
      continue;
    }
    if (strncmp(line,"<A NAME=",8) == 0)
    {
      fgets(line1,999,fi);
      if (strncmp(line1,"<H2> ",5) == 0)
      {
        char *tmp = strchr(line1,'.');
        if (tmp[1] == ' ')
        {
          int ret = start_new_file();
          if (ret != 0) return ret;
        }
      }
      fprintf(fo,"%s",line1);
    }
    fprintf(fo,"%s",line);
  }
  fprintf(fo,"<p><hr><p>\n");
  fprintf(fo,"<a href=%s0."HTML">Start of doc</a> | ",base);
  fprintf(fo,"<a href=%s%d."HTML">Previous chapter</a>\n",base,chapter-1);
  fclose(fo);
  if (argc > 1) fclose(fi);
  return 0;
}

