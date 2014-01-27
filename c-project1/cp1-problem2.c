/*
 * CS3600, Spring 2014
 * C Bootcamp, Homework 1, Problem 2
 * (c) 2012 Alan Mislove
 *
 * In this problem, your goal is to three integers as input, and perform a few bitwise
 * calculations.  You will write functions that will do the following:
 * 
 * 1. You will determine the integer that has the highest number of bits set as '1', and
 *    will return it (function most_ones).
 *
 * 2. You will take the exclusive-OR of all three integers, and return it as an integer
 *    (function xor_all).
 *
 * 3. You will reverse the order of the bits of the first integer, and will return the
 *    result as an integer (function reverse_bits).
 */

#include <stdio.h>
#include <stdlib.h>

// the prototype of the first function
int most_ones(int a, int b, int c);

//helper function for most_ones
int compare_bits_and(int n);

// the prototype of the second function
int xor_all(int a, int b, int c);

// the prototype of the third function
int reverse_bits(int a);

int main(int argc, char *argv[]) {
  // check for the right number of arguments
  if (argc != 4) {
    printf("Error: Usage: ./cp1-problem2 [int1] [int2] [int3]\n");
    return 0;
  }

  // interpret the variables
  int int1 = atoi(argv[1]);
  int int2 = atoi(argv[2]);
  int int3 = atoi(argv[3]);
  
  // calculate the result for the first problem
  int result1 = most_ones(int1, int2, int3);  
  printf("Success: The argument with the most 1s is: %d\n", result1);

  // calculate the result for the second problem
  int result2 = xor_all(int1, int2, int3);  
  printf("Success: The XOR of all three ints is: %d\n", result2);

  // calculate the result for the third problem
  int result3 = reverse_bits(int1);  
  printf("Success: The reversal of the bits of %d is: %d\n", int1, result3);
 
  return 0;
}

/**
 * This function takes in three ints and returns the int that has the most
 * bits set to 1.  If multiple arguments have the same number of bits set to
 * 1, the function should return the *first* such argument.
 */
int most_ones(int a, int b, int c) {
  if (compare_bits_and(a) >= compare_bits_and(b)) {
    if (compare_bits_and(a) >= compare_bits_and(c)) {
      return a;
      }
    else {
      return c;
      }
  }
  else {
    if (compare_bits_and(b) >= compare_bits_and(c)) {
      return b;
      }
    else {
      return c;
    }
  }
}

//helper function for most_ones
//returns the number of bits set to 1 in a given int
int compare_bits_and(int n) {
  int acc = 0;
  do
  {
    if ((n & 1) == 1) {
      acc++;
      }
    n >>= 1;
  } while (n);
  return acc;
}

/**
 * This function takes in three ints and calculates the exclusive OR (XOR) of
 * all three ints, and returns the result as an int.
 */
int xor_all(int a, int b, int c) {
  int d = a ^ b ^ c;
  return d;
}

/**
 * This function takes in an integer and reverses the order of the bits, and
 * returns the result.  For example, if 40 is the input value, this is
 *   0000 0028
 * in hex.  Reversed, this would be
 *   1400 0000
 * the decimal value of which would be 335544320.
 */
int reverse_bits(int a) {
  int r = a; // eventual reversed bits int of a
  int s = sizeof(a) * 8 - 1; // 8 bit shift needed at the end

  for (a >>= 1; a; a >>= 1)
  {
    r <<= 1;
    r |= a & 1;
    s--;
  }
  r <<= s; // shift when a's highest bits are zero
  return r;
}
