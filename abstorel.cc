/* Copyright (C) 1996,1997 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/*
 $Id$
*/
#include <rhide.h>
#include <rhutils.h>
#include <string.h>

/*
  This functions makes ret_path relative to ref_path
  It assumes that ref_path is an absolute path which can
  optionally have a slash appended
*/

void AbsToRelPath(char *ref_path,char *& ret_path)
{
  char *ref = ref_path;
  char *ret;
  char *ret_val;
  // Just in case, that ret_pat is not absolute, make it now absolute
  // relative to the current directory
  if (!ref_path || !*ref_path) return;
#if 0
/* I hope, I can assume that this is called only with an absolute
   name ret_path */
  FExpand(ret_path);
#endif
  ret = ret_path;
  while (*ret && *ret == *ref)
  {
    ret++;
    ref++;
  }
  // total different
  if (ret == ret_path)
    return;
  // ref is part of ret
  if (!*ref)
  {
    // they are equal (can happen only when comparing directories)
    if (!*ret)
    {
      string_free(ret_path);
      string_dup(ret_path,"");
      return;
    }
    // ref is completely a directory part of ret
    if (*ret == '/' || ref[-1] == '/' /* ref had a slash as last char */)
    {
      // do no copy the slash
      if (*ret == '/') ret++;
      string_dup(ret_val,ret);
      string_free(ret_path);
      ret_path = ret_val;
      return;
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
  // ret is part of ref
  if (!*ret)
  {
    // ret is completely a directoy part of ref
    if (*ref == '/')
    {
      ref++;
      string_dup(ret_val,"..");
      while ((ref = strchr(ref,'/')) != NULL)
      {
        ref++;
        string_cat(ret_val,"/..");
      }
      string_free(ret_path);
      ret_path = ret_val;
      return;
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
  string_dup(ret_val,"..");
  while ((ref = strchr(ref,'/')) != NULL)
  {
    string_cat(ret_val,"/..");
    ref++;
  }
  if (*ret)
  {
    string_cat(ret_val,"/");
    string_cat(ret_val,ret);
  }
  string_free(ret_path);
  ret_path = ret_val;
}


#ifdef TEST

int main(int argc,char *argv[])
{
  char *ref;
  char *ret;
  if (argc < 3) return -1;
  string_dup(ref,argv[1]);
  string_dup(ret,argv[2]);
  printf("%s relative to %s is ",ret,ref);
  AbsToRelPath(ref,ret);
  printf("%s\n",ret);
  return 0;
}

#endif

