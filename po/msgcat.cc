/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/* This program can be used to translate the strings in a more friendly
   way. It knows about the special char '~' in keywords, so there is
   no necessary at all all, to retranslate the string, if the hotkey
   in the key has changed. It can read also any previous translated
   files to get possible translations from those files.
   It uses the conion functions for highlighting text on the screen and
   it may work (bether the color highlight may work) only on graphic cards
   wich can show correct the textcolor(7) and textbackground(7) colors.

   The program uses the great 'fstrcmp()' function from the gettext lib,
   which does a 'fuzzy' seach for finding a similar string.

   The usage is:

   msgcat [-l] [-s] [--only-similar] [-cat cat-file ] [-ocat cat-file]
          oldest-file any-newer-file potfile output-file

   where old-file(s) are the files, with existing
   translations in descending order by time, because if a file contains
   a translation, which was already found, the new one is taken.

   The file cat-file can be used to read from and to store in all the
   translations. This file can contain also translations, which are
   currently not used by the potfile.

   If -ocat is given, the cat-file specified there is used as output
   cat file instead of (if given) the cat-file from the -cat option.

   if -s was given, the program does not prompt for translating, it
   translates silently all exact msgid's and the others not.

   if -l was given, all the comments (which include for instance
   the line numbers of the input-files) are written in the output.

   if --only-similar is given, the program prompst only for strings,
   which are found to be similar.

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <rhutils.h>
#ifdef __DJGPP__
#include <conio.h>

static void
NORM()
{
  textcolor(7);
  textbackground(0);
}

static void
LIGHT()
{
  textcolor(0);
  textbackground(7);
}

static void
CLEAR_SCREEN()
{
  clrscr();
}

#define CRLF "\r\n"
#define CR "\r"

#else

#include <curses.h>

static void
NORM()
{
#if 1
//  attr_set(A_NORMAL);
#endif
}

static void
LIGHT()
{
#if 1
//  attr_set(A_REVERSE);
#endif
}

static void
CLEAR_SCREEN()
{
}

#define CRLF "\n"
#define CR "\n"

#define cscanf scanw
#define cprintf printw
#define cputs(x) printw("%s", x)

#include <unistd.h>

static int
__file_exists(const char *fname)
{
  return (access(fname, R_OK) == 0);
}

#endif

#include <unistd.h>

extern "C" double fstrcmp(const char *, const char *);

struct msg
{
  char *id;
  char *id_;
  char *Msg;
  char *comment;
};

struct msg *msgs = NULL;
int msg_count = 0;
static int silent = 0;
static int only_similar = 0;
static int write_all = 0;
static char MSG[4000];
static char ID[4000];
static FILE *fi, *fo;
static char LINE[4000];
static char line[4000];
static int line_count = 0;
static char *COMMENT = NULL;
static char *SOURCE_FILE = NULL;
static int SOURCE_LINE;
static char *base_dir = ".";

static char *last_fname = NULL;
static char **last_source = NULL;
static int last_lines_count = 0;
static int last_lines_size = 0;

#define line_diff 5

static int
read_line(char *buf, int max_size, FILE * f)
{
  char *p = buf;

  max_size--;
  while (!feof(f))
  {
    *p = fgetc(f);
    if (*p == '\n')
    {
      p++;
      *p = 0;
      return 1;
    }
    if (p - buf < max_size)
      p++;
  }
  *p++ = '\n';
  *p = 0;
  return 0;
}

static int
read_source_file(char *fname)
{
  if (!fname)
    return 0;
  if (last_fname && (strcmp(last_fname, fname) == 0))
    return 1;
  string_free(last_fname);
  last_fname = string_dup(fname);
  int i;

  for (i = 0; i < last_lines_count; i++)
  {
    string_free(last_source[i]);
  }
  last_lines_count = 0;
  char *_fname = NULL;

  if (__file_exists(fname))
    string_cat(_fname, fname);
  else
    string_cat(_fname, base_dir, "/", fname, NULL);
  FILE *f = fopen(_fname, "rt");

  string_free(_fname);
  if (!f)
    return 0;
  char buf[80];

  while (read_line(buf, 79, f))
  {
    if (last_lines_count == last_lines_size)
    {
      last_lines_size += 128;
      last_source =

        (char **) realloc(last_source, last_lines_size * sizeof(char *));
    }
    last_source[last_lines_count++] = string_dup(buf);
  }
  fclose(f);
  return 1;
}

static void
write_source_lines(char *fname, int line_no)
{
  if (!read_source_file(fname))
    return;
  int start_line = line_no - 1 - line_diff;

  if (start_line < 0)
    start_line = 0;
  int end_line = line_no + line_diff;

  if (end_line >= last_lines_count)
    end_line = last_lines_count;
  CLEAR_SCREEN();
  LIGHT();
  cprintf("Translating from file '%s' line %d\r\n", fname, line_no);
  NORM();
  while (start_line <= end_line)
  {
    if (start_line + 1 == line_no)
      LIGHT();
    else
      NORM();
    cputs(last_source[start_line]);
    cputs("\r");
    start_line++;
  }
}

/* This function returns the string without any '~', because this is a
   special case in TV programs, so if I change the hotkey for a string
   there is no need to retranslate the string.
*/
char *
similar(char *msg)
{
  char *buffer;
  char *t1, *t2;

  buffer = string_dup(msg);
  t1 = strchr(buffer, '~');
  while (t1)
  {
    t2 = strchr(t1 + 1, '~');
    if (!t2)
      break;
    strcpy(t1, t1 + 1);
    strcpy(t2 - 1, t2);
    t1 = strchr(buffer, '~');
  }
  return buffer;
}

static int
cmp_msg(const void *m1, const void *m2)
{
  return strcmp(((struct msg *) m1)->id, ((struct msg *) m2)->id);
}

static void
sort_msgs()
{
  qsort(msgs, msg_count, sizeof(struct msg), cmp_msg);
}

static struct msg *
find_msg(char *id)
{
  int i;

  for (i = 0; i < msg_count; i++)
  {
    if (strcmp(msgs[i].id, id) == 0)
      return &msgs[i];
  }
  return NULL;
}

void
add_msg(char *id, char *msg)
{
  struct msg *_msg = find_msg(id);

  if (_msg)
  {
    string_free(_msg->Msg);
    _msg->Msg = string_dup(msg);
    string_free(_msg->comment);
    _msg->comment = COMMENT ? string_dup(COMMENT) : NULL;
    return;
  }
  msgs = (struct msg *) realloc(msgs, sizeof(struct msg) * (msg_count + 1));

  msgs[msg_count].id = string_dup(id);
  msgs[msg_count].Msg = string_dup(msg);
  msgs[msg_count].id_ = similar(id);
  msgs[msg_count].comment = COMMENT ? string_dup(COMMENT) : NULL;
  msg_count++;
}

static char *
read_line()
{
  line_count++;
  return fgets(LINE, 4000, fi);
}

int
read_id(int write_it)
{
  int started = 0;
  int file_seen = 0;
  char *start, *end;

  ID[0] = 0;
  string_free(COMMENT);
  string_free(SOURCE_FILE);
  while (read_line())
  {
    if (!started && strncmp(LINE, "msgid ", 6) != 0)
    {
      if (write_it && write_all)
        fprintf(fo, "%s", LINE);
      if (strncmp(LINE, "# ", 2) == 0)
      {
        string_cat(COMMENT, LINE);
      }
      if (!file_seen && strncmp(LINE, "#: ", 3) == 0)
      {
        file_seen = 1;
        char *a = LINE + 3;
        char *b = a;

        while (*b)
        {
          if ((*b == ':') && isdigit(b[1]))
            break;
          b++;
        }
        *b = 0;
        SOURCE_FILE = string_dup(a);
        b++;
        sscanf(b, "%d", &SOURCE_LINE);
      }
      continue;
    }
    if (started && LINE[0] != '\"')
      return 1;
    if (!started && write_it)
      fprintf(fo, "msgid  ");
    start = strchr(LINE, '\"');
    if (!start)
      return 0;
    start++;
    end = strrchr(LINE, '\"');
    if (!end)
      return 0;
    *end = 0;
    if (write_it)
      fprintf(fo, "\"%s\"\n", start);
    strcat(ID, start);
    started = 1;
  }
  return 0;
}

int
read_msg()
{
  int started = 0;
  char *start, *end;
  char *xline;

  MSG[0] = 0;
  do
  {
    xline = LINE;
    /*
       Cut spaces on the beginning 
     */
    while (*xline == ' ' || *xline == '\t')
      xline++;

/* fprintf(stderr,"%s\n",LINE); */
    if (!started && (strncmp(xline, "msgstr ", 7) != 0))
    {
      fprintf(stderr, "Error: No 'msgstr' keyword found. (line %d)\n",
              line_count);
    }
    if (started && xline[0] != '\"')
      return 1;
    started = 1;
    start = strchr(xline, '\"') + 1;
    end = strrchr(xline, '\"');
    *end = 0;
    strcat(MSG, start);
  }
  while (read_line());
  return 0;
}

static void
put_breakline(FILE * f, int start_len, int max_len, char *s)
{
  int len;

  while (1)
  {
    char *tmp;

    len = strlen(s);
    if (len + start_len <= max_len)
    {
      fprintf(f, "\"%s\"\n", s);
      return;
    }
    tmp = s + max_len - start_len;
    while (tmp > s && *tmp != ' ')
      tmp--;
    if (tmp == s)
    {
      fprintf(f, "\"%s\"\n", s);
      return;
    }
    *tmp = 0;
    fprintf(f, "\"%s \"\n", s);
    *tmp = ' ';
    start_len = 0;
    s = tmp + 1;
  }
}

void
write_id(char *msg, char *comment)
{
  char *tmp;
  char *start = msg;
  int started = 7;

  if (comment)
    fprintf(fo, "%s\n", comment);
  while ((tmp = strstr(start, "\\n")) != NULL)
  {
    char c = tmp[2];

    tmp += 2;
    *tmp = 0;
    if (started)
      fprintf(fo, "msgid  ");
    put_breakline(fo, started, 75, start);
    started = 0;
    *tmp = c;
    start = tmp;
  }
  if (started)
    fprintf(fo, "msgid  ");
  if (started || *start)
    put_breakline(fo, started, 75, start);
  fflush(fo);
}

void
write_msg(char *msg)
{
  char *tmp;
  char *start = msg;
  int started = 7;

  while ((tmp = strstr(start, "\\n")) != NULL)
  {
    char c = tmp[2];

    tmp += 2;
    *tmp = 0;
    if (started)
      fprintf(fo, "msgstr ");
    put_breakline(fo, started, 75, start);
    started = 0;
    *tmp = c;
    start = tmp;
  }
  if (started)
    fprintf(fo, "msgstr ");
  if (started || *start)
    put_breakline(fo, started, 75, start);
  fprintf(fo, "\n");
  fflush(fo);
}

static void
convert(char *s)
{
  char *b;

  while ((b = strchr(s, '\b')) != NULL)
  {
    strcpy(b - 1, b + 1);
  }
}

/*
  return >= 0 normal
           -1 end
           -2 quit
           -3 empty
*/

int
fuzzy_search(char *id, char *idd)
{
  int i;
  char c;
  double best_weight;
  int best = -1;
  double weight;

  best_weight = 0.6;
  for (i = 0; i < msg_count; i++)
  {
    weight = fstrcmp(id, msgs[i].id_);
    if (weight > best_weight)
    {
      best_weight = weight;
      best = i;
    }
  }
  while (best == -1)
  {
    if (only_similar)
      return -3;
    NORM();
    cprintf("Please translate:" CRLF "  ");
    LIGHT();
    cprintf("%s", idd);
    NORM();
    cprintf(CRLF "Enter the string: ");
    LIGHT();
    line[0] = 0;
    cscanf("%[^" CR "]", line);
    convert(line);
    NORM();
    cprintf("\nAccept ? (y)es/(n)o/(c)ancel/(e)nd/(q)uit ");
    c = tolower(getch());
    do
    {
      c = tolower(getch());
      switch (c)
      {
        case 'q':
          return -2;
        case 'e':
          return -1;
      }
    }
    while (c != 'y' && c != 'n' && c != 'c');
    NORM();
    cprintf("%c" CRLF, c);
    if (c == 'y')
    {
      add_msg(idd, line);
      return msg_count - 1;
    }
    if (c == 'c')
      return -3;
  }
  NORM();
  cprintf("translate:" CRLF "  ");
  LIGHT();
  cprintf("%s", idd);
  NORM();
  cprintf(CRLF "from" CRLF "  ");
  LIGHT();
  cprintf("%s", msgs[best].id);
  NORM();
  cprintf(CRLF "to" CRLF "             ");
  LIGHT();
  cprintf("%s", msgs[best].Msg);
  NORM();
  cprintf(CRLF "??? (y)es/(n)o/(c)hange/(e)nd/(q)uit ");
  do
  {
    c = tolower(getch());
    switch (c)
    {
      case 'q':
        return -2;
      case 'e':
        return -1;
    }
  }
  while (c != 'y' && c != 'n' && c != 'c');
  NORM();
  cprintf("%c" CRLF, c);
  if (c == 'y')
  {
    strcpy(MSG, msgs[best].Msg);
    strcpy(MSG + strlen(MSG), "\\0# FUZZY");
    add_msg(idd, MSG);
    return msg_count - 1;
/*  return best; */
  }
  while (c == 'c')
  {
    NORM();
    cprintf("Translation: ");
    LIGHT();
    line[0] = 0;
    cscanf("%[^" CR "]", line);
    convert(line);
    NORM();
    c = tolower(getch());
    cprintf("\nAccept ? (y)es/(n)o/(c)hange/(e)nd/(q)uit ");
    do
    {
      c = tolower(getch());
      switch (c)
      {
        case 'q':
          return -2;
        case 'e':
          return -1;
      }
    }
    while (c != 'y' && c != 'n' && c != 'c');
    NORM();
    cprintf("%c" CRLF, c);
  }
  if (c == 'n')
    return -3;
  add_msg(idd, line);
  return msg_count - 1;
}

/*
  return 0 for normal
         1 for end
         2 for quit
*/

int
translate()
{
  int i;
  char *id_;

/*  if (ID[0] == 0)
    {
      write_msg("");
      return;
    } */
  for (i = 0; i < msg_count; i++)
  {
    if (strcmp(msgs[i].id, ID) == 0)
    {
      write_id(msgs[i].id, msgs[i].comment);
      write_msg(msgs[i].Msg);
      return 0;
    }
  }
  if (silent)
  {
    write_id(ID, NULL);
    write_msg("");
    return 0;
  }
  id_ = similar(ID);
  for (i = 0; i < msg_count; i++)
  {
    if (strcmp(msgs[i].id_, id_) == 0)	// do it not silently
    {
      char c;

      write_source_lines(SOURCE_FILE, SOURCE_LINE);
      NORM();
      cprintf("translate:" CRLF "  ");
      LIGHT();
      cprintf("%s", ID);
      NORM();
      cprintf(CRLF "from" CRLF "  ");
      LIGHT();
      cprintf("%s", msgs[i].id);
      NORM();
      cprintf(CRLF "to" CRLF "             ");
      LIGHT();
      cprintf("%s", msgs[i].Msg);
      NORM();
      cprintf(CRLF "??? (y)es/(n)o/(c)hange/(e)nd/(q)uit ");
      do
      {
        c = tolower(getch());
        switch (c)
        {
          case 'q':
            return 2;
          case 'e':
            return 1;
        }
      }
      while (c != 'y' && c != 'n' && c != 'c');
      NORM();
      cprintf("%c" CRLF, c);
      if (c == 'y')
      {
        add_msg(ID, msgs[i].Msg);
        write_msg(msgs[i].Msg);
        return 0;
      }
      while (c == 'c')
      {
        NORM();
        cprintf("Translation: ");
        LIGHT();
        line[0] = 0;
        cscanf("%[^" CR "]", line);
        convert(line);
        NORM();
        c = tolower(getch());
        cprintf("\nAccept ? (y)es/(n)o/(c)hange/(e)nd/(q)uit ");
        do
        {
          c = tolower(getch());
          switch (c)
          {
            case 'q':
              return 2;
            case 'e':
              return 1;
          }
        }
        while (c != 'y' && c != 'n' && c != 'c');
        NORM();
        cprintf("%c" CRLF, c);
      }
      if (c != 'n')
      {
        add_msg(ID, line);
        write_msg(line);
        return 0;
      }
    }
  }
  write_source_lines(SOURCE_FILE, SOURCE_LINE);
  i = fuzzy_search(id_, ID);
  write_id(ID, NULL);
  if (i > -1)
    write_msg(msgs[i].Msg);
  else
    write_msg("");
  if (i == -1)
    return 1;
  if (i == -2)
    return 2;
  return 0;
}

