/* A login system with a mysterious problem... */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define REQUIRED_PASSWORD "penguin"

int main(void)
{
    char* input = (char*) malloc(8 * sizeof(char));
    char* password = (char*) malloc(8 * sizeof(char));

    if (input > password) {
        char* ptr = input;
        input = password;
        password = ptr;
    }

    /* Copy required password into buffer */

    strcpy(password, REQUIRED_PASSWORD);

    /* Obtain password from user */

    #ifdef VISIBLE
      printf("Enter your password: ");
      fgets(input, 8, stdin);
    #else
      char* tmp = getpass("Enter your password: ");
      strncpy(input, tmp, 7);
    #endif

    /* Make sure strings are null-terminated */

    password[7] = input[7] = '\0';

    /* Authenticate by comparing acquired and stored passwords */

    if (strcmp(password, input) == 0) {
        printf("\nLogin succeeded.\n\n");
        return 0;
    }
    else {
        printf("\nLogin failed!\n\n");
        return 1;
    }
}
