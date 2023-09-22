#define IPYCPP
// $$ipycpp_file:   src/main.cpp
// $$ipycpp_build:  bash build.sh
// $$ipycpp_run:    bin/main
// $$ipycpp_clevermain: true
#define NODEBUG
#include "include.cpp"
const auto DEFAULT_TSPFILE_SMALL = "./data/pr439.tsp";
const auto DEFAULT_TSPFILE_LARGE = "./data/pr1002.tsp";


// Example of running the nearest neighbor algorithm for each starting point.

int main(){
  TSP tsp(DEFAULT_TSPFILE_LARGE);
  TSPSolution sol(&tsp);
  time_t startTime = time(NULL);
  extraMileage(&sol, startTime + 60, 0);
  printf("Seconds: %ld\n", (time(NULL) - startTime));
  printf("cost = %d\n", sol.cost);
}
