/* Copyright (C) 1996-2003 Robert Hoehne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef USE_DUMMY_ICONV

int iconv_open(const char* tocode __attribute__((unused)), 
               const char* fromcode __attribute__((unused)))
{
  return -1;
}

int libiconv_open(const char* tocode, 
                  const char* fromcode)
{
  return iconv_open(tocode, fromcode);;
}


int iconv(int cd __attribute__((unused)),
          const char **inbuf __attribute__((unused)),
	  int *inbytesleft __attribute__((unused)),
	  char **outbuf __attribute__((unused)),
	  int *outbytesleft __attribute__((unused)))
{
  return 0;
}

int libiconv(int cd,
             const char **inbuf,
	     int *inbytesleft,
	     char **outbuf,
	     int *outbytesleft)
{
  return iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft);
}

int iconv_close(int cd __attribute__((unused)))
{
  return -1;
}

int libiconv_close(int cd)
{
  return iconv_close(cd);
}
	    
#endif

