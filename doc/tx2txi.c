/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

static char **directories = NULL;
static int dir_count = 0;

static void
add_directory(char *dir)
{
  dir_count++;
  directories = (char **) realloc(directories, dir_count * sizeof(char *));

  directories[dir_count - 1] = strdup(dir);
}

static char *
find_file(char *name, FILE ** f)
{
  static char tmp[PATH_MAX];
  int i;

  strcpy(tmp, name);
  *f = fopen(tmp, "r");
  if (*f)
    return tmp;
  for (i = 0; i < dir_count; i++)
  {
    strcpy(tmp, directories[i]);
    strcat(tmp, "/");
    strcat(tmp, name);
    *f = fopen(tmp, "r");
    if (*f)
      return tmp;
  }
  *f = NULL;
  return NULL;
}

static int line_count = 0;
static char **lines = NULL;

struct Node
{
  int level;
  char *node_name;
  char *name;
  char *level_string;
  struct Node *next;
  struct Node *prev;
  struct Node *up;
  struct Node *down;
  int line_count;
  char **lines;
};

static char *Line = NULL;
static int line_size = 0;
static struct Node *current_node = NULL;
static struct Node *last_node = NULL;
static struct Node *Top = NULL;
static FILE *inf, *outf;

static void
readln()
{
  int cur_pos = 0;
  if (cur_pos >= line_size)
  {
    line_size += 1024;
    Line = (char *) realloc(Line, line_size);
  }
  Line[0] = 0;
  while (!feof(inf))
  {
    if (cur_pos >= line_size)
    {
      line_size += 1024;
      Line = (char *) realloc(Line, line_size);
    }
    switch (Line[cur_pos] = getc(inf))
    {
      case '\n':
        Line[cur_pos] = 0;
        return;
      case '\r':
        continue;
      case -1:
        Line[cur_pos] = 0;
        return;
      default:
        cur_pos++;
    }
  }
  if (cur_pos >= line_size)
  {
    line_size += 1024;
    Line = (char *) realloc(Line, line_size);
  }
  Line[cur_pos] = 0;
}

static struct Node *
NewNode()
{
  struct Node *n;
  n = (struct Node *) malloc(sizeof(struct Node));
  memset(n, 0, sizeof(struct Node));

  return n;
}

static void
InsertCurrentNode()
{
  if (!Top)
  {
    Top = current_node;
    last_node = current_node;
    current_node = NULL;
    return;
  }
  if (last_node->level == current_node->level)
  {
    current_node->prev = last_node;
    current_node->up = last_node->up;
    last_node->next = current_node;
    last_node = current_node;
    current_node = NULL;
    return;
  }
  if (last_node->level < current_node->level)
  {
    current_node->up = last_node;
    current_node->prev = last_node;
    last_node->down = current_node;
    last_node = current_node;
    current_node = NULL;
    return;
  }
  while (last_node->level > current_node->level)
    last_node = last_node->up;
  last_node->next = current_node;
  current_node->prev = last_node;
  current_node->up = last_node->up;
  last_node = current_node;
  current_node = NULL;
}

static void
ScanNodeLine()
{
  char *tmp = Line + 5;
  char *end;

  while (*tmp && *tmp == ' ')
    tmp++;
  end = tmp;
  while (*end && *end != ',')
    end++;
  *end = 0;
  if (end != tmp)
    current_node->node_name = strdup(tmp);
  current_node->level = 1000;
}

static void
GetName(struct Node *n)
{
  char *tmp = strchr(Line, ' '), *end, c;
  char *_name = tmp;

  while (*tmp == ' ')
    tmp++;
  end = tmp;
  while (*end && *end != ' ')
    end++;
  c = *end;
  n->name = strdup(tmp);
  *end = c;
  *_name = 0;
  n->level_string = strdup(Line);
  *_name = ' ';
}

static int
GetLevel()
{
#define s(x,l) if (strncmp(Line,"@"#x,sizeof(#x)) == 0) return l
  s(chapter, 1);
  s(section, 2);
  s(subsection, 3);
  s(subsubsection, 4);
  s(top, 0);
  s(unnumbered, 1);
  s(unnumberedsec, 2);
  s(unnumberedsubsec, 3);
  s(unnumberedsubsubsec, 4);
  s(appendix, 1);
  s(appendixsec, 2);
  s(appendixsubsec, 3);
  s(appendixsubsubsec, 4);
#if 0
  s(majorheading, 0);
  s(chapheading, 1);
  s(heading, 2);
  s(subheading, 3);
  s(subsubheading, 4);
#endif
#undef s
  return -1;
}

