#include "validation.h"

// Check the length of the string
int lenght_check(int length)
{
	if (length > 30) // Check if the string is longer than 30 characters
	{
		printf("The inserted string exceeds the maximum limit (64 characters maximum)\n");
		return 1;
	}
	return 0;
}

// Check if the operator is correct
int operator_check(char* str)
{
	if (strchr("+-xX*/", str[0]) == NULL) // First token is not an operand
	{
		printf("An incorrect operator was entered, check the format\n");
		return 1;
	}
	return 0;
}

// Check if the operands (numbers) are correct
int num_check(char* str)
{
	for (unsigned int i = 0; i < strlen(str); i++)
	{
		if (!isdigit(str[i]))
		{
			printf("The operands entered are not in the correct format.\n");
			if (str[i] == ' ')
				printf("Please ensure there are no spaces after the last character.\n");

			return 1;
		}
	}

	return 0;
}

// Check if the string is in the correct format
// Returns 0 if the string is correct, 1 if incorrect
int string_check(const char* input_string)
{
	char str[MAX];
	strcpy(str, input_string);

	if (!strcmp(str, "=") == 0)
	{
		char* token = strtok(str, " ");
		unsigned int check_op_number = 0;
		for (unsigned int i = 1; i <= 3 && token != NULL; i++)
		{
			switch (i)
			{
				case 1:
					if (operator_check(token)) // Check if the first token is a valid operator
						return 1;
					token = strtok(NULL, " ");
					break;

				case 2:
					if (num_check(token)) // Check if the second token is a valid number
						return 1;
					token = strtok(NULL, "\n");
					break;

				case 3:
					if (num_check(token)) // Check if the third token is a valid number
						return 1;
					check_op_number = 1;
					break;
			}
		}
		if (check_op_number == 0) // Less than two operands have been entered
		{
			printf("Fewer than two operands have been entered\n");
			return 1;
		}
	}

	return 0;
}