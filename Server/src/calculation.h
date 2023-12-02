#include <stdio.h>
#include <string.h>
#include<ctype.h>
#include <stdlib.h>

#ifndef NULL
#define NULL   ((void *) 0)
#endif

int set_operation(char *str, int *n1, int *n2, char *op);       // Function that sets the operation values based on the input string
float calculation(int n1, int n2, char op);                     // Function that performs the calculation
float add(int a, int b);                                        // Function that adds two numbers
float sub(int a, int b);                                        // Function that subtracts two numbers                          
float mult(int a, int b);                                       // Function that multiplies two numbers
float division(int a, int b);                                   // Function that divides two numbers               
