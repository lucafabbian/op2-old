
// include guard
#ifndef TSUTILS_H
#define TSUTILS_H


#include <stdlib.h>
#include <time.h>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <random>


#ifdef NODEBUG
// Define a debug macro that does nothing
#define debug(fmt, ...) \
    do { (void)(fmt); } while(0)

#else
// Define a debug macro similar to printf
#define debug(fmt, ...) \
    printf(fmt, ##__VA_ARGS__)
#endif

#define timecost(a) printf("TIMECOST:%ld %d\n", time(NULL), a)

#ifndef DEFAULT_TIMELIMIT
#define DEFAULT_TIMELIMIT 60* 10
#endif


std::random_device rd;
std::mt19937 gen(rd());

// Function to generate a random 32-bit integer
uint32_t random_uint32() {
    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
    return dist(gen);
}

// Function to generate a UUID version 4
std::string generate_uuid() {
    // Generate random numbers for each of the fields
    uint32_t time_low = random_uint32();
    uint32_t time_mid = random_uint32();
    uint32_t time_hi_and_version = random_uint32() & 0xFFFF0FFF | 0x00004000; // Set the version bits to 4
    uint32_t clock_seq_hi_and_reserved = random_uint32() & 0x3FFF | 0x8000; // Set the variant bits to 10
    uint32_t clock_seq_low = random_uint32();
    uint32_t node = random_uint32();

    // Format the UUID as a string
    char uuid[37];
    snprintf(uuid, sizeof(uuid), "%08X-%04X-%04X-%04X-%04X%08X",
             time_low, time_mid, time_hi_and_version,
             clock_seq_hi_and_reserved, clock_seq_low, node);

    return uuid;
}


// swap two elements
void swap(int *a, int *b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

// swap two elements inside an array
void swap(int *a, const int i, const int j){
	int temp = a[i];
	a[i] = a[j];
	a[j] = temp;
}


// Function to perform Fisher-Yates shuffle
void shuffleArray(int *arr, int size) {

    // Start from the last element and swap one by one
    for (int i = size - 1; i > 0; i--) {
        // Generate a random index between 0 and i (inclusive)
        int j = rand() % (i + 1);

        // Swap arr[i] with the element at the random index
        swap(&arr[i], &arr[j]);
    }
}


int randomExponentialIndex(int arraySize, double lambda = 0.5) {
    double u = (double)rand() / RAND_MAX;
    return (int)(-log(1 - u) / lambda) % arraySize;
}


int randomLinearIndex(int arraySize) {
    int randomValue = rand() % ((int) (arraySize * (arraySize + 1))/2); // Generate a random number between 0 and cumulativeSum - 1

    int cumulativeSum = 0;
    for (int i = arraySize; i > 0; i--) {
        cumulativeSum += i;
        if (randomValue < cumulativeSum) {
            return arraySize - i; // Return the index
        }
    }
    // This should not be reached under normal circumstances
    return -1;
}


void printTime(time_t start){
  time_t end = time(NULL);
  int time = end- start;
  debug("time: %d\n", time);
}


double rand01(){
    return ((double) rand() / RAND_MAX);
}



#endif