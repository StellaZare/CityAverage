#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

/* ---------- Constants and global variables ---------- */

#define TRUE 1
#define FALSE 0

/* ---------- Helper functions ---------- */

/* ---------- Main function ---------- */
int main(int argc, char* argv[]){

    for(int i = 0; i < argc; i++){
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}