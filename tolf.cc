#include <stdio.h>
#include <sys/stat.h>
#include <utime.h>
#include <rhutils.h>

#ifdef TOCRLF
#define LFSTRING "\r\n"
#else
#define LFSTRING "\n"
#endif

static
int convert_file(const char *arg)
{
  char *fname = string_dup(arg);
  FExpand(fname);
  char *tmp_name = string_dup(fname);
  split_fname_fmt(fname, "%D", tmp_name);
  string_cat(tmp_name, "_tmp_");
  struct stat st;
  if (stat(fname, &st) != 0)
  {
    string_free(fname);
    string_free(tmp_name);
    perror(arg);
    return -1;
  }
  struct utimbuf ut;
  ut.actime = st.st_atime;
  ut.modtime = st.st_mtime;
  FILE *fi = fopen(fname, "rt");
  FILE *fo = fopen(tmp_name, "w+b");
  char *line = NULL;
  int line_size = 0;
  while (getline(line, line_size, fi) != -1)
  {
    fprintf(fo, "%s" LFSTRING, line);
  }
  fclose(fo);
  fclose(fi);
  remove(fname);
  if (rename(tmp_name, fname) != 0)
  {
    perror("rename");
    return -3;
  }
  utime(fname, &ut);
  chmod(fname, st.st_mode);
  string_free(fname);
  string_free(tmp_name);
  return 0;
}

int main(int argc, char **argv)
{
  for ( ; argc > 1; argc--, argv++)
  {
    int ret = convert_file(argv[1]);
    if (ret != 0)
      return ret;
  }
  return 0;
}

