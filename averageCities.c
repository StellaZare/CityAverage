#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

/* ---------- Constants and global variables ---------- */

#define TRUE 1
#define FALSE 0
#define MAX_CITIES 10
#define MAX_LENGTH 100

pthread_mutex_t mutex;
float max_temp_overall;
char max_temp_location[100];
float min_temp_overall;
char min_temp_location[100];
float sum_overall;
int num_entries_overall;

char cities[MAX_CITIES][MAX_LENGTH] = {"Charlottetown.dat", "Edmonton.dat", "Halifax.dat", "Montreal.dat",
    "Ottawa.dat", "Quebec.dat", "Toronto.dat", "Vancouver.dat", "Victoria.dat", "Winnipeg.dat"};

/* ---------- Helper functions ---------- */
void printData(char filename[], float min_temp, float max_temp, float sum_temp, int num_entries){
    pthread_mutex_lock(&mutex);
    if (max_temp > max_temp_overall){
        max_temp_overall = max_temp;
        strcpy(max_temp_location, filename);
    }
    if(min_temp < min_temp_overall){
        min_temp_overall = min_temp;
        strcpy(min_temp_location, filename);
    }
    sum_overall += sum_temp;
    num_entries_overall += num_entries;

    printf("===========================================\n");
    printf("File                %s\n", filename);
    printf("Number of entries   %d\n", num_entries);
    printf("Min temprature      %.2f degrees\n", min_temp);
    printf("Max temprature      %.2f degrees\n", max_temp);
    printf("Average temprature  %.2f degrees\n", sum_temp/(2*num_entries));
    pthread_mutex_unlock(&mutex);
}

void* processCity(char filename[]){
    FILE* file = fopen(filename, "r");
    if(!file){
        printf("Error opening file %s", filename);
        exit(1);
    }
    float data[2];
    char line[MAX_LENGTH];
    float min_temp = 100;
    float max_temp = -100;
    float sum_temp = 0;
    int num_entries = -1;

    while(fgets(line, MAX_LENGTH, file)){
        sscanf(line, "%f %f", &data[0], &data[1]);
        max_temp = (data[0] > max_temp) ? data[0]: max_temp;
        min_temp = (data[1] < min_temp) ? data[1]: min_temp;
        sum_temp = sum_temp + data[0] + data[1];
        num_entries++;
    }
    fclose(file);
    printData(filename, min_temp, max_temp, sum_temp, num_entries);
    return NULL;
}

/* ---------- Main function ---------- */
int main(int argc, char* argv[]){
    clock_t start_time, end_time;
    int multithread = FALSE;
    pthread_mutex_init(&mutex, NULL);
    
    start_time = clock();
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], "-m") == 0)
            multithread = TRUE;
    }

    if(multithread){
        printf("Running with multiple threads\n");
        pthread_t threads[MAX_CITIES];
        for(int idx = 0; idx < MAX_CITIES; idx++){
            char file_path[MAX_LENGTH] = "data/";
            strcat(file_path, cities[idx]);
            int return_value = pthread_create(&threads[idx], NULL, (void*)processCity, strdup(file_path));
            if (return_value) {
                printf("Error unable to create thread %s\n", file_path);
                exit(1);
            }
        }
        for(int idx = 0; idx < MAX_CITIES; idx++){
            pthread_join(threads[idx], NULL);
        }

    }else{
        printf("Running in a single thread\n");
        for(int idx = 0; idx < MAX_CITIES; idx++){
            char file_path[MAX_LENGTH] = "data/";
            processCity(strcat(file_path, cities[idx]));
        }   
    }

    pthread_mutex_destroy(&mutex);
    end_time = clock();
    printf("===========================================\n");
    printf("Number of files     %d\n", MAX_CITIES);
    printf("Number of entries   %d\n", num_entries_overall);
    printf("Min temprature     %.2f degrees in %s\n", min_temp_overall, min_temp_location);
    printf("Max temprature      %.2f degrees in %s\n", max_temp_overall, max_temp_location);
    printf("Average temprature  %.2f degrees\n", sum_overall/(2*num_entries_overall));
    printf("Clock cycles        %ld\n", end_time - start_time);
    return 0;
}
