/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <unistd.h>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <iostream>
#include <cstdio>

using namespace std;

static int chapter = 0;
string line;
string line1;
static vector<string> start_of_file;
int start_ended = 0;
static ifstream _fi;
static istream *fi;
static ofstream fo;
static string base;

static
string int2str(int i)
{
  char buf[50];
  sprintf(buf, "%d", i);
  return string(buf);
}

static void
write_foot(bool next = true)
{
  fo << "<p><hr><p>" << endl;
  fo << "<a href=" << base << "0.html>Start of doc</a> | ";
  if (next)
    fo << "<a href=" << base << chapter+1 << ".html>Next</a>";
  if (chapter > 0)
    fo << " | <a href=" << base << chapter-1 << ".html>Previous</a>";
  fo << endl << "</body>\n</html>" << endl;
}

static void
write_head(ofstream & fo, int chap, bool next = true)
{
  vector<string>::iterator it = start_of_file.begin();
  for (it = start_of_file.begin(); it != start_of_file.end(); it++)
  {
    fo << (*it) << endl;
  }
  fo << "<p><hr><p>" << endl;
  fo << "<a href=" << base << "0.html>Start of doc</a> | ";
  if (next)
    fo << "<a href=" << base << chap+1 << ".html>Next</a>";
  if (chap > 0)
    fo << " | <a href=" << base << chap-1 << ".html>Previous</a>";
  fo << endl << "</body>\n</html>" << endl;
}

int start_new_file()
{
  write_foot(true);
  fo.close();
  chapter++;
  string fname = base + int2str(chapter) + ".tmp";
  fo.open(fname.c_str());
  if (!fo) return -2;
  return 0;
}

void add_to_start()
{
  start_of_file.push_back(line);
}

static map<string, int> ref_ids;

static
void addref(string line)
{
  if (line.find("<a name=") != 0)
    return;
  int i = line.find("\"");
  int j = line.rfind("\"");
  if (i < 0)
    return;
  ref_ids[line.substr(i+1, j-i-1)] = chapter;
}

static bool
ReadLine(istream & is, string &s)
{
  return getline(is, s, '\n');
}

static void
check_ref(string & s)
{
  map<string, int>::iterator it;
  for (it = ref_ids.begin(); it != ref_ids.end(); it++)
  {
    string ref = "\"#"+(*it).first+"\"";
    string::size_type i;
    if ((i = s.find(ref)) != string::npos)
    {
      string fname = base + int2str((*it).second) + ".html";
      s.insert(i+1, fname);
      return;
    }
  }
}

static void
convert_file(int chap, bool is_last)
{
  ifstream fi;
  ofstream fo;
  string fnamei = base + int2str(chap) + ".tmp";
  fi.open(fnamei.c_str());
  string fnameo = base + int2str(chap) + ".html";
  fo.open(fnameo.c_str());
  write_head(fo, chap, !is_last);
  string s;
  while (ReadLine(fi, s))
  {
    check_ref(s);
    fo << s << endl;
  }
  fo.close();
  fi.close();
  unlink(fnamei.c_str());
}

int main(int argc,char *argv[])
{
  if (argc > 1)
  {
    _fi.open(argv[1]);
    if (!_fi) return -2;
    argv++;
    fi = &_fi;
  } else fi = &cin;
  base = argv[1];
  string fname = base + int2str(chapter) + ".tmp";
  fo.open(fname.c_str());
  while (ReadLine(*fi, line))
  {
    if (!start_ended)
    {
      if (line.find("<body>") == 0)
      {
        add_to_start();
        start_ended = 1;
        continue;
      }
      add_to_start();
      continue;
    }
    if (line.find("<a name=") == 0)
    {
      if (!ReadLine(*fi, line1))
        break;
      bool end = false;
      while (!end && line1 == "")
      {
        end = !ReadLine(*fi, line1);
      }
      if ((line1.find("<h1>") != string::npos) ||
          (line1.find("<h2>") != string::npos))
      {
        int ret = start_new_file();
        if (ret != 0) return ret;
      }
      addref(line);
      fo << line << endl << line1 << endl;
    }
    else
    {
      fo << line << endl;
    }
  }
  write_foot(false);
  fo.close();
  if (argc > 1) _fi.close();
  for (int i=0; i <= chapter; i++)
  {
    convert_file(i, i == chapter);
  }
  return 0;
}