char **potable;
int PotableItems;
int
main(int argc, char *argv[])
{
  char *catfile;
  char *ocatfile;
  char *outputfile;
  char *potfile;

  int i;

#ifdef __linux__
  initscr();
#endif

  catfile = NULL;
  ocatfile = NULL;
  outputfile = NULL;
  potfile = NULL;


  potable = (char **) malloc(sizeof(char *));

  PotableItems = 0;
  *potable = NULL;

/* Parse Arguments */
  while (argc > 1)
  {
    if (strcmp(argv[1], "-l") == 0)
    {
      write_all = 1;
      argc--;
      argv++;
      continue;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-?") == 0)
    {
      printf("\n>>>>msgcat<<<\n"
             "This program can be used to translate the strings in a more friendly\n"
             "way. It knows about the special char '~' in keywords, so there is\n"
             "no necessary at all all, to retranslate the string, if the hotkey\n"
             "in the key has changed. It can read also any previous translated\n"
             "files to get possible translations from those files.\n"
             "\n   The usage is:\n"
             " msgcat [-l] [-s] [--only-similar] [-cat cat-file ] [-ocat cat-file]\n"
             "oldest-file any-newer-file potfile output-file\n"
             "\n"
             "where old-file(s) are the files, with existing translations in descending\n"
             "   order by time, because if a file contains a translation,\n"
             "   which was already found, the new one is taken.\n"
             "The file cat-file can be used to read from and to store in all the\n"
             "   translations. This file can contain also translations, which are\n"
             "   currently not used by the potfile.");
      return (0);
    }

    if (strcmp(argv[1], "--only-similar") == 0)
    {
      only_similar = 1;
      argc--;
      argv++;
      continue;
    }

    if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--silent") == 0)
    {
      silent = 1;
      argc--;
      argv++;
      continue;
    }

    if (argc > 1 && strcmp(argv[1], "-directory") == 0)
    {
      base_dir = argv[2];
      argc -= 2;
      argv += 2;
      continue;
    }

    if (argc > 1 && strcmp(argv[1], "-cat") == 0)
    {
      catfile = argv[2];
      ocatfile = argv[2];
      argc -= 2;
      argv += 2;
      continue;
    }

    if (argc > 1 && strcmp(argv[1], "-ocat") == 0)
    {
      ocatfile = argv[2];
      argc -= 2;
      argv += 2;
      continue;
    }

    if (strcmp(argv[1], "-o") == 0)
    {
      outputfile = argv[2];
      argc -= 2;
      argv += 2;
      continue;
    }

    if (strcmp(argv[1], "-po") == 0)
    {
      potfile = argv[2];
      argc -= 2;
      argv += 2;
      continue;
    }

    if (argv[1])
    {
      potable =

        (char **) realloc(potable, (PotableItems + 1) * sizeof(char *));
      potable[PotableItems] = argv[1];
      PotableItems++;
    }
    argc--;
    argv++;
  }
/* End of reading arguments */


  if (PotableItems < 2 && outputfile == NULL)
  {
    puts("Error: The output file is unknown!");
    return (-1);
  }
  if (outputfile == NULL)
  {
    PotableItems--;
    outputfile = potable[PotableItems];
  }

  if (PotableItems < 2 && potfile == NULL)
  {
    puts("Error: The pot file is unknown!");
    return (-2);
  }
  if (potfile == NULL)
  {
    PotableItems--;
    potfile = potable[PotableItems];
  }

  /*
     Read the catfile first, before the .po files 
   */
  if (catfile && __file_exists(catfile))
  {
    fi = fopen(catfile, "r");
    line_count = 0;
    while (read_id(0))
    {
      read_msg();
      if (strlen(MSG) > 0)
        add_msg(ID, MSG);
    }
    fclose(fi);
  }

/* Read the .po files */
  for (i = 0; i < PotableItems; i++)
  {
    if ((fi = fopen(potable[i], "r")) == NULL)
    {
      printf("Error: Cannot open a file %s\n!", potable[i]);
      continue;
    }

    line_count = 0;
    while (read_id(0))
    {
      read_msg();
      if (strlen(MSG) > 0)
        add_msg(ID, MSG);
    }
    fclose(fi);
  }

/* Write output po file to the disk */
  char *wd = getcwd(NULL, 100);

  printf("%s\n", wd);
  if ((fi = fopen(potfile, "r")) == NULL)
  {
    perror(potfile);
    printf("Error: Cannot open pot file!");
    return (-3);
  }
  char _outputfile[9];

  strcpy(_outputfile, "poXXXXXX");
  int fd = mkstemp(_outputfile);
  if (fd != -1)
    close(fd);
  if ((fo = fopen(_outputfile, "w")) == NULL)
  {
    printf("Error: Cannot open temp. output file!");
    return (-3);
  }
  line_count = 0;
  struct msg *_msg = find_msg("");

  if (_msg)
  {
    write_id(_msg->id, _msg->comment);
    write_msg(_msg->Msg);
  }
  int trans_ret = 0;

  while (read_id(0))
  {
    read_msg();
    if ((trans_ret = translate()) != 0)
      break;
  }
  if (trans_ret == 1)
  {
    silent = 1;
    while (read_id(0))
    {
      read_msg();
      translate();
    }
  }
  fclose(fi);
  fclose(fo);
  if (trans_ret != 2)
  {
    unlink(outputfile);
    rename(_outputfile, outputfile);
  }

/* Write output cat file to the disk */
  if (ocatfile)
  {
    sort_msgs();
    fo = fopen(ocatfile, "w+");
    for (i = 0; i < msg_count; i++)
    {
      write_id(msgs[i].id, NULL);
      write_msg(msgs[i].Msg);
    }
    fclose(fo);
  }

#ifdef __linux__
  endwin();
#endif
  return 0;
}
