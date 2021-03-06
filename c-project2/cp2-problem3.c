/*
 * CS3600, Spring 2014
 * C Bootcamp, Homework 2, Problem 3
 * (c) 2012 Alan Mislove
 *
 * In this problem, your goal is to learn about strings (character arrays).
 * You should fill in the functions below, as described, and be sure to
 * not touch anything in the main() function.
 *
 * Also note that you may not use any of the functions provided by <string.h>;
 * you must do all of the string manipulation yourself.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void str_cat(char *str1, char *str2, char *dst);
int count_lowercase(char *str);
void reverse(char *str);
int string_length(char str[]); //returns length of given string

int main(int argc, const char **argv) {
  // check for the right number of arguments
  if (argc != 3) {
    printf("Error: Usage: ./cp2-problem3 [str1] [str2]\n");
    return 0;
  }

  char *str1 = (char *) argv[1];
  char *str2 = (char *) argv[2];
  char *str3 = (char *) malloc(strlen(argv[1])+strlen(argv[2])+1);

  str_cat(str1, str2, str3);
  printf("The concatenation is: %s\n", str3);

  int t = count_lowercase(str3);
  printf("There are a total of %d lowercase characters.\n", t);

  reverse(str3);
  printf("The reverse is: %s\n", str3);
}

/**
 * This function takes in three char*s, and should concatenate the
 * first two and store the result in the third.  For example, if the
 * value of str1 is "foo" and str2 is "bar", you should store the
 * string "foobar" into dst.  Be careful about the trailing \0s.  
 * You can assume that dst has sufficient space for the result (but
 * no more).
 */
 int string_length(char str[]) { //helper function to calc strlen
  int i = 0;
  while(str[i] != '\0') {
    i++;
  }
  return i;
}

void str_cat(char *str1, char *str2, char *dst) {
  int size1 = string_length(str1);
  int size2 = string_length(str2);
  int size = size1 + size2 + 1;
  snprintf(dst, size, "%s%s", str1, str2);
  return;
}

/**
 * This function should count the number of lowercase letters ('a' .. 'z') 
 * that occur in the argument and return the result.
 */
int count_lowercase(char *str) {
  int acc = 0;
  int i = 0;
  while(str[i] != '\0') {
    if (str[i] >= 'a' && str[i] <= 'z') {
	  acc++;
	  }
    i++;
  }
  return acc;
}

/**
 * This function should reverse the string argument *in-place*.  In other
 * words, you should change the characters in str so that they are in the 
 * reverse order.
 */
void reverse(char *str) {
  int i;
  char temp;
  int l = strlen(str);
  
  for (i = 0; i < (l/2); ++i) {
     temp = str[i];
     str[i] = str[l - i -1];
     str[l - i -1] = temp;
  }
}
