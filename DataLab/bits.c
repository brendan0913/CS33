/* 
 * CS:APP Data Lab 
 * 
 * Brendan Rossmango 505370692
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
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


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

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

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
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
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
/* Copyright (C) 1991-2012 Free Software Foundation, Inc.
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
/* We do support the IEC 559 math functionality, real and complex.  */
/* wchar_t uses ISO/IEC 10646 (2nd ed., published 2011-03-15) /
   Unicode 6.0.  */
/* We do not support C11 <threads.h>.  */
//1
/* 
 * bitNor - ~(x|y) using only ~ and & 
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitNor(int x, int y) {
  /* by De Morgan's Laws, ~(x|y) is equal to (~x & ~y) */ 
  return (~x & ~y);
}
//2
/* 
 * isEqual - return 1 if x == y, and 0 otherwise 
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int isEqual(int x, int y) {
  /* exploits ability of the xor operator to act as an equal sign
   * if the bits are not the same (not equal), then xor outputs 1;
   * if the bits are the same (equal), then xor outputs 0
   * the bang is because when the numbers are equal (output of xor is 0), the desired output is 1
   */
  return !(x ^ y);
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  /* uses right shift operator to see if x can be represented as an n-bit number and shifts n-1
   * if the number doesn't fit, then a 1 bit is left on which is negated to become 0
   * if the number does fit, then all the 1 bits are shifted off, leaving 0s which is negated to become 1
   * Two cases: x is positive or x is negative; in either case, one of the operands of the addition
   *    will be 0 due to the &
   * 	if positive, then the number is not negated (the case where it gets negated results in 0 and 0 is added)
   * 	if negative, then the numebr is negated (the case where it doesn't get negated is 0
   * 	   due to the negated negative sign bit)
   *  for positive numbers, the n-1 shift is intentional; the shift intentionally leaves a 1 bit
   *     if the number doesnt fit instead of shifting it off; 
   *     if it does fit, the 1 bit will be shifted off leaving only 0s, meaning the number fits
   * for negative numbers to be represented in n bits, the 32 bit number is negated,
   *    and the n-1 shift shifts all the 1s off if
   * 	it fits and leaves a 1 bit on if it doesn't fit
   */
  return !(((x & ~(x >> 31)) + (~x & (x >> 31))) >> (n + ~0)); // must shift by n-1
}
//3
/* 
 * isLess - if x < y  then return 1, else return 0 
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLess(int x, int y) {
  /* Four cases: the signs are unequal (either x < 0 and y > 0 or x > 0 and y < 0) 
   *    and the signs are equal (either positive or negative)
   * if x > y or x == y, then is 0 returned
   * 	if both x and y are positive or negative, isUnequal is 0
   * 	if x is positive and y is negative, isUnequal is 1 (which returns 0, since positive > negative)
   * 	if x is negative and y is positive, isUnequal is 0, so then more info from isEqual or lessThan is needed
   * 		this then returns 1 correctly because isEqual results in 0
   * 	the last step is if x and y have the same sign (isUnequal returns 0 and isEqual reutrns 1;
   * 	        to see if the 1 remains, we need lessThan)
   * 	lessThan adds a negated x to y and then finds the sign
   * 	if both positive and x is less than y, then adding a negated x results in a positive sign bit due to overflow, 
   * 		which results in !(0 | 0) == 1
   * 	if both positive and x is greater than or equal to y, then the signed bit is 1 (there is no overflow),
   * 	which results in !(1 | 0) which is 0
   * 	if both negative and x is less than y, then there is overflow and the signed bit is 0
   * 		which results in !(0|0) == 1
   * 	if both negative and x is greater than or equal to y, then there the signed bit remains as 1, which returns 0
   * 		!(1 | 0) == 0
   */
  int unequalSign = (!(x >> 31)) & (y >> 31); // if the signs are equal, then isUnequal is 0 (false)
  int equalSign = !((x >> 31) ^ (y >>31)); // if the signs equal, then isEqual is 1; if x is positive and y is negative
						//  or vice versa, it is 0
  int lessThan = ((y + ~x) >> 31);
  return !((lessThan & equalSign) | unequalSign);
}
/*
 * multFiveEighths - multiplies by 5/8 rounding toward 0.
 *   Should exactly duplicate effect of C expression (x*5/8),
 *   including overflow behavior.
 *   Examples: multFiveEighths(77) = 48
 *             multFiveEighths(-22) = -13
 *             multFiveEighths(1073741824) = 13421728 (overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 3
 */
int multFiveEighths(int x) {
  /* makes use of right shifting to implement dividing by a power of 2 
   * also keeps track of the 3 bits that are shifted off to completely multiply by 5 without losing precision
   * to round negative numbers toward 0, exploits use of mask - will be == to 7 if negative, 0 if positive
   */
  int divideBy8 = x >> 3; // dividing by powers of 2 can be done by right-shifting
		        	// (except it doesnt round down to 0)
  int remainder = x & 7; // to keep track of the 3 bits that get lost after right shifting
  int mask = x >> 31 & 7; // used to round negative numbers not divisible by 8 towards 0; 
      			// the mask is 7 if negative, 0 if not
  divideBy8 += divideBy8 << 2; // multiply by 5
  remainder += remainder << 2;
  // if the number is positive, the mask will be 0, and the remainder is just shifted by 3
  // if the number is negative, 7 is added to the remainder, which is then shifted by 3
  return divideBy8 + ((remainder + mask) >> 3);
}
//4
/* 
 * twosComp2SignMag - Convert from two's complement to sign-magnitude 
 *   where the MSB is the sign bit
 *   You can assume that x > TMin
 *   Example: twosComp2SignMag(-5) = 0x80000005.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int twosComp2SignMag(int x) {
  /* makes use of a mask for the sign bit to keep track of the sign and right shifts and the xor operator
   * to find the magnitude
   * uses | operator to put the sign back on (will not put sign on if x is positive
   *	 meaning the & negativeSignBit will take no effect)
   * if the number is positive, the right shifts and xor operator do not affect the
   * input number (right shifting results in 0; adding 0 and xor'ing 0 do nothing), and all that is left is
   * put the sign bit back on if negative 
   * if the number is negative, then the right shifts result in -1, and adding this subtracts 1 from x; then,
   * 	the xor with all 1s acts as negation; finally, we or the negative sign bit back on 
   */
  int negativeSignBit = 1 << 31; // use as mask to put the sign bit back on as the 32nd bit
  // left side of the | computes the magnitude; right side computes the sign
  // if the number is positive, the right shift and xor do not affect the number
  // if the number is negaitve, the right shift, addition, and xor act as subtraction and negation
  return ((x + (x >> 31)) ^ (x >> 31)) | (x & negativeSignBit);
}
/*
 * bitParity - returns 1 if x contains an odd number of 0's
 *   Examples: bitParity(5) = 0, bitParity(7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int bitParity(int x) {
  /* make use of the shift operator to break the 32 bit binary representation of the number into 
   * chunks (16 bit, then 8 bit, then 4 bit, then 2 bit, then 1 bit)
   * the xor operator, at the final shift of 1, leaves a 1 as LSB for odd parity
   * by the end, the rightmost bit of x will represent the parity (if 1, then it has odd number of 0s)
   * 
   * in total, there are 31 right shifts
   * for even parity, the LSB will be 0
   * for odd parity, the LSB will be 1
   */
  x = x ^ (x >> 16);
  x = x ^ (x >> 8);
  x = x ^ (x >> 4);
  x = x ^ (x >> 2);
  x = x ^ (x >> 1);
  return x & 1; // if the number has an odd number of 0s, then the rightmost bit will be a 1
}
