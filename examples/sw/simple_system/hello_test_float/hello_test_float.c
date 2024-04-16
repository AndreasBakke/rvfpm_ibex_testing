// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "simple_system_common.h"
volatile float a, b, c, d, e, f, g;
volatile char array[32];

void add(float a, float b, float *res) {
  *res = a + b;
}

void fmadd(float op1, float op2, float op3, float *res) {
  *res = (op1*op2)+op3;
}

int main(int argc, char **argv) {
  pcount_enable(0);
  pcount_reset();
  pcount_enable(1);

  a = 123.243;
  b = 475.874;
  add(a, b, &c);
  // puts("Res1: ");
  // puts(array);
  // printf("The result is: %f", c);

  d = 11.9888;
  e = 0.19282;
  f = -14.11111111;
  fmadd(d, e, f, &g);
  // printf("Result2 is: %f", g);



  puts("Hello simple system\n");
  puthex(0xDEADBEEF);
  putchar('\n');
  puthex(0xBAADF00D);
  putchar('\n');

  pcount_enable(0);

  // Enable periodic timer interrupt
  // (the actual timebase is a bit meaningless in simulation)
  timer_enable(2000);

  uint64_t last_elapsed_time = get_elapsed_time();

  while (last_elapsed_time <= 4) {
    uint64_t cur_time = get_elapsed_time();
    if (cur_time != last_elapsed_time) {
      last_elapsed_time = cur_time;

      if (last_elapsed_time & 1) {
        puts("Tick!\n");
      } else {
        puts("Tock!\n");
      }
    }
    asm volatile("wfi");
  }

  return 0;
}
