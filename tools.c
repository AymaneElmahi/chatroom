#include "tools.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

void trim_newline(char *text)
{
  int len = strlen(text) - 1;
  if (text[len] == '\n')
  {
    text[len] = '\0';
  }
}

void clear_stdin_buffer()
{
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    /* discard content*/;
}

/* FUNCTION MEANT FOR GROUP PRIVATE MESSAGE */
// //
// int str_split(char *a_str, const char a_delim, char **result)
// {
//   int count = 0;
//   char *tmp = a_str;
//   char *last_comma = 0;
//   char delim[2];
//   delim[0] = a_delim;
//   delim[1] = 0;

//   /* Count how many elements will be extracted. */
//   while (*tmp)
//   {
//     if (a_delim == *tmp)
//     {
//       count++;
//       last_comma = tmp;
//     }
//     tmp++;
//   }

//   /* Add space for trailing token. */
//   count += last_comma < (a_str + strlen(a_str) - 1);

//   /* Add space for terminating null string so caller
//      knows where the list of returned strings ends. */
//   count++;

//   result = malloc(sizeof(char *) * count);

//   if (result)
//   {
//     int idx = 0;
//     char *token = strtok(a_str, delim);

//     while (token)
//     {
//       assert(idx < count);
//       *(result + idx++) = strdup(token);
//       token = strtok(0, delim);
//     }
//     assert(idx == count - 1);
//     *(result + idx) = 0;
//   }

//   return count;
// }
