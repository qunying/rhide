/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TDialog
#define Uses_TRect
#define Uses_TValidator
#define Uses_TInputLine
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TView
#define Uses_TLabel
#define Uses_TButton
#define Uses_TDeskTop
#define Uses_MsgBox

#define Uses_tvutilFunctions
#include <libtvuti.h>

#include <rhutils.h>

#define Uses_TFlagEntry
#include <libide.h>

#include <string.h>
#include <stdio.h>

static ushort TFlagEntry_Version = 1;

void
TFlagEntry::ToggleActive()
{
  char buffer[256], label[256];
  TValidator *val = NULL;

  if (activated)
  {
    activated = False;
  }
  else
  {
    activated = True;
    switch (type)
    {
      case FLAG_TYPE:
        return;
      case INT_TYPE:
        if (data.ivalue < 0)
          strcpy(buffer, "0");
        else
          sprintf(buffer, "%d", data.ivalue);
        if (ValidInputBox(_("You must type a value here"),
                          _("~V~alue"), buffer, 3,
                          new TRangeValidator(0, 9)) != cmOK)
        {
          activated = False;
        }
        else
          sscanf(buffer, "%d", &data.ivalue);
        break;
      case OPTIONAL_INT_TYPE:
        if (data.ivalue < 0)
          strcpy(buffer, "");
        else
          sprintf(buffer, "%d", data.ivalue);
        if (ValidInputBox(_("You can type a value here"),
                          _("~V~alue (-1 for none)"), buffer, 3,
                          new TRangeValidator(-1, 9)) != cmOK)
        {
          data.ivalue = -1;
        }
        else
          sscanf(buffer, "%d", &data.ivalue);
        break;
      case STRING_TYPE:
        strcpy(label, _("~V~alue"));
        if (data.svalue.allowed)
        {
          strcat(label, _(" allowed chars "));
          strcat(label, "'");
          strcat(label, data.svalue.allowed);
          strcat(label, "'");
          val = new TFilterValidator(data.svalue.allowed);
        }
        if (data.svalue.value)
          strcpy(buffer, data.svalue.value);
        else
          buffer[0] = 0;
        if (ValidInputBox
            (_("You must type a string here"), label, buffer, 255,
             val) != cmOK)
        {
          activated = False;
          return;
        }
        string_free(data.svalue.value);
        string_dup(data.svalue.value, buffer);
        break;
      case OPTIONAL_STRING_TYPE:
        strcpy(label, _("~V~alue"));
        if (data.svalue.allowed)
        {
          strcat(label, _(" allowed chars "));
          strcat(label, "'");
          strcat(label, data.svalue.allowed);
          strcat(label, "'");
          val = new TFilterValidator(data.svalue.allowed);
        }
        if (data.svalue.value)
          strcpy(buffer, data.svalue.value);
        else
          buffer[0] = 0;
        if (ValidInputBox(_("You can type a string here"), label, buffer, 255,
                          val) == cmOK)
        {
          string_free(data.svalue.value);
          string_dup(data.svalue.value, buffer);
        }
        break;
      case CHAR_TYPE:
        strcpy(label, _("~V~alue"));
        if (data.cvalue)
          strcpy(buffer, data.cvalue);
        else
          buffer[0] = 0;
        if (ValidInputBox
            (_("You must type a character here"), label, buffer, 255,
             NULL) != cmOK)
        {
          activated = False;
          return;
        }
        string_free(data.cvalue);
        string_dup(data.cvalue, buffer);
        break;
      case OPTIONAL_CHAR_TYPE:
        strcpy(label, _("~V~alue"));
        if (data.cvalue)
          strcpy(buffer, data.cvalue);
        else
          buffer[0] = 0;
        if (ValidInputBox
            (_("You can type a character here"), label, buffer, 255,
             NULL) == cmOK)
        {
          string_free(data.cvalue);
          string_dup(data.cvalue, buffer);
        }
        break;
    }
  }
}

void *
TFlagEntry::read(ipstream & is)
{
  char temp;
  ushort version;

  is >> version;
  value = is.readString();
  is >> temp;
  activated = (Boolean) temp;
  is >> temp;
  hasoption = (Boolean) temp;
  is >> temp;
  type = (FlagType) temp;
  switch (type)
  {
    case OPTIONAL_CHAR_TYPE:
    case CHAR_TYPE:
      data.cvalue = is.readString();
      break;
    case OPTIONAL_STRING_TYPE:
    case STRING_TYPE:
      data.svalue.allowed = is.readString();
      data.svalue.value = is.readString();
      break;
    case OPTIONAL_INT_TYPE:
    case INT_TYPE:
      is >> data.ivalue;
      break;
    default:
      break;
  }
#ifdef DEBUG
  fprintf(stderr, "TFlagEntry: %s\n", value);
#endif
  return this;
}

void
TFlagEntry::write(opstream & os)
{
  char temp;

  os << TFlagEntry_Version;
  os.writeString(value);
  temp = (char) activated;
  os << temp;
  temp = (char) hasoption;
  os << temp;
  temp = (char) type;
  os << temp;
  switch (type)
  {
    case OPTIONAL_CHAR_TYPE:
    case CHAR_TYPE:
      os.writeString(data.cvalue);
      break;
    case OPTIONAL_STRING_TYPE:
    case STRING_TYPE:
      os.writeString(data.svalue.allowed);
      os.writeString(data.svalue.value);
      break;
    case OPTIONAL_INT_TYPE:
    case INT_TYPE:
      os << data.ivalue;
      break;
    default:
      break;
  }
}

