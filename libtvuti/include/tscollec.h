/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#if defined( Uses_TSCollection ) && !defined( __TSCollection__ )
#define __TSCollection__

class TSCollection:public TCollection
{
public:
  TSCollection(void):TCollection(10, 10)
  {
  }
private:

    virtual void *readItem(ipstream &)
  {
    return 0;
  }
  virtual void writeItem(void *, opstream &)
  {
  };

};

#endif
