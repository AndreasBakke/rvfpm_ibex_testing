// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "simple_system_common.h"
volatile float a, b, c, d, e, f, g, h, i, j , k;
volatile char array[32];

void add(float a, float b, float *res) {
  *res = a + b;
}

void fmadd(float op1, float op2, float op3, float *res) {
  *res = (op1*op2)+op3;
}

void div(float a, float b, float *res){
  *res = a/b;
}
void puthexfloat(float f) { //prints float as hex with 6 decimals
  uint32_t temp = (uint32_t)f;
  puthex(temp);
  puts(".");
  f=f-temp;
  f=f*100000; //print first 6 decimals;
  puthex(f);
}

int main(int argc, char **argv) {
  pcount_enable(0);
  pcount_reset();
  pcount_enable(1);

  a = 123.243;
  b = 475.874;
  d = 0.98876;
  add(a, b, &c);
  puts("Result1: ");
  puthexfloat(c);

  for (int i =0; i<100; i++) {
    fmadd(i, d, e, &e);
    add(c, d, &c);
  }
  h = b;
  for (int i=0; i<20; i++) {
    div(h,a, &h);
  }



  puts("\nResult2: ");
  puthexfloat(e);
  puts("\n\n");
  puts("\nResult3: ");
  puthexfloat(h);
  puts("\n\n");


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
