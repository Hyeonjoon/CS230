/* 
 * CS:APP Data Lab 
 * 
 * <Hyun Jun Lee 20150634>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses ISO/IEC 10646 (2nd ed., published 2011-03-15) /
   Unicode 6.0.  */
/* We do not support C11 <threads.h>.  */
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  /* exploit ability of logical not operator to convert | to &. */	
  return ~((~x)|(~y));
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  /* exploit left shift operator to make FF be located in position of byte n. */
  int bit_shift = n << 3;
  return ((x & (0xFF << bit_shift)) >> bit_shift) & 0xFF;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  /* exploit & operator to make extended bit 0. */
  int minus_n = (~n) + 1;
  int bit_shift = 31 + minus_n;
  int mask = ~(((~0) << 1) << bit_shift);
  return ((x >> n) & mask);
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
  /* Divide x into 8 parts of 4 bits. */
  int mask = ((0x11) | (0x11 << 8) | (0x11 << 16) | (0x11 << 24));
  int Firsts = x & mask;
  int Seconds = (x >> 1) & mask;
  int Thirds = (x >> 2) & mask;
  int Fourths = (x >> 3) & mask;
  int counts = Firsts + Seconds + Thirds + Fourths;

  int result = (0xF & counts);
  result = result + (0xF & (counts >> 4));
  result = result + (0xF & (counts >> 8));
  result = result + (0xF & (counts >> 12));
  result = result + (0xF & (counts >> 16));
  result = result + (0xF & (counts >> 20));
  result = result + (0xF & (counts >> 24));
  result = result + (0xF & (counts >> 28));

  return result;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  /* exploit that MSBs of x and -x are 0 only when x is 0. */
  return (~(((~x + 1) | x) >> 31)) & (~((~1 + 1) << 1));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  /* exploit << operator to make 0x1 0x80000000. */
  return (1 << 31);
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  /* exploit arithmetic right shift which duplicates the MSB. */
  /* Considered that n-th bit should be sign bit. */
  int n_minus_1 = n + (~1) + 1;
  int mask_part1 = (x >> 31) << n_minus_1;// MSB....MSB 0...0 with n-1 0s.
  int mask_part2 = (~((~(1) + 1) << n_minus_1)) & x;// n ~ 32-th bit initializing with 0.
  int mask = mask_part1 + mask_part2;// n ~ 32-th bit initializing with MSB.
  return !(x ^ mask);
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
  /* exploit double ! to add 1 to outcome of shift when x is negative and has 1s in its discards. */
  int mask = ~((~0) << n);// masking the discards.
  int MSBs = (x >> 31);
  int round = !(!((x & mask) & MSBs));// 1 if x is negative and has at least one 1 in its discards.
  return (x >> n) + round;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  /* expolit ~ to make x equal to -x-1. */
  return (~x) + 1;
}
/*
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
  /*
   * exploit ! and >> using the fact that if x is positive, it has at least one 1
   * and if shifted right 31 times it becames 0x0.
   */
  return !((!x) + (x >> 31));
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  /* exploit ! to represent each conditions true(1) or false(0). */
  int MSBx = !(!(x >> 31));// 0x0 or 0x1.
  int MSBy = !(!(y >> 31));// 0x0 or 0x1.
  int SignDiff = !((MSBx + (~MSBy + 1)) >> 31);// 0 if x > 0 and y < 0.
  int SignOfDiff = !((y + ~x + 1) >> 31);// 0 if y-x < 0.
  return SignDiff & ((MSBx + ~MSBy + 1) | SignOfDiff);
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
  /*
   * Divide x into 2 parts(high, and low) repeatedly.
   * If there's at least one 1 at higher part, shift right and make that part start from 0-th bit.
   * Keep adding the number of shifts in 'count'
   * At the end, count contains n which is the position of highest 1 minus 1.
   */
  int count;
  int flag_16bits, flag_8bits, flag_4bits, flag_2bits, flag_1bit;
  int x_16bits, x_8bits, x_4bits, x_2bits;
  int minus_1;

  minus_1 = (~1) + 1;
  count = 0;
  
  flag_16bits = !(!(x & (minus_1 << 16)));// 1 if there exists at least one 1 in highest 16bits in x.
  x_16bits = x >> (flag_16bits << 4);
  count = count + (flag_16bits << 4);
  
  flag_8bits = !(!(x_16bits & (minus_1 << 8)));// 1 if there exists at least one 1 in highest 8bits in x_16bits.
  x_8bits = x_16bits >> (flag_8bits << 3);
  count = count + (flag_8bits << 3);
  
  flag_4bits = !(!(x_8bits & (minus_1 << 4)));// 1 if there exists at least one 1 in highest 4bits in x_8bits.
  x_4bits = x_8bits >> (flag_4bits << 2);
  count = count + (flag_4bits << 2);
  
  flag_2bits = !(!(x_4bits & (minus_1 << 2)));// 1 if there exists at least one 1 in highest 2bits in x_4bits.
  x_2bits = x_4bits >> (flag_2bits << 1);
  count = count + (flag_2bits << 1);
  
  flag_1bit = !(!(x_2bits & (minus_1 << 1)));// 1 if there exists 1 in highest 1bit in x_2bits.
  count = count + (flag_1bit);
  return count;
}
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
  /* exploit masks(sign, exp, frac) to masking only bits for each parts. */
  unsigned sign = (uf & 0x80000000);// just cleared other bits.
  unsigned exp = (uf & 0x7F800000);
  unsigned frac = (uf & 0x007FFFFF);

  if((exp == 0x7F800000) && (frac != 0)){// if uf is NaN
    return uf;
  }else{
    return ((sign + 0x80000000) + exp + frac);
  }
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  /*
   * Masking sign, and frac bits and masking GRS bits to round.
   * Exploit while loop and counting variable to get exp.
   */
  int count, G, R, S, E, frac, sign_bit, frac_bit, Exp_bit;
  if(x == 0){
    return 0;
  }
  
  count = 0;
  sign_bit = (x & 0x80000000);
  
  if(x < 0){
    x = (~x) + 1;
  }
  
  while((x >> 31) == 0){
    x = (x << 1);
    count = count + 1;
  }
  
  E = (33 + ~(count + 1));
  frac = (x & 0x7FFFFFFF);
  
  G = ((frac & 0x00000100) >> 8);
  R = ((frac & 0x00000080) >> 7);
  S = !(!(frac & 0x0000007F));
  
  frac_bit = (frac >> 8);
  Exp_bit = ((E + 127) << 23);
  
  if(R && ((G && (!S)) || (S))){
    frac_bit = frac_bit + 1;
  }
  return (sign_bit + Exp_bit + frac_bit);
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  /*
   * Divided into 4 case.
   * (NaN and infinity)/(+0 and -0)/(Denormalized small numbers)/(Normalized numbers)
   */
  int Mask_exp = 0x7F800000;
  int exp = ((uf & Mask_exp) >> 23);
  if(exp == 0xFF){// uf is NaN or Infinity.
    return uf;
  }else if((uf & 0x7FFFFFFF) == 0){// uf is +0 or -o.
    return uf;
  }else if(exp == 0){// uf is denormalized small number.
    return ((uf & 0x80000000) | (uf << 1));
  }else{
    exp = exp + 1;
  }
  return ((uf & (~Mask_exp)) | (exp << 23));
}
