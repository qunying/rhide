/* Copyright (C) 1996-2000 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdbrh.h>
#include <librhgdb.h>
#include <rhgdbint.h>

int
register_count()
{
  return NUM_REGS;
}

int
is_float_reg(int num)
{
  return TYPE_CODE(REGISTER_VIRTUAL_TYPE(num)) == TYPE_CODE_FLT;
}

const char *
register_name(int num)
{
  return REGISTER_NAME(num);
}

unsigned long
get_register_value(int num)
{
  char d[sizeof(unsigned long)];
  char buf[MAX_REGISTER_RAW_SIZE];

  if (!debugger_started)
    return 0;
  read_register_gen(num, buf);
  memset(d, 0, sizeof(d));
  if (REGISTER_CONVERTIBLE(num))
  {
    REGISTER_CONVERT_TO_VIRTUAL(num, REGISTER_VIRTUAL_TYPE(num), buf, d);
  }
  else
    memcpy(d, buf, REGISTER_RAW_SIZE(num));
  return *(unsigned long *) d;
}

long double
get_float_register_value(int num)
{
  char d[sizeof(long double)];
  char buf[MAX_REGISTER_RAW_SIZE];

  if (!debugger_started)
    return 0;
  read_register_gen(num, buf);
  if (REGISTER_CONVERTIBLE(num))
  {
    REGISTER_CONVERT_TO_VIRTUAL(num, REGISTER_VIRTUAL_TYPE(num), buf, d);
  }
  else
    memcpy(d, buf, REGISTER_RAW_SIZE(num));
  return *((long double *) d) + 0.0;
}

void
set_register_value(int num, unsigned long value)
{
  if (!debugger_started)
    return;
  write_register(num, value);
}

/* Registers we shouldn't try to store.  */
#if !defined (CANNOT_STORE_REGISTER)
#define CANNOT_STORE_REGISTER(regno) 0
#endif

#ifndef REGCACHE_H
extern int registers_pid;

/* Write register REGNO at MYADDR to the target.  MYADDR points at
   REGISTER_RAW_BYTES(REGNO), which must be in target byte-order.  */

static void
write_register_gen(regno, myaddr)
     int regno;
     char *myaddr;
{
  int size;

  /*
     On the sparc, writing %g0 is a no-op, so we don't even want to change
     the registers array if something writes to this register.  
   */
  if (CANNOT_STORE_REGISTER(regno))
    return;

  if (registers_pid != _inferior_pid())
  {
    registers_changed();
    registers_pid = _inferior_pid();
  }

  size = REGISTER_RAW_SIZE(regno);

  /*
     If we have a valid copy of the register, and new value == old value,
     then don't bother doing the actual store. 
   */

  if (register_valid[regno]
      && memcmp(&registers[REGISTER_BYTE(regno)], myaddr, size) == 0)
    return;

  target_prepare_to_store();

  memcpy(&registers[REGISTER_BYTE(regno)], myaddr, size);

  register_valid[regno] = 1;

  target_store_registers(regno);
}
#endif //REGCACHE_H

void
set_float_register_value(int num, long double value)
{
  char d[sizeof(long double)];
  char buf[20];

  if (!debugger_started())
    return;
  *(long double *) d = value;
#ifdef REGISTER_CONVERT_TO_RAW
  REGISTER_CONVERT_TO_RAW(REGISTER_VIRTUAL_TYPE(num), num, d, buf);
#else
  floatformat_from_double(&floatformat_i387_ext, d, buf);
#endif
  write_register_gen(register_count() + num, buf);
}

int
get_register_size(int num __attribute__ ((unused)))
{
  return REGISTER_RAW_SIZE(num);
}
