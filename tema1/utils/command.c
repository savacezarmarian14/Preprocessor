#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// -I 

char** listOfDirectories(int argc, char* argv[], int* includes) {
  char** result = NULL;
  (*includes) = 0;
  for(int i = 0; i < argc; i++) {
    if (strstr(argv[i], "-I") != NULL) {
      /* verific daca are lungime 2 sau mai mare */
      if (strlen(argv[i]) == 2) { // daca are lungime 2
        if( result == NULL ) { // daca nu am bagat nimic inca in result
          result = malloc(sizeof(char*));
          (*includes)++;
          result[0] = strdup(argv[++i]);
        }
        else { // daca am bagat deja in result
        //  int len_result = (int) (sizeof(result) / sizeof(*result));
          result = realloc(result, ((*includes) + 1) * sizeof(*result));
          (*includes)++;
          result[((*includes) - 1)] = strdup(argv[++i]); // ++i doarece argumentul este in celula urm asa ca trebuie sa avansam cu o unitate
        }
      }
      /* daca e mai amre lungimea de 2 adica daca e -I<path> nu -I <path> */
      else if(strlen(argv[i]) > 2) {
        if( result == NULL) {
          result = malloc(sizeof(char*));
          (*includes)++;
          result[0] = strdup(argv[i] + 2);
        }
        else {
         // int len_result = (int) (sizeof(result) / sizeof(*result));
          result = realloc(result, ((*includes) + 1) * sizeof(*result));
          (*includes)++;
          result[(*(includes) - 1)] = strdup(argv[i] + 2);
        }
      } 
    }
  }
  return result;
}


