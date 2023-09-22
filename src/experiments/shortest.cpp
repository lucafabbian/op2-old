
#include "utils.cpp"
#include <stdio.h>
#include <algorithm>
#include <cmath>


void printArray(int *array, int size) {
  for(int i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }

  printf("\n");
}

int main(int argc, char *argv[]) {

  const int size = 1000;
  int array1[size];
  int array2[size];

  for (int i = 0; i < size; i++) {
    array1[i] = i;
    array2[i] = i;
  }

  shuffleArray(array1, size);
  shuffleArray(array2, size);

  printf("array1: ");
  printArray(array1, size);
  printf("array2: ");
  printArray(array2, size);



  int array2index[size];

  for(int i = 0; i < size; i++){
    array2index[array2[i]] = i;
  }

  int overlapping[size];

  double totalBestValue = 0;
  int totalStart, totalLen, totaOverlap, totalOvLen;

  for(int sLen = 5; sLen < size - 5; sLen++){
    for(int start = 0; start < size; start++){
      int i = 0;
      for(int c = start; c < start + sLen; c++){
        overlapping[i++] = array2index[array1[c % size]];
      }

      std::sort(overlapping, overlapping + i);

      int bestOverlap = overlapping[0];
      int overlapLength = overlapping[0] + size - overlapping[i -1] -1;
      for(int j = 1; j < i; j++){
        int overlap = overlapping[j] - overlapping[j- 1] - 1;
        if(overlap > overlapLength){
          overlapLength = overlap;
          bestOverlap = overlapping[j];
        }
      }

      //printf("overlapLength: %d\n", overlapLength);

      //double value = sqrt(sqrt(overlapLength)) + sqrt(sqrt(sLen));
      double value = std::min(overlapLength, sLen);
      if(value > totalBestValue){
        totalBestValue = value;
        totalStart = start;
        totalLen = sLen;
        totaOverlap = bestOverlap;
        totalOvLen = overlapLength;
        printf("total: val%f start%d len%d over%d overL%d\n", totalBestValue, totalStart, totalLen, totaOverlap, totalOvLen);
      }
    }
  }


}