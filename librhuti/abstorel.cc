/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RHU for details */
#include <rhutils.h>
#include <string.h>

/*
  This functions makes ret_path relative to ref_path
  It assumes that ref_path is an absolute path which can
  optionally have a slash appended
*/

int
AbsToRelPath(const char *ref_path, char *&ret_path, const char *subst,
             int allow_prevdirs, int max_up_count)
{
  const char *ref = ref_path;
  const char *ret;
  char *ret_val;
  int up_count = 0;

  // Just in case, that ret_pat is not absolute, make it now absolute
  // relative to the current directory
  if (!ref_path || !*ref_path)
    return 0;
  ret = ret_path;
  while (*ret && *ret == *ref)
  {
    ret++;
    ref++;
  }
  // total different
  if (ret == ret_path)
    return 0;
  // ref is part of ret
  if (!*ref)
  {
    // they are equal (can happen only when comparing directories)
    if (!*ret)
    {
      string_free(ret_path);
      if (subst)
        string_dup(ret_path, subst);
      else
        string_dup(ret_path, "");
      return 1;
    }
    // ref is completely a directory part of ret
    if (*ret == '/' || ref[-1] == '/')
    // ref had a slash as last char
    {
      // do no copy the slash
      if (*ret == '/')
        ret++;
      if (subst)
      {
        string_dup(ret_val, subst);
        string_cat(ret_val, "/", ret, NULL);
      }
      else
        string_dup(ret_val, ret);
      string_free(ret_path);
      ret_path = ret_val;
      return 1;
    }
    // ref was only a sub_string of ret
    while (*ret != '/')
    {
      ret--;
      ref--;
    }
    ret++;
    ref++;
  }
  if (!allow_prevdirs)
    return 0;
  // ret is part of ref
  if (!*ret)
  {
    // ret is completely a directoy part of ref
    if (*ref == '/')
    {
      ref++;
      string_dup(ret_val, "..");
      up_count++;
      while ((ref = strchr(ref, '/')) != NULL)
      {
        ref++;
        string_cat(ret_val, "/..");
        up_count++;
        if (up_count > max_up_count)
        {
          string_free(ret_val);
          return 0;
        }
      }
      string_free(ret_path);
      ret_path = ret_val;
      return 1;
    }
    // ret was simply a substring of ref
    while (*ret != '/')
    {
      ref--;
      ret--;
    }
    ref++;
    ret++;
  }
  while (*ref != '/' || *ret != '/')
  {
    ref--;
    ret--;
  }
  ref++;
  ret++;
  string_dup(ret_val, "..");
  while ((ref = strchr(ref, '/')) != NULL)
  {
    up_count++;
    if (up_count > max_up_count)
    {
      string_free(ret_val);
      return 0;
    }
    string_cat(ret_val, "/..");
    ref++;
  }
  if (*ret)
  {
    string_cat(ret_val, "/");
    string_cat(ret_val, ret);
  }
  string_free(ret_path);
  ret_path = ret_val;
  return 1;
}


#ifdef TEST

#include <stdio.h>

int
main(int argc, char *argv[])
{
  char *ref;
  char *ret;

  if (argc < 3)
    return -1;
  string_dup(ref, argv[1]);
  string_dup(ret, argv[2]);
  fprintf(stderr, "%s relative to %s is ", ret, ref);
  AbsToRelPath(ref, ret, ret);
  fprintf(stderr, "%s\n", ret);
  return 0;
}

#endif
