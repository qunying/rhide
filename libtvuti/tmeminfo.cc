/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#define Uses_TMemInfo
#include <libtvuti.h>

#include <stdio.h>
#ifdef __DJGPP__
#include <dpmi.h>
#endif

void
TIDEMemInfo::update(int force)
{
  unsigned long _p_mem, _v_mem;

  get_mem_info(_v_mem, _p_mem);
  if (force || _p_mem != p_mem || _v_mem != v_mem)
  {
    v_mem = _v_mem;
    p_mem = _p_mem;
    drawView();
  }
}

static char
scale_mem(unsigned long &b, int long_format)
{
  unsigned long v = 1024;

  if (long_format)
    v *= 1024;
  if (b <= v)
    return 'B';
  b /= 1024;
  if (b <= v)
    return 'K';
  b /= 1024;
  if (b <= v)
    return 'M';
  b /= 1024;
  return 'K';
}

void
TIDEMemInfo::draw()
{
  TDrawBuffer b;
  char dummy[80];
  char v = ' ', p = ' ';
  unsigned long _v_mem = v_mem, _p_mem = p_mem;

  v = scale_mem(_v_mem, long_format);
  p = scale_mem(_p_mem, long_format);
  uchar c = getColor(2);

  b.moveChar(0, ' ', c, size.x);
  sprintf(dummy, "%ld%c/%ld%c", _v_mem, v, _p_mem, p);
  b.moveStr(0, dummy, c);
  writeLine(0, 0, size.x, 1, b);
}


#ifdef __linux__
#include <unistd.h>

unsigned long
used_mem()
{
  char fname[256];

  sprintf(fname, "/proc/%d/stat", getpid());
  FILE *fmem = fopen(fname, "r");
  unsigned long mem;

  fscanf(fmem,
         "%*d %*s %*c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %ld",
         &mem);
  fclose(fmem);
  return mem;
}
#endif

void
TIDEMemInfo::get_mem_info(unsigned long &virt_mem, unsigned long &phys_mem)
{
#ifdef __DJGPP__
  _go32_dpmi_meminfo info;

  _go32_dpmi_get_free_memory_information(&info);
  virt_mem = info.available_memory;
  phys_mem = info.available_physical_pages * 4096;
#else // __DJGPP__
#ifdef __linux__
  {
    FILE *mem = fopen("/proc/meminfo", "r");
    unsigned long buffers, sw_free, cached;
    char buf[1024];

    fgets(buf, 1024, mem);
    fscanf(mem, "Mem: %*d %*d %ld %*d %ld %ld\nSwap: %*d %*d %ld", &phys_mem,
           &buffers, &cached, &sw_free);
    fclose(mem);
#if 0
    phys_mem += buffers;
#endif
    phys_mem += cached;
    virt_mem = phys_mem + sw_free;
  }
#else // __linux__
  virt_mem = 0;
  phys_mem = 0;
#endif // __linux__
#endif // __DJGPP__
}
