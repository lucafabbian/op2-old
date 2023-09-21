// include guard
#ifndef PERF_H
#define PERF_H

// include libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <limits.h>

#include "tsp.cpp"
#include "utils.cpp"


struct PerfFunction {
  const char *name;
  void (*func)(TSPSolution *, const double);
};

const char* defaultTestBed[] = {
  "./data/pr1002.tsp",
  "./data/a280.tsp",
  "./data/att48.tsp",
  "./data/att532.tsp",
  "./data/bier127.tsp",
  "./data/eil101.tsp",
  "./data/kroA150.tsp",
  "./data/kroA200.tsp",
  "./data/pr299.tsp",
  "./data/pr439.tsp",
};

void plotFunc(const char *name, void (*func)(TSPSolution *, const time_t),
  const double seconds = 60, const char * testBed[] = defaultTestBed, const int numTestBeds = 10){
      printf("Plotting %s...\n", name);

  system("mkdir -p ./plot/perf");
  std::string filename = "./plot/perf/" + std::string(name) + ".txt";

  FILE *fptr;
  fptr = fopen(filename.c_str(),"w");
  if(fptr == NULL){
    printf("Error opening!");
    exit(1);
  }


  for (int i = 0; i < numTestBeds; i++) {
    TSP tsp(testBed[i]);
    TSPSolution sol(&tsp);
    time_t timeLimit = time(NULL) + seconds;
    func(&sol, timeLimit);
    fprintf(fptr, "%d\n", sol.cost);
  }
  fclose(fptr);

}

void plotFunc(const char *name, int (*func)(TSP *, const time_t),
  const double seconds = 60, const char * testBed[] = defaultTestBed, const int numTestBeds = 10){
  printf("Plotting %s...\n", name);
  system("mkdir -p ./plot/perf");
  std::string filename = "./plot/perf/" + std::string(name) + ".txt";

  FILE *fptr;
  fptr = fopen(filename.c_str(),"w");
  if(fptr == NULL){
    printf("Error opening!");
    exit(1);
  }


  for (int i = 0; i < numTestBeds; i++) {
    TSP tsp(testBed[i]);
    time_t timeLimit = time(NULL) + seconds;
    fprintf(fptr, "%d\n", func(&tsp, timeLimit));
  }
  fclose(fptr);

}





#ifndef IPYCPP
/*
#include "tsp.cpp"
#include "executer.cpp"
#include "heuristic.cpp"
#include "metaheuristic.cpp"

int nearestGRASP(TSP *tsp, const time_t timeLimit){
  auto r = solveMultiThreadEndless(nearestWithGrasp, *tsp, timeLimit, {0, tsp->dim, 1});
  auto cost = r.solution->cost;
  free(r.solution);
  return cost;
}

int nearestGRASPPlus2Opt(TSP *tsp, const time_t timeLimit){
  auto r = solveMultiThreadEndless(nearestWithGraspPlusTwoOpt, *tsp, timeLimit, {0, tsp->dim, 1});
  auto cost = r.solution->cost;
  free(r.solution);
  return cost;
}


int main() {

  plotFunc("nearestGrasp1min", nearestGRASP , 60, defaultTestBed, 10);
  plotFunc("nearestGrasp5min", nearestGRASP , 60 * 5, defaultTestBed, 10);

  plotFunc("nearestGraspPlus2OPT1min", nearestGRASPPlus2Opt , 60, defaultTestBed, 10);
  plotFunc("nearestGraspPlus2OPT5min", nearestGRASPPlus2Opt , 60 * 5, defaultTestBed, 10);


}*/

#endif


#endif