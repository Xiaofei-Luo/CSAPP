/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
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

/*
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
    // De Morgan's laws: x Λ y = ¬(¬(x Λ y)) = ¬(¬x V ¬y)
    return ~(~x | ~y);
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
    // Least significant byte of (x >> 8*n)
    return (x >> (n << 3)) & 0xff;
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
    // mask the first n bits of x >> n
    int mask = ~(1 << 31 >> n << 1);  // n = 0!
    return (x >> n) & mask;
}

/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    // ref: https://stackoverflow.com/questions/3815165/how-to-implement-bitcount-using-only-bitwise-operators
    // add neighbouring 2, 4, 8, 16 bits together
    int mask1, mask2, mask3, mask4, mask5;

    //to get mask1 0x55555555 = 01010101 01010101 01010101 01010101
    mask1 = 0x55 << 8 | 0x55;
    mask1 = mask1 << 16 | mask1;

    //to get mask2 0x33333333 = 00110011 00110011 00110011 00110011
    mask2 = 0x33 << 8 | 0x33;
    mask2 = mask2 << 16 | mask2;

    //to get mask3 0x0f0f0f0f = 00001111 00001111 00001111 00001111
    mask3 = 0x0F << 8 | 0x0F;
    mask3 = mask3 << 16 | mask3;

    //to get mask4 0x00ff00ff = 00000000 11111111 00000000 11111111
    mask4 = 0xFF << 16 | 0xFF;

    //to get mask5 0x0000ffff = 00000000 00000000 11111111 11111111
    mask5 = 0xFF << 8 | 0xFF;

    x = (x & mask1) + ((x >> 1) & mask1);  //add every 2 bits
    x = (x & mask2) + ((x >> 2) & mask2);  //add every 4 bits
    x = (x & mask3) + ((x >> 4) & mask3);  //add every 8 bits
    x = (x & mask4) + ((x >> 8) & mask4);  //add every 16 bits
    x = (x & mask5) + ((x >> 16) & mask5);  //add every 32 bits
    return x;
}

/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
    // MSB of x | ~x+1 is 0 if x is 0 else 1
    return ~((x | (~x + 1)) >> 31) & 0x01;
}

/*
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    return 1 << 31;
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
    // fit condition: if x>0, left 32-n bits are all 0s if x<0,
    // left 32-n bits are all 1s. left then right shift 32-n bits
    // to reset the left 32-n bits to 0 or 1
    int shift = 33 + ~n;  // ~n + n = -1 => 32-n = 33 + ~n
    return !(x ^ ((x << shift) >> shift));  // a^b = 0 <=> a == b
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
    // x>>n gives floor(x/2^n), problematic if x<0
    // fact: ceil(x/2^n) = floor((x+2^n-1)/2^n)
    // if x>0: x >>n; if x<0: (x + (1<<n)-1) >> n
    int sign = x >> 31 & 0x01;
    return (x + (sign << n) + ~sign + 1) >> n;
}

/*
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    // ~x + x = -1
    return ~x + 1;
}

/*
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
    // if x<0, MSB=1; if x>=0, MSB=0
    return !((x >> 31) | (!x));
}

/*
 * isLessOrEqual - if x <= y then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    // case1. x>=0, y<0 return 0
    // case2. x<0, y>=0 return 1
    // case3. y - x >= 0, -x = ~x + 1
    int signx = x >> 31;
    int signy = y >> 31;
    return (!(signx ^ signy) & (((x + ((~y) + 1)) >> 31) | !(x ^ y))) | (signx & (!signy));
}

/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
    // position of leftmost 1 is n, return n-1
    // make bits at position 0-n 1, use bit count
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    return bitCount(x) - 1;
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
    // - NaN when exp = 11...1, frac != 0
    // - for unsigned integers >> is logical shift
    // - a Λ 0 = a, a Λ 1 = ~a
    int exp = uf >> 23 & 0xff;
    int frac = uf << 9 >> 9;
    if (!(exp ^ 0xff) && frac)
        return uf;
    else
        return uf ^ (1 << 31);
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
    // special case: 0
    if (x == 0)
        return 0;
    // -INT_MIN = INT_MIN
    if (x == 0x80000000)
        return 0xcf000000;

    // sign
    unsigned sign = x >> 31;
    if (sign)
        x = -x;
    // fraction, exponent
    unsigned frac = x, exp;
    // move the MSB to leftmost
    for (exp = 127 + 31; !(frac & 0x80000000); --exp)
        frac <<= 1;
    // last 8 bits
    unsigned tail = frac & 0xff;
    frac >>= 8;

    // round tail bits to nearest even
    if ((tail > 0x80) | (tail == 0x80 && (frac & 1)))
        frac += 1;
    // if after rounding, fraction has more than 23 bits
    if (frac == 0x01000000)
        exp++;
    // keep last 23 bits
    frac = frac & 0x7fffff;
    return (sign << 31) | (exp << 23) | frac;
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
    unsigned sign = uf >> 31;
    unsigned exp = (uf >> 23) & 0xff;
    unsigned frac = uf & 0x7fffff;

    // denormlized case 1: return uf
    if (exp == 0xff)
        return uf;
    // denormalized case 2: leftshift frac by 1, exp++ if there's a carry
    if (exp == 0) {
        frac <<= 1;
        if (frac & 0x800000) {
            frac &= 0x7fffff;
            exp++;
        }
    } else
        // normalized case, exp++
        exp++;
    return (sign << 31) | (exp << 23) | frac;
}