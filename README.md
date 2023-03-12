# CityAverage

## To run program
To compile:
```
make
```

To run in single thread:
```
./averageCities
```

To run with multiple threads:
```
./averageCities -m
```

To clean:
```
make clean
```

## Functions
### Main
The main function takes as input the commandline information passed when executing the program.
```C
int main(int argc, char* argv[]){...}
```
The function then determines if the "-m" tag was passed or not. 
If the "-m" tag is present the function loops through a list of file names and processes each in a new thread using the pthread library.
```C
pthread_create(&threads[idx], NULL, (void*)processCity, strdup(file_path));
```
Then all threads are joined to the main thread using a loop to ensure they all terminate before the program continues.
```C
pthread_join(threads[idx], NULL);
```
If the "-m" tag was not given the program will execute in a single thread by calling the processCity function on each filename in the array one after the other.
```C
for(int idx = 0; idx < MAX_CITIES; idx++){
  char file_path[MAX_LENGTH] = "data/";
  processCity(strcat(file_path, cities[idx]));
}
```
Finally, the main function prints out the overall data regardless of the multiplicy of the threads up to this point.
This includes:
* Number of files analyzed
* Total number of entries
* Min temp overall and its location
* Max temp overall and its location
* Average temo overall 
* Clock cycles 

### processCity
The processCity function takes as input a char array object containign the target file's path from the current directory. (ie. data/Victoria.dat)
```C
void* processCity(char filename[]){ ... }
```
The function opens the file and reads the data using a while loop and the functions fgets() and sscanf(). For each line in the file the values are checked against a local variables max_temp and min_temp. Then the values are added to the sum_temp which is later used to calculate the average and the num_entries is incremented.
```C
while(fgets(line, MAX_LENGTH, file)){
  sscanf(line, "%f %f", &data[0], &data[1]);
  max_temp = (data[0] > max_temp) ? data[0]: max_temp;
  min_temp = (data[1] < min_temp) ? data[1]: min_temp;
  sum_temp = sum_temp + data[0] + data[1];
  num_entries++;
}
```
processCity then calls printData.

### printData
The function takes as input all the information that needs to be outputted for a particulat file. 
```C
void printData(char filename[], float min_temp, float max_temp, float sum_temp, int num_entries){...}
```
The function then locks the mutex so it can enter its critical section: reading and writing to global variables and printing the data to stout.
```C
pthread_mutex_lock(&mutex);
//critical section
pthread_mutex_unlock(&mutex);
```
Once the mutex has bee acquired the function compared the overall values with those of the specific file and updates global values are required. 
The the function prints the data relating to the specific file, before releasing the mutex.

For each city file the function prints the followign data points:
* Name of the file
* Number of entries (lines) in the file
* Min temp in file
* Max tmep in file
* Avg temp reported (sum of all values / 2*num_entries)

## Error Handling
### Main
There is error handling for the call to pthread_create()

### processCity
There is error handling for the call to fopen()

## Single vs. Multithreaded Processing
When ./averageCities is run as a single threaded process each file is opened, read and data printed before the next file is processed. 
alternatively when ./averageCities is run as a multithreaded process each file is processed in an individual thread; meaning all 10 files are read in parallel but they each wait for their turn to enter their critical sections by acuiring the mutex. We can also observe the order in which the cities print their output in the multithreaded process can vary as threads may access the mutex in any order.

We can validate the threading is being done correctly by comparing the output data generated with a single threaded process with that of the multithread process. In this case we see that the min temp, max temp, average and number of entries is the same.

By tracking the number of clock cycles we can compare the efficently of both approaches.
```C
clock_t start_time, end_time;
start_time = clock();
// all the code
end_time = clock();
```
Average clock cycles for single thread after 10 trials: 170751.90
Average clock cycles for multi- thread after 10 trials: 177916.10

We can see for this program the single threaded approach takes on average 7000 fewer clock cycles than the multi-threaded solution.
This is normal and could be attributed to the overhead costs of context switches between PCB in the multithreaded solution.

## Linux vs. MacOS
We can also compare the performance of the program on various operating systems. 
In the previous section I compared the average clock cycle of the single-threaded solution and the multithreaded solution running on the Linux server.

I can also run the program on my person computer (MasOS) to observe how the performance is affected.

Average clock cycles for single thread after 10 trials: 126114.50
Average clock cycles for multi- thread after 10 trials: 311203.40

We can see that the single threaded solution is using few clock cycles than the multi-threaded. A trend we observed in the previosu section as well. 
However we can also see that the single threaded solution performs considerably better on MacOS, while the multi-threaded solution performs better on Linux. This could be due to either operating system's unique shecudlign algorithms.

