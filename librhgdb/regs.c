/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <libgdb.h>
#include <librhgdb.h>


int register_count()
{
  return NUM_REGS-NUM_FREGS;
}

int float_register_count()
{
  return NUM_FREGS;
}

static char *register_names[] = REGISTER_NAMES;

const char *register_name(int num)
{
  return register_names[num];
}

unsigned long get_register_value(int num)
{
  if (!debugger_started)
    return 0;
  return read_register(num);
}

double get_float_register_value(int num)
{
  double d;
  char buf[10];
  if (!debugger_started)
    return 0;
  read_register_gen(register_count()+num, buf);
  floatformat_to_double(&floatformat_i387_ext, buf,
                        &d);
  return d;
}

void set_register_value(int num, unsigned long value)
{
  if (!debugger_started)
    return;
  write_register(num, value);
}

/* Registers we shouldn't try to store.  */
#if !defined (CANNOT_STORE_REGISTER)
#define CANNOT_STORE_REGISTER(regno) 0
#endif

extern int registers_pid;

/* Write register REGNO at MYADDR to the target.  MYADDR points at
   REGISTER_RAW_BYTES(REGNO), which must be in target byte-order.  */

static void
write_register_gen (regno, myaddr)
     int regno;
     char *myaddr;
{
  int size;

  /* On the sparc, writing %g0 is a no-op, so we don't even want to change
     the registers array if something writes to this register.  */
  if (CANNOT_STORE_REGISTER (regno))
    return;

  if (registers_pid != inferior_pid)
    {
      registers_changed ();
      registers_pid = inferior_pid;
    }

  size = REGISTER_RAW_SIZE(regno);

  /* If we have a valid copy of the register, and new value == old value,
     then don't bother doing the actual store. */

  if (register_valid [regno]
      && memcmp (&registers[REGISTER_BYTE (regno)], myaddr, size) == 0)
    return;
  
  target_prepare_to_store ();

  memcpy (&registers[REGISTER_BYTE (regno)], myaddr, size);

  register_valid [regno] = 1;

  target_store_registers (regno);
}

void set_float_register_value(int num, double value)
{
  char buf[10];
  if (!debugger_started)
    return;
  double_to_i387((char*)&value, buf);
  write_register_gen(register_count()+num, buf);
}

int get_register_size(int num)
{
  return REGISTER_RAW_SIZE(num);
}


