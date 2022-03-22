#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "utils/hashmap.h"
#include "utils/command.h"
#include <sys/types.h>
#include <dirent.h>
#define MAX_LINE_LEN 128

char** parseLine(char*, const char*, int*);


void addDefineArgs(HashMap h, int argc, char* argv[]) {
  int i;
  for(i = 0; i < argc; i++) {
    if(strlen(argv[i]) == 2)  {//-D [key][=value]
      if(strcmp(argv[i], "-D") == 0) {
        char *aux = strdup(argv[++i]);
        int n; // nu o sa l folosesc vreodata ( > /dev/null )
      
        char **key_value = parseLine(aux, "=", &n);
       
        putInHashMap(h, key_value[0], key_value[1]);
      }
    }
    else if(strlen(argv[i]) > 2) { //-D[key][=value]
      char* prefix = strdup(argv[i]);
      prefix[2] = 0;
      
      if(strcmp(prefix, "-D") == 0) {
        char* aux = strdup(argv[i]);
        aux = aux + 2; // mut pointerul cu 2 unitati pentru a separa -D de resul argumentului
        int n; // nu o sa l folosesc vreodata 
        char **key_value = parseLine(aux, "=", &n);
        putInHashMap(h, key_value[0], key_value[1]); // introduc in hashmap key - value
      }
    }
  } 
}
/* 
 *  myGetLine - INT
 *  --------------------------------------------------------------------------------------
 *  Aceasta functie citeste din fisierul "target" litera cu litera (de la cursor spre eof)
 *  pana cand ajunge la un endline ( '\n' ). Rezultatul il stocheaza "buffer"
 *  La citirea caracterului EOS ( '\0' ) functia intoarce numarul de caractere 
 *  pe care le are randul.
 *  In cazul in care citirea nu se efectueaza in conditii normale , functia intoarce -1
 *  --------------------------------------------------------------------------------------
 */

int myGetLine(FILE* target, char* buffer) {
	char caracter = ' ';
	int i = 0;
	while(target != NULL &&fread(&caracter, sizeof(char), 1, target)) {
		if((int)caracter == ((int)'\n')){
      buffer[i++] = caracter;
      buffer[i] = 0;
      return i;
    }
    buffer[i++] = caracter;
	}

	return -1;
}

/* 
 *  addLine - Void
 *  --------------------------------------------------------------------------------------
 *  Aceasta functie adauga "line" la "buffer" la pozitia "index"
 *  --------------------------------------------------------------------------------------
 */

void addLine(char* line, char*** buffer, int index) {
	if(*buffer == NULL) {
		*buffer = malloc(sizeof(char**));
	}
	*buffer = (char**) realloc((*buffer), (index + 1) * sizeof(char*));
	(*buffer)[index] = (char*) malloc(MAX_LINE_LEN * sizeof(char));
	(*buffer)[index] = strdup(line);
}

/* parseLine - CHAR** 
 * --------------------------------------------------------------------------------------
 * Aceasta functie parseaza liniile din fisier in functie de delimitatorii primiti
 * Return - (char**) cuvintele ce alcatuiesc linia
 * ------------------------------------------------------------------------------------
 */

char** parseLine(char *line, const char* delim, int* n) {
	char* word = strtok(line, delim);
	char** word_list = malloc(sizeof(char**));
	int word_number = 0;
	while(word != NULL) {
		addLine(word, &word_list, word_number++); 
		word = strtok(NULL, delim);
	}
	*n = word_number;
	return word_list;
}

int isDefine(char **words) {
	if(strcmp(words[0], "define")  == 0)
		return 1;
	return 0;
}

int isInclude(char **words) {
	if(strcmp(words[0], "include")  == 0)
		return 1;
	return 0;
}

void deleteLine(char ***page, int *size, int line) {
  int i;
	for(i = line; i < *size - 1; i++)
		(*page)[i] = strdup((*page)[i + 1]);
	*size -= 1;
}

