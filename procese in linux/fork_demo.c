#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define PROCES_COPIL 0


int main() {

    pid_t PID = fork(); // se apeleaza odata si se intoarce de doua ori 0 in COPIL pid_copil in PARINTE
   
    if (PID == PROCES_COPIL) {
        printf("Eu sunt procesul parinte\n");
    } else {
        printf("Procesul parinte are ca proces copul procesul : %d\n", PID);
    }

    return 0;   
}