static void
start_node()
{
  if (current_node)
    InsertCurrentNode();
  current_node = NewNode();
  if (strncmp(Line, "@node", 5) == 0)
  {
    ScanNodeLine();
    readln(inf);
    if (GetLevel() == -1)
    {
      current_node->line_count++;
      current_node->lines =
        (char **) realloc(current_node->lines,
                          current_node->line_count * sizeof(char *));

      current_node->lines[current_node->line_count - 1] = strdup(Line);
      return;
    }
  }
  current_node->level = GetLevel();
  GetName(current_node);
}

static int in_menu = 0;

static void
handle_line()
{
  if (in_menu)
  {
    if (strncmp(Line, "@end menu", 9) == 0)
      in_menu = 0;
    return;
  }
  if (strncmp(Line, "@menu", 5) == 0)
  {
    in_menu = 1;
    return;
  }
  if (strncmp(Line, "@node", 5) == 0 || GetLevel() >= 0)
  {
    start_node();
    return;
  }
  if (current_node)
  {
    current_node->line_count++;
    current_node->lines =
      (char **) realloc(current_node->lines,

                        current_node->line_count * sizeof(char *));
    current_node->lines[current_node->line_count - 1] = strdup(Line);
    return;
  }
  line_count++;
  lines = (char **) realloc(lines, line_count * sizeof(char *));

  lines[line_count - 1] = strdup(Line);
}

static void
scan_file()
{
  while (!feof(inf))
  {
    readln();
    if (strncmp(Line, "@include {", 10) == 0)
    {
      char *fstart = Line + 10, *fend;

      fend = fstart;
      while (*fend && *fend != '}')
        fend++;
      if (*fend)
      {
        FILE *ifile = inf;

        *fend = 0;
        find_file(fstart, &inf);
        if (inf != NULL)
        {
          scan_file();
          fclose(inf);
        }
        inf = ifile;
        continue;
      }
    }
    handle_line();
  }
  if (current_node)
    InsertCurrentNode();
}

#define nodename(n) (n->node_name?n->node_name:n->name)

static void
print_node_line(struct Node *node)
{
  fprintf(outf, "@node %s, %s, %s, %s\n",
          nodename(node),
          node->next ? nodename(node->next) : " ",
          node->prev ? nodename(node->prev) : " ",
          node->up ? nodename(node->up) : " ");
  if (node->level_string)
    fprintf(outf, "%s %s\n", node->level_string, node->name);
}

static void
print_node_contents(struct Node *n)
{
  int i;

  for (i = 0; i < n->line_count; i++)
  {
    fprintf(outf, "%s\n", n->lines[i]);
  }
}

static void
output_menu(struct Node *_n)
{
  struct Node *n = _n->down;

  fprintf(outf, "@menu\n");
  do
  {
    fprintf(outf, "* %s::\n", nodename(n));
    n = n->next;
  }
  while (n);
  fprintf(outf, "@end menu\n\n");
}


static void
output_node(struct Node *n)
{
  print_node_line(n);
  print_node_contents(n);
  if (n->down)
  {
    output_menu(n);
    output_node(n->down);
  }
  if (n->next)
    output_node(n->next);
}

static void
output_file()
{
  int i;

  for (i = 0; i < line_count; i++)
    fprintf(outf, "%s\n", lines[i]);
  output_node(Top);
}


int
main(int argc, char *argv[])
{
  struct Node *dir;
  char *real_name;

  while (argc > 3)
  {
    argc--;
    if (strcmp(argv[1], "-I") == 0)
    {
      argc--;
      argv++;
      add_directory(argv[1]);
    }
    argv++;
  }
  real_name = find_file(argv[1], &inf);
  if (!inf)
    return -1;
  scan_file();
  fclose(inf);
  if (argc < 3)
    outf = stdout;
  else
    outf = fopen(argv[2], "w");
  dir = NewNode();
  dir->name = strdup("(dir)");
  Top->prev = dir;
  Top->up = dir;
  output_file();
  fclose(outf);
  return 0;
}
