// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "simple_system_common.h"
volatile float a, b, c, d, e, f, g, h, i, j , k;
volatile int x, y, z;
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
  puts("Hello simple system\n\n");

  a = 123.243;
  b = 475.874;
  d = 0.98876;
  add(a, b, &c);
  puts("Add 123.243 & 475.875: ");;
  puthexfloat(c);

  for (int i =0; i<100; i++) {
    fmadd(i, d, e, &e);
    add(c, d, &c);
  }
  __asm__("fmv.x.w %0, %1" : "=r"(x): "f"(c));
  puts("\n\n100 FMADDs result in: ");
  puthex(x);

  h = b;
  puts("\n\nPerforming 20 divisions:");
  for (int i=0; i<20; i++) {
    div(h,a, &h); //The FPNew in Fpu_ss has some compliance issues for the div&sqrt operations, running many divisions might lead to differing tracefiles
  __asm__("fmv.x.w %0, %1" : "=r"(x): "f"(h));
    puts("\n i=");
    puthex(i);
    puts(" result: ");
    puthex(x);
  }

  h=b;
  puts("\n\nPerforming 20 square roots:");
  for (int i=0; i<20; i++) {
  __asm__("fsqrt.s %0, %1" : "=f"(h): "f"(h)); //The FPNew in Fpu_ss has some compliance issues for the div&sqrt operations, running many divisions might lead to differing tracefiles
  __asm__("fmv.x.w %0, %1" : "=r"(x): "f"(h));
    puts("\n i=");
    puthex(i);
    puts(" result: ");
    puthex(x);
  }

  j = 123.1456909f;
  h = 12.2019f;
  g = -0.0f;
  d = 0.32172f;

  puts("\n\nSign, max and fnmsub in while loop testing.");
  while(1) {
    h = j > 0 ? h : -h; //Sign instruction
    __asm__ ("fmax.s %0, %1, %2" : "=f"(g) : "f"(h), "f"(j)); //inline assembly max
    j = j-g*d; //fnmsub

    if (j < 0) {
      break;
    }
  }
  __asm__("fmv.x.w %0, %1" : "=r"(x): "f"(j));
  puts("\n Final result:");
  puthex(x);

  //Run some inline assembly tests for untested instructions
  //Classify all variables used so far
  puts("\n\nTesting Classify instructions: \n");

  x = 0xff800000;
  __asm__("fmv.w.x %0, %1" : "=f"(a) : "r"(x)); //move to f-reg
  __asm__("fclass.s %0, %1" : "=r"(x) : "f"(a));//-infty
  puts(" Expecting 0x1: ");
  puthex(x);

  b = -2.4f;
  __asm__("fclass.s %0, %1" : "=r"(y) : "f"(b)); //Negative normal
  puts("\n Expecting 0x2: ");
  puthex(y);

  c = -1.0e-38f/1e5f;
  __asm__("fclass.s %0, %1" : "=r"(z) : "f"(c)); //Negative subnormal
  puts("\n Expecting 0x4: ");
  puthex(z);

  d = -0.0f;
  __asm__("fclass.s %0, %1" : "=r"(x) : "f"(d)); // -0
  puts("\n Expecting 0x8: ");
  puthex(x);


  e = 0.0f;
  __asm__("fclass.s %0, %1" : "=r"(y) : "f"(e)); // +0
  puts("\n Expecting 0x10: ");
  puthex(y);


  f = 1.0e-38f/1e5f;
  __asm__("fclass.s %0, %1" : "=r"(z) : "f"(f)); // positive subnormal
  puts("\n Expecting 0x20: ");
  puthex(z);


  g = 2.4f;
  __asm__("fclass.s %0, %1" : "=r"(x) : "f"(g)); //positive normal
  puts("\n Expecting 0x40: ");
  puthex(x);


  y = 0x7f800000;
  __asm__("fmv.w.x %0, %1" : "=f"(h) : "r"(y)); //move to f-reg
  __asm__("fclass.s %0, %1" : "=r"(y) : "f"(h)); //+inft
  puts("\n Expecting 0x80: ");
  puthex(y);

  z = 0x7FA00000; ///Create SNAN in X registers
  __asm__("fmv.w.x %0, %1" : "=f"(j) : "r"(z)); //move to f-reg
  __asm__("fclass.s %0, %1" : "=r"(z) : "f"(j)); //SNAN
  puts("\n Expecting 0x100: ");
  puthex(z);

  x = 0x7FA00000; ///Create QNAN in X registers
  __asm__("fmv.w.x %0, %1" : "=f"(k) : "r"(x)); //move to f-reg
  __asm__("fclass.s %0, %1" : "=r"(x) : "f"(k)); //QNAN
  puts("\n Expecting 0x200: ");
  puthex(x);


  //min
  puts("\n\nTesting MIN instructions: ");
  a = 0.0f;
  b = -0.0f;
  __asm__ ("fmin.s %0, %1, %2" : "=f"(j) : "f"(a), "f"(b));
  __asm__("fmv.x.w %0, %1" : "=r"(x): "f"(j));
  puts("\n Min(0.0, -0.0), expecting: 0x8000000: ");
  puthex(x);

  a = 1.0e-38f/1e5f; //positive subnormal
  b = 1.0e-38f/1e6f; //a bit smaller subnormal
  __asm__ ("fmin.s %0, %1, %2" : "=f"(j) : "f"(a), "f"(b));
  __asm__("fmv.x.w %0, %1" : "=r"(x): "f"(j));
  puts("\n Min(1.0e-43, 1.0e-44), expecting: 0x0000007: ");
  puthex(x);

  a = 2.0e38f; //positive subnormal
  b = 2.1e38f; //a bit smaller subnormal
  __asm__ ("fmin.s %0, %1, %2" : "=f"(j) : "f"(a), "f"(b));
  __asm__("fmv.x.w %0, %1" : "=r"(x): "f"(j));
  puts("\n Min(2.0e38f, 2.1e38f), expecting: 0x7f167699: ");
  puthex(x);

  puts("\n\nTesting MAX instructions: ");
  a = 0.0f;
  b = -0.0f;
  __asm__ ("fmax.s %0, %1, %2" : "=f"(j) : "f"(a), "f"(b));
  __asm__("fmv.x.w %0, %1" : "=r"(x): "f"(j));
  puts("\n MAX(0.0, -0.0), expecting: 0x0: ");
  puthex(x);

  a = 1.0e-38f/1e5f; //positive subnormal
  b = 1.0e-38f/1e6f; //a bit smaller subnormal
  __asm__ ("fmax.s %0, %1, %2" : "=f"(j) : "f"(a), "f"(b));
  __asm__("fmv.x.w %0, %1" : "=r"(x): "f"(j));
  puts("\n MAX(1.0e-43, 1.0e-44), expecting: 0x00000047: ");
  puthex(x);

  a = 2.0e38f; //positive subnormal
  b = 2.1e38f; //a bit smaller subnormal
  __asm__ ("fmax.s %0, %1, %2" : "=f"(j) : "f"(a), "f"(b));
  __asm__("fmv.x.w %0, %1" : "=r"(x): "f"(j));
  puts("\n MAX(2.0e38f, 2.1e38f), expecting: 0x7f1dfc87: ");
  puthex(x);

  puts("\n\n");

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