void setDefineMap(HashMap h, char*** lines, int *no_lines) {
  int i, j;
  for(i = 0; i < *no_lines; i++) {
    char* line = strdup((*lines[i])); //preiau linia curenta din 'pagina'

    if(line[0] == '#') { // verific daca prima litera este '#'
      int no_words = 0; // numarul de cuvinte pe rand #define A 2 -> (define, A, 2) (char*)
      char **words = parseLine(line, "# ", &no_words); // chiar (define, A, 2)

      if(isDefine(words)) { // in caz ca pe rand am #include sau #if
        char *aux_value = no_words == 2 ? strdup("") : strdup(words[2]);
        putInHashMap(h, words[1], aux_value);
    
        for(j = i; j < *no_lines - 1; j++)
          (*lines)[j] = strdup((*lines)[j + 1]);
    
        *no_lines -= 1;
        i--;
      }
    }
  }
}

char* swapkey(char* initial_word, Node node) {
  char* v = strdup(initial_word);
  char* p = strdup(node->key);
  char* change = strdup(node->value);

  char* result = strstr(v, p);
  if(result != NULL) {
    result[0] = '\0';
    result += strlen (p);

    char * aux = strdup(v);
    strcat(aux, change);
    strcat(aux, result);

    return swapkey(aux,  node); // recursivitate pe coada kiss Mihai Nan ;)
  } 

   return v;
}




 /* \/---Am facut aceasta functie doar pt a testa rewinddir() */


char* getFullPath(char* directory, char* file) {
  char *dir = strdup(directory);
  strcat(dir, "/");
  strcat(dir, file);
  return dir;
}

void solveDefine(HashMap h, char*** page, int no_lines) {
  Node target = popFirst(h);
  int i ,j;
  if (target != NULL) 
    for (i = 0; i < no_lines; i++) {
    char* line = strdup((*page)[i]); // copiez linia curenta in "line"
    int no_words = 0; //numarul de cuvinte din linie
  
    char** words = parseLine(line, "\"", &no_words); // lista de cuvinte
    char* buffer = calloc(MAX_LINE_LEN, sizeof(char));
    for (j = 0; j < no_words; j++) {
      
      if(words[j] != NULL && j % 2 == 0){ // mereu define urile valide sunt pe numar par intre doua ' " '
        char* result = swapkey(strdup(words[j]), target);
 
        strcat(buffer, strdup(result));
       
        if( no_words > 1 && no_words > j + 1)
          strcat(buffer, "\"");  
      } 
      else if(j % 2 == 1) {
        strcat(buffer, strdup(words[j]));
        if(j != no_words - 1)
         strcat(buffer, "\"");
      }
    }

    (*page)[i] =  strdup(buffer);

  }
}


char* getIncludeName(char **args) {
  if(args[1][0] == '\"') {
    char *aux = strdup(args[1]);
    strcpy(aux, aux + 1);
    aux[strlen(aux) - 1] = 0;
    return aux;
  }
  return NULL;
}

char* inputFile(int argc, char** argv) {
  int i;
  if(argv[1][0] != '-')
    return strdup(argv[1]);
  else {
    for(i = 2; i < argc; i++) {
      if(argv[i][0] == '-' && strlen(argv[i]) == 2)
        i += 2;
      if(argv[i][0] != '-')
        return strdup(argv[i]);
    } 
  }
}
char* path(char* name, char* d_name){
  char *aux = strdup(d_name);
  strcat(aux, "/");
  strcat(aux, strdup(name));
  return aux;
}
FILE* getIncludePath(char* name, char** d_name, int d_no) {
  int i;
  for(i = 0; i < d_no; i++) {
    char* fullPath = path(name, d_name[i]);
    FILE *file = fopen(fullPath, "r");
    if(file != NULL)
      return file;
    fclose(file);
  }
  return NULL;
}

char** readFile(FILE* file, int* no_lines){
  (*no_lines) = 0;
  char** STRING_FILE = NULL;
  char *buffer = (char*) malloc(MAX_LINE_LEN * sizeof(char));
  int rc ;
  while((rc = myGetLine(file, buffer)) != -1) {
    addLine(buffer, &STRING_FILE, (*no_lines)++);
  }
  return STRING_FILE;
}

