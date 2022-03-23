#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PROCES_COPIL 0

/* setez procesul parinte sa dea cat la fisierul output */
/* setez procesul copil sa faca comanda ls > output */
int main() {

    /* Create a new proces (HINT:fork()) */
    pid_t ls_proc = fork();
    if(ls_proc == 0) {
        /* Child process */
        char* command[] = {"ls",  NULL};
        execv("/bin/ls", command);
        exit(0);
    } 
    if(ls_proc > 0) {
        int status;
        pid_t wait_child = waitpid(ls_proc, &status, 0);
        if(wait_child != -1)
            printf("TAKE CARE SON\n");
    }
    
    return 0;   
}
