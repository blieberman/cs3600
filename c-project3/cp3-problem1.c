/*
 * CS3600, Spring 2014
 * C Bootcamp, Homework 3, Problem 1
 * (c) 2012 Alan Mislove
 *
 * In this problem, your goal is to fill in the itoaaa function.
 * This function will take in a 32-bit signed integer, and will
 * return a malloc'ed char * containing the English representation
 * of the number.  A few examples are below:
 *
 * 0 -> "zero"
 * 9 -> "nine"
 * 45 -> "forty five"
 * -130 -> "negative one hundred thirty"
 * 11983 -> "eleven thousand nine hundred eighty three"
 *
 * Do not touch anything outside of the itoaaa function (you may,
 * of course, define any helper functions you wish).  You may also
 * use any of the functions in <string.h>. 
 *
 * Finally, you must make sure to free() any intermediate malloced()
 * memory before you return the result.  You should also return a 
 * char* that is malloced to be as small as necessary (the script 
 * checks for this).  For example, if you returned "forty five", you
 * should put this into a malloced space of 12 bytes (11 + '\0').
 *
 */

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

char *itoaaa(int i);

int main(int argc, char **argv) {
  // check for the right number of arguments
  if (argc != 2) {
    printf("Error: Usage: ./cp3-problem1 [int]\n");
    return 0;
  }

  // create the time structure
  long long arg = atoll(argv[1]);
  if (arg == (int) arg) {
    // call the function
    char *result = itoaaa((int) arg);

    // print out the result
    printf("%d is: %s\n", (int) arg, result);
  } else {
    printf("Error: Number out of range.\n");
  }

  return 0;
}

//Number to English Arrays to be used as maps:	
char *digits[] = {"", "one", "two", "three", "four", "five", 
	"six", "seven", "eight", "nine", "ten", "eleven", "twelve", "thirteen",
	"fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"};
	
char *tens[] = {"", "", "twenty", "thirty", "forty", "fifty",
	"sixty", "seventy", "eighty", "ninety"};	

/**
* itoaaaHelper : helper function of itoaaa
*/
char *itoaaaHelper(int arg) {
	
	char *buffer = malloc(1024);
	strcpy(buffer, "");
	
	if (arg < 0) {
		strcpy(buffer, "negative ");
		arg *= -1;
		strcat(buffer, itoaaaHelper(arg));
	}
	else if (arg / 1000000000 > 0) {
		sprintf(buffer, "%s billion %s", itoaaaHelper(arg / 1000000000),
		  itoaaaHelper(arg % 1000000000));
	}
	else if (arg / 1000000 > 0) {
		sprintf(buffer, "%s million %s", itoaaaHelper(arg / 1000000),
		  itoaaaHelper(arg % 1000000));
	}
	else if (arg / 1000 > 0) {
		sprintf(buffer, "%s thousand %s", itoaaaHelper(arg / 1000),
		  itoaaaHelper(arg % 1000));
	}
	else if (arg / 100 > 0) {
		sprintf(buffer, "%s hundred %s", itoaaaHelper(arg / 100),
		  itoaaaHelper(arg % 100));
	}
	else if (arg > 19) {
		strcat(buffer, tens[arg / 10]);
		
		if (arg % 10 != 0) {
			strcat(buffer, " ");
		}

		strcat(buffer, digits[arg % 10]);
	}
	else {
		strcat(buffer, digits[arg]);
	}
	
	return buffer;
	free(buffer);
}

/**
* This function should print out the English full representation
* of the passed-in argument.  See above for more details.
*/
char *itoaaa(int arg) {
	if (arg == 0) {
		return "zero";
	}
	else {
	  char* finbuf = malloc(1+sizeof(itoaaaHelper(arg)));
		strcpy(finbuf, itoaaaHelper(arg));
		
		return finbuf;
		free(finbuf);
	}		
}