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
  for(int i = 0; i < argc; i++) {
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
	char caracter;
	int i = 0;
	while((fread(&caracter, sizeof(char), 1, target)) > 0) {
		if((int)caracter == ((int)'\n'))
			caracter = 0;
		buffer[i++] = caracter;
		if(caracter == 0)
			return i - 1;
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

void deleteLine(char ***page, int *size, int line) {
	for(int i = line; i < *size - 1; i++)
		(*page)[i] = strdup((*page)[i + 1]);
	*size -= 1;
}

void setDefineMap(HashMap h, char*** lines, int *no_lines) {
  for( int i = 0; i < *no_lines; i++) {
    char* line = strdup((*lines[i])); //preiau linia curenta din 'pagina'

    if(line[0] == '#') { // verific daca prima litera este '#'
      int no_words = 0; // numarul de cuvinte pe rand #define A 2 -> (define, A, 2) (char*)
      char **words = parseLine(line, "# ", &no_words); // chiar (define, A, 2)

      if(isDefine(words)) { // in caz ca pe rand am #include sau #if
        char *aux_value = no_words == 2 ? strdup("") : strdup(words[2]);
        putInHashMap(h, words[1], aux_value);
    
        for(int j = i; j < *no_lines - 1; j++)
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


DIR** openAllDirectories(char **directories_name, int no_directories) {
  DIR** directories = (DIR**) malloc(no_directories * sizeof(int));
  for(int i = 0; i < no_directories; i++) 
    directories[i] = opendir(directories_name[i]);
  return directories;
}

 /* \/---Am facut aceasta functie doar pt a testa rewinddir() */
void printFiles(DIR* directory) {
  struct dirent* iterator;
  while((iterator = readdir(directory)) != NULL)
    printf("%s\n", iterator->d_name);
  rewinddir(directory);
}

FILE* getFile(char* file_name, char* dir_name, DIR* directory) {
  struct dirent* iterator;
  while((iterator = readdir(directory)) != NULL) {
    if(strcmp(file_name, iterator->d_name) == 0) {
      char* file_path = strdup(dir_name);
      strcat(file_path, "/");
      strcat(file_path, file_name);
      FILE* fd = fopen(file_path, "r");
      return fd; 
    }
  }
  return -1;
}
char* getFullPath(char* directory, char* file) {
  char *dir = strdup(directory);
  strcat(dir, "/");
  strcat(dir, file);
  return dir;
}

char** getIncludeFile(char* header_name,char **dirs_name, DIR** dirs, int no_dirs,
    int* len) {
  //struct dirent* object;
  char **buffer = NULL;
  FILE* header_file;
  for(int i = 0; i < no_dirs; i++) {
    struct dirent* object;
    while((object = readdir(dirs[i])) != NULL) {
      if(strcmp(object->d_name, header_name) == 0) {

        header_file = fopen(getFullPath(dirs_name[i], header_name), "r");
        rewinddir(dirs[i]);
        break;
      }
    }
    rewinddir(dirs[i]);
  }
  int rc, no_lines = 0;
  char *line = malloc(MAX_LINE_LEN * sizeof(char));
  while((rc = myGetLine(header_file, line)) != -1)
      addLine(line, &buffer, no_lines++);
  (*len) = no_lines;
  return buffer;

}
  

void solveDefine(HashMap h, char*** page, int no_lines) {
  Node target = popFirst(h);
  
 
 //iterez prin (*page) pentru a separa cuvintele intre " "
  if(target != NULL) 
    for(int i = 0; i < no_lines; i++) {
    char* line = strdup((*page)[i]); // copiez linia curenta in "line"
    int no_words = 0; //numarul de cuvinte din linie
  
    char ** words = parseLine(line, "\"", &no_words); // lista de cuvinte
    char* buffer = calloc(MAX_LINE_LEN, sizeof(char));
    for (int j = 0; j < no_words; j++) {
      
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

void rewindAllDir(DIR** dirs, int no_dirs) {
  for(int i = 0; i < no_dirs; i++)
    rewinddir(dirs[i]);
}

int main(int argc, char* argv[]) {
	FILE* target_file = fopen("example.c", "rw");

	int rc = 0;
	int lines = 0;
	char*  buffer = malloc(MAX_LINE_LEN * sizeof(char));
  int no_directories = 0;
  /*-------------Crearea unei liste de directoare -------*/

  char** directories = listOfDirectories(argc, argv, &no_directories);

  /*-----------------------------------------------------*/
  
  /*-------------Deschiderea directoarelor---------------*/

  DIR** dirChannel = openAllDirectories(directories, no_directories);
  int nn = 0;
  char** incl = getIncludeFile("hashmap.h", directories, dirChannel, no_directories, &nn);
  for(int i = 0; i < nn; i++)
    printf("----------------------%s\n", incl[i]);
  rewindAllDir(dirChannel, no_directories);
  incl = getIncludeFile("list.h", directories, dirChannel, no_directories, &nn);
  for(int i = 0; i < nn; i++)
    printf("----------------------%s\n", incl[i]);
  ///TODO TODO TODO 
  /*------------Citirea fievarei linii din fisierul de parsat------------------------------*/

  char** file_container = NULL;
	while( (rc = myGetLine(target_file, buffer)) != -1) {
		addLine(buffer, &file_container, lines++);  // adaug linia in vectorul de linii
  }

  /*---------------------------------------------------------------------------------------*/

/*--------crearea unu hashmap pentru define-uri key->value cu un checksum drept hf-------*/	

	//Am creat in utils doua headere hashmap.h si list.h simple//
	HashMap defineMap = createHashMap(25, &BASIC_HASH_FUNCTION);
  
	for(int i = 0; i < lines; i++) {
		char *current_line  = strdup(file_container[i]); // folosesc o linie auxiliara ca sa nu stric nimic
		if(current_line[0] == '#') {
			int no = 0;
			char **words = parseLine(current_line, "# ", &no); // am separat linia de in functie de '#' si ' ' 
			if(isDefine(words)) {
				char* value;
				if(no == 2)
					value = strdup(""); // in caz ca nu are VALUE
				else
				  value = strdup(words[2]); // in caz ca are si VALUE
				putInHashMap(defineMap, words[1], value); // adaug define in hashmap
				deleteLine(&file_container, &lines, i); // sterg linia #define KEY VALUE
				i--; // raman pe loc cu cautarea
			}	
		}
	} 
  
  addDefineArgs(defineMap, argc, argv); // adaug define urile date ca argumente in linia de comanda
/*----------------------Am adaugat toate define-urile in hashmap ------------*/


  while(defineMap->hashmap_no_values > 0)
    solveDefine(defineMap, &(file_container), lines);

  for(int i = 0; i < lines; i++)
   printf("%s\n", file_container[i]);


  return 0;
}