char** solveInclude(char** main_file, int lines, char** d_name, int d_no, int *count) {
  int i, j;
  char** new_buffer = NULL;
  int new_buffer_lines = 0;
  for(i = 0; i < lines; i++) {
    char* line = strdup(main_file[i]);
    int no_words;
    char** words = parseLine(line, "# \"", &no_words);
    if(isInclude(words) && words[1][0] != '<') {
      FILE* libfile = getIncludePath(words[1], d_name, d_no);
      int libsize;
      char** STRING_LIB_FILE = readFile(libfile, &libsize); 
      fclose(libfile);
      for(j = 0; j < libsize; j++) {
        if(STRING_LIB_FILE[j][0] != '\0')
          addLine(strdup(STRING_LIB_FILE[j]), &new_buffer, new_buffer_lines++);
      }
    } else if(main_file[i][0] != '\0') {
      addLine(strdup(main_file[i]), &new_buffer, new_buffer_lines++ );
    } 
  }
  *count = new_buffer_lines;
  return new_buffer;
} 

char* outputFile(int argc, char* argv[]) {
    int i;
    for (i = 0; i < argc; i++) {
      char *arg = strdup(argv[i]);
      if(strstr(arg, "-O") != NULL) {
        if(strlen(arg) == 2)
          return strdup(argv[i + 1]);
        else {
          strcpy(arg, arg + 2);
          return arg;
        }
      }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
  int i,j;
  char* input_file = inputFile(argc, argv);
	FILE* target_file = fopen(input_file,"r");
  char* output_file = outputFile(argc, argv);
  if(output_file != NULL)
    printf("output file : %s\n ", output_file);

	int rc = 0;
	int lines = 0;
	char*  buffer = malloc(MAX_LINE_LEN * sizeof(char));
  int no_directories = 0;
  /*-------------Crearea unei liste de directoare -------*/

  char** directories = listOfDirectories(argc, argv, &no_directories);

  /*-----------------------------------------------------*/
  
  /*-------------Deschiderea directoarelor---------------*/


  char** file_container = NULL;
	while( (rc = myGetLine(target_file, buffer)) != -1) {
      addLine(buffer, &file_container, lines++);  // adaug linia in vectorul de linii
  }
  if(target_file != NULL)
    fclose(target_file);
  int lastlines = 0;
  while(lines != lastlines) {
    lastlines = lines;
    file_container = solveInclude(file_container, lines, directories, no_directories, &lines);
  }

  

  /*---------------------------------------------------------------------------------------*/

/*--------crearea unu hashmap pentru define-uri key->value cu un checksum drept hf-------*/	

	//Am creat in utils doua headere hashmap.h si list.h simple//
	HashMap defineMap = createHashMap(25, &BASIC_HASH_FUNCTION);
  
	for(i = 0; i < lines; i++) {
		char *current_line  = strdup(file_container[i]); // folosesc o linie auxiliara ca sa nu stric nimic
		if(current_line[0] == '#') {
			int no = 0;
			char **words = parseLine(current_line, "# \n", &no); // am separat linia de in functie de '#' si ' ' 
			if(isDefine(words)) {
				char* value;
				if(no == 2)
					value = strdup(""); // in caz ca nu are VALUE
				else
				  value = strdup(words[2]); // in caz ca are si VALUE
				putInHashMap(defineMap, words[1], value); // adaug define in hashmap
				deleteLine(&file_container, &lines, i); // sterg linia #define KEY VALUE
				i--; // raman pe loc cu cautarea
        free(value);
			}	
      free(words);
		}
    free(current_line);
	} 
  
  addDefineArgs(defineMap, argc, argv); // adaug define urile date ca argumente in linia de comanda
/*----------------------Am adaugat toate define-urile in hashmap ------------*/


  while(defineMap->hashmap_no_values > 0)
    solveDefine(defineMap, &(file_container), lines);

  if(output_file == NULL)
    for(i = 0; i < lines; i++)
      printf("%s\n", file_container[i]);
  else {
    FILE* outputf =  fopen(output_file, "w");
    for (i = 0; i < lines; i++) {
      fwrite(file_container[i], strlen(file_container[i]) * sizeof(char), 1, outputf);
    }
    fclose(outputf);
  }

  freeHashMap(defineMap);
  for(i = 0; i < lines; i++)
    free(file_container[i]);
  free(file_container);
  free(directories);



  return 0;
}

