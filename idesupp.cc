/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <rhide.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

#include <rhutils.h>

static char *buffer = NULL;

char *ExpandFileNameToThePointWhereTheProgramWasLoaded(const char *s)
{
  string_free(buffer);
  if (__file_exists(s))
  {
    string_dup(buffer,s);
    return buffer;
  }
  char *spec = NULL;
  string_cat(spec,"$(word 1,$(foreach file,$(addsuffix /$(notdir ",s,
                  "),$(RHIDE_CONFIG_DIRS)),$(wildcard $(file))))",NULL);
  buffer = expand_rhide_spec(spec);
  string_free(spec);
  if (*buffer)
    return buffer;
  string_free(buffer);
  string_cat(spec,"$(word 1,$(addsuffix /", s, ",$(RHIDE_CONFIG_DIRS) .))", NULL);
  buffer = expand_rhide_spec(spec);
  string_free(spec);
  if (!*buffer)
    string_cat(buffer, s);
  return buffer;
}

#if 1
#include <dirent.h>
#include <fnmatch.h>
static char PathOrig[PATH_MAX];
static char FileNameToLoad[PATH_MAX];
static char *PathOrigPos;

int edTestForFile(const char *name)
{
 struct stat st;

 if (stat(name,&st)==0)
    return S_ISREG(st.st_mode);
 return 0;
}

#ifndef __DJGPP__
char *GetHome(void)
{
 char *s=getenv("HOME");
 if (!s)
   {
    s=getenv("HOMEDIR");
    if (!s)
       s=".";
   }
 return s;
}

char *ExpandFileNameToUserHome(const char *s)
{
 strcpy(FileNameToLoad,GetHome());
 strcat(FileNameToLoad,"/.");
 strcat(FileNameToLoad,s);
 return FileNameToLoad;
}
#else
// The DOS version is used only by edspecs.cc
char *ExpandFileNameToUserHome(const char *s)
{
 char *p=getenv("SET_FILES");
 if (p)
    strcpy(FileNameToLoad,p);
 else
    FileNameToLoad[0]=0;
 strcat(FileNameToLoad,"/");
 strcat(FileNameToLoad,s);
 return FileNameToLoad;
}
#endif

char *ReplaceExtention(char *name, char *ext, char *old)
{
 char *pos;
 pos=strrchr(name,'/');
 if (!pos)
    pos=name;
 pos=strstr(pos,old);
 if (pos)
    strcpy(pos,ext);
 else
    strcat(name,ext);
 return name;
}

int DeleteWildcard(char *mask)
{
 int deleted=0;
 DIR *d;
 d=opendir(".");
 
 if (d)
   {
    struct dirent *de;
    while ((de=readdir(d))!=0)
      {
       if (!fnmatch(mask,de->d_name,0))
         {
          unlink(de->d_name);
          deleted++;
         }
      }
    closedir(d);
   }
 return deleted;
}


char *GetPathRelativeToRunPoint(char *dest, const char *binReplace, char *file)
{
 char *ret;

 if (PathOrigPos!=NULL)
   {
    *PathOrigPos=0;
    strcpy(dest,PathOrig);
    char *s=strstr(dest,"bin");
    if (!s)
       s=strstr(dest,"BIN");
    if (s)
       strcpy(s,binReplace);
    ret=dest+strlen(dest)-1;
   }
 else
   {
    *dest=0;
    ret=dest;
   }

 strcat(dest,file);

 return ret;
}


void SetReferencePath(char *orig)
{
 PathOrigPos=strrchr(orig,'/');
 if (PathOrigPos)
   {
    PathOrigPos++;
    *PathOrigPos=0;
    strcpy(PathOrig,orig);
    PathOrigPos=PathOrig+(int)(PathOrigPos-orig);
   }
 else
    PathOrig[0]=0;
}

char *RedirectStdErrToATemp(int &StdErrOri,int &StdErrNew)
{
 char aux[PATH_MAX];
 #ifdef __DJGPP__
 char *s=(char *)GetVariable("SET_FILES");
 #else
 // In UNIX the user can't write to the share directory so we must avoid creating
 // such a files
 char *s=GetHome();
 #endif
 char *ret=0;

 sprintf(aux,"%s/erXXXXXX",s);
 StdErrNew=mkstemp(aux);
 if (StdErrNew>0)
   {
    ret=strdup(aux);
    StdErrOri=dup(fileno(stderr));
    fflush(stderr);
    dup2(StdErrNew,fileno(stderr));
   }
 return ret;
}

const int eachRead=16300;

int FileCopy(const char *orig, const char *dest)
{
 // First I alloc a 16Kb buffer
 char *buf=new char[eachRead];
 if (!buf)
    return 0;

 // Create destination
 FILE *d=fopen(dest,"wb");
 if (!d)
   {
    delete buf;
    return 0;
   }

 // Open source
 FILE *o=fopen(orig,"rb");
 if (!o)
   {
    fclose(d);
    delete buf;
    return 0;
   }

 int read;
 do
   {
    read=fread(buf,1,eachRead,o);
    fwrite(buf,read,1,d);
   }
 while (read==eachRead);

 int ret=!(ferror(o) || ferror(d));
 fclose(o);
 fclose(d);
 delete buf;

 return ret;
}
#endif

