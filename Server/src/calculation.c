#include "calculation.h"

// Function that sets the operation values based on the input string
int set_operation(char* str, int* n1, int* n2, char* op)
{
   // Tokenize the input string using space as the delimiter
   char* token = strtok(str, " ");
   for (unsigned int i = 1; (i <= 3) && (token != NULL); i++)
   {
      switch (i)
      {
         case 1:
            // Set the operation character
            strcpy(op, token);
            break;

         case 2:
            // Convert and set the first number
            *n1 = atoi(token);
            break;

         case 3:
            // Convert and set the second number
            *n2 = atoi(token);
            break;
      }
      token = strtok(NULL, " ");
   }

   return 0;
}

// Function that performs the calculation
float calculation(int n1, int n2, char op)
{
   switch (op)
   {
      case '+':
         return add(n1, n2);
      case '-':
         return sub(n1, n2);
      case 'x':
      case 'X':
      case '*':
         return mult(n1, n2);
      case '/':
         return division(n1, n2);
   }
   return 0;
}

float add(int a, int b)
{
   return (float)a + (float)b;
}

float sub(int a, int b)
{
   return (float)a - (float)b;
}

float mult(int a, int b)
{
   return (float)a * (float)b;
}

float division(int a, int b)
{
   return (float)a / (float)b;
}