TFlagEntry::~TFlagEntry()
{
  string_free(value);
  if (type == OPTIONAL_STRING_TYPE || type == STRING_TYPE)
  {
    string_free(data.svalue.allowed);
    string_free(data.svalue.value);
  }
  if (type == OPTIONAL_CHAR_TYPE || type == CHAR_TYPE)
  {
    string_free(data.cvalue);
  }
}

TFlagEntry::TFlagEntry(char *_flag):
activated(False), hasoption(False), type(FLAG_TYPE)
{
  if (!_flag)
  {
    value = NULL;
    return;
  }
  char flag[1024];

  strcpy(flag, _flag);
  char *tmp = strchr(flag, '%');

  if (tmp)
  {
    hasoption = True;
    *tmp = 0;
    string_dup(value, flag);
    *tmp = '%';
    tmp++;
    switch (*tmp)
    {
      case 'd':
        if (tmp[1] == '?')
          type = OPTIONAL_INT_TYPE;
        else
          type = INT_TYPE;
        data.ivalue = -1;
        break;
      case 'c':
        if (tmp[1] == '?')
          type = OPTIONAL_CHAR_TYPE;
        else
          type = CHAR_TYPE;
        data.cvalue = NULL;
        break;
      case 's':
        if (tmp[1] == '?')
        {
          type = OPTIONAL_STRING_TYPE;
          tmp++;
        }
        else
          type = STRING_TYPE;
        tmp++;
        data.svalue.allowed = data.svalue.value = NULL;
        if (*tmp == '(')
        {
          char *temp = strchr(tmp + 1, ')');

          if (temp)
          {
            *temp = 0;
            string_dup(data.svalue.allowed, tmp + 1);
            *temp = ')';
          }
        }
        break;
      default:
        hasoption = False;
        type = FLAG_TYPE;
        break;
    }
  }
  else
  {
    string_dup(value, flag);
  }
}

TFlagEntry::TFlagEntry(const TFlagEntry * flag)
{
  string_dup(value, flag->value);
  type = flag->type;
  hasoption = flag->hasoption;
  activated = flag->activated;
  switch (type)
  {
    case INT_TYPE:
    case OPTIONAL_INT_TYPE:
      data.ivalue = flag->data.ivalue;
      break;
    case CHAR_TYPE:
    case OPTIONAL_CHAR_TYPE:
      if (flag->data.cvalue)
        string_dup(data.cvalue, flag->data.cvalue);
      else
        data.cvalue = NULL;
      break;
    case STRING_TYPE:
    case OPTIONAL_STRING_TYPE:
      if (flag->data.svalue.allowed)
        string_dup(data.svalue.allowed, flag->data.svalue.allowed);
      else
        data.svalue.allowed = NULL;
      if (flag->data.svalue.value)
        string_dup(data.svalue.value, flag->data.svalue.value);
      else
        data.svalue.value = NULL;
      break;
    default:
      break;
  }
}

char *
TFlagEntry::GetText()
{
  char *tmp;
  char buffer[256];

  string_dup(tmp, "[");
  if (activated == True)
    string_cat(tmp, "x] ");
  else
    string_cat(tmp, " ] ");
  string_cat(tmp, value);
  switch (type)
  {
    case INT_TYPE:
    case OPTIONAL_INT_TYPE:
      if (data.ivalue >= 0)
      {
        sprintf(buffer, "%d", data.ivalue);
        string_cat(tmp, buffer);
      }
      break;
    case STRING_TYPE:
    case OPTIONAL_STRING_TYPE:
      if (data.svalue.value)
        string_cat(tmp, data.svalue.value);
      break;
    case CHAR_TYPE:
    case OPTIONAL_CHAR_TYPE:
      if (data.cvalue)
        string_cat(tmp, data.cvalue);
      break;
    default:
      break;
  }
  return tmp;
}

char *
TFlagEntry::GetControlString()
{
  char *tmp;

  string_dup(tmp, value);
  switch (type)
  {
    case OPTIONAL_INT_TYPE:
      string_cat(tmp, "%d?");
      break;
    case INT_TYPE:
      string_cat(tmp, "%d");
      break;
    case OPTIONAL_CHAR_TYPE:
      string_cat(tmp, "%c?");
      break;
    case CHAR_TYPE:
      string_cat(tmp, "%c");
      break;
    case STRING_TYPE:
      string_cat(tmp, "%s");
      break;
    case OPTIONAL_STRING_TYPE:
      string_cat(tmp, "%s?");
      if (data.svalue.allowed != NULL)
      {
        string_cat(tmp, "(");
        string_cat(tmp, data.svalue.allowed);
        string_cat(tmp, ")");
      }
      break;
    default:
      break;
  }
  return tmp;
}

char *
TFlagEntry::GetValue()
{
  char *tmp;
  char itmp[10];

  if (activated == False)
    return NULL;
  string_dup(tmp, value);
  switch (type)
  {
    case FLAG_TYPE:
      return tmp;
    case OPTIONAL_STRING_TYPE:
      if (!data.svalue.value)
        return tmp;
    case STRING_TYPE:
      string_cat(tmp, data.svalue.value);
      return tmp;
    case OPTIONAL_INT_TYPE:
      if (data.ivalue < 0)
        return tmp;
    case INT_TYPE:
      sprintf(itmp, "%d", data.ivalue);
      string_cat(tmp, itmp);
      return tmp;
    case OPTIONAL_CHAR_TYPE:
      if (!data.cvalue)
        return tmp;
    case CHAR_TYPE:
      string_cat(tmp, data.cvalue);
      return tmp;
    default:
      return tmp;
  }
}

TStreamable *
TFlagEntry::build()
{
  return new TFlagEntry(streamableInit);
}

TFlagEntry::TFlagEntry(StreamableInit)
{
}
