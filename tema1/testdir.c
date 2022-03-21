#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

typedef struct DirMap {
  DIR** directory_stream;
  char* dir_name;
  FILE** listOfFiles;
}*DirMap;

DirMap *dirList(char **listOfDirectories, int no_directories) {
  DirMap *list = malloc(sizeof(DirMap));
  
  for(int i = 0; i < no_directories; i++) {
    printf("%s\n", listOfDirectories[i]);
    DIR* d = opendir(listOfDirectories[i]);
    list[i]->directory_stream = d;
    list[i]->dir_name = strdup(listOfDirectories[i]);
  }
  return list;
}

int main() {
  
  char **dirs = malloc(sizeof(char*));
  dirs[0] = strdup("utils");
  DirMap* list = dirList(dirs, 1);

  for(int i = 0; i < 1; i++) {
    struct dirent* object;
    while((object = readdir(list[i]->directory_stream)) != NULL)
      printf("%s\n", object->d_name); 
  }
  return 0;
}
