#ifndef EXECUTER_H
#define EXECUTER_H

#include "tsp.cpp"

struct HyperParam {
  int start;
  int end;
  int step;
};

struct Result {
  int time;
  TSPSolution *solution;
  int param1, param2, param3, param4;
};

Result solveSingleThread(void (*solvFunc)(TSPSolution*, const time_t, const int), TSP &tsp, const time_t timeLimit, const HyperParam &param){

  time_t start = time(NULL);

  // try every possible starting point
  TSPSolution *minSol = new TSPSolution(&tsp);
  solvFunc(minSol, timeLimit, param.start);
  int minI = 0;

  TSPSolution *challenger = new TSPSolution(&tsp);

  for(int i = param.start + 1; i < param.end; i+= param.step){
    solvFunc(challenger, timeLimit, i);
    if(challenger->cost < minSol->cost){
      auto tmp = minSol;
      minSol = challenger;
      challenger = tmp;
      minI = i;
    }
    if(time(NULL) > timeLimit) goto endLoop;
  }

  endLoop:

  time_t end = time(NULL);
  int time = end - start;

  delete challenger;
  return {time, minSol, minI, -1 , -1, -1};
}

Result solveSingleThread(void (*solvFunc)(TSPSolution*, const time_t, const int, const int), TSP &tsp, const time_t timeLimit, const HyperParam &param, const HyperParam &param2){
  time_t start = time(NULL);

  // try every possible starting point
  TSPSolution *minSol = new TSPSolution(&tsp);
  solvFunc(minSol, timeLimit, param.start, param2.start);
  int minI = 0, minJ = 0;
  TSPSolution *challenger = new TSPSolution(&tsp);

  for(int i = param.start; i < param.end; i+= param.step){
    for(int j = param2.start; j < param2.end; j+= param2.step){
      solvFunc(challenger, timeLimit, i, j);
      if(challenger->cost < minSol->cost){
        auto tmp = minSol;
        minSol = challenger;
        challenger = tmp;
        minI = i;
        minJ = j;
      }
      if(time(NULL) > timeLimit) goto endLoop;
    }
  }

  endLoop:

  time_t end = time(NULL);
  int time = end- start;;

  delete challenger;
  return {time, minSol, minI, minJ , -1, -1};
}

Result solveSingleThread(void (*solvFunc)(TSPSolution*, const time_t, const int, const int, const int), TSP &tsp, const time_t timeLimit,
const HyperParam &param, const HyperParam &param2, const HyperParam &param3){
  time_t start = time(NULL);

  // try every possible starting point
  TSPSolution *minSol = new TSPSolution(&tsp);
  solvFunc(minSol, timeLimit, param.start, param2.start, param3.start);
  int minI = 0, minJ = 0, minK = 0;
  TSPSolution *challenger = new TSPSolution(&tsp);

  for(int i = param.start; i < param.end; i+= param.step){
    for(int j = param2.start; j < param2.end; j+= param2.step){
      for(int k = param3.start; k < param3.end; k+= param3.step){
        solvFunc(challenger, timeLimit, i, j, k);
        if(challenger->cost < minSol->cost){
          auto tmp = minSol;
          minSol = challenger;
          challenger = tmp;
          minI = i;
          minJ = i;
          minK = i;
        }
        if(time(NULL) > timeLimit) goto endLoop;
      }
    }
  }

  endLoop:

  time_t end = time(NULL);
  int time = end- start;;

  delete challenger;
  return {time, minSol, minI, minJ , minK, -1};
}



Result solveMultiThread(void (*solvFunc)(TSPSolution*, const time_t, const int), TSP &tsp, const time_t timeLimit, const HyperParam &param){
  // start clock
  time_t start = time(NULL);

  // try every possible starting point
  TSPSolution *minSol = new TSPSolution(&tsp);
  solvFunc(minSol, timeLimit, param.start);
  int minIndex = 0;

  #pragma omp parallel num_threads(8)
  {
    TSPSolution *challenger = new TSPSolution(&tsp);
    #pragma omp for
    for(int i = param.start; i < param.end; i++){
      challenger->fill();
      solvFunc(challenger, timeLimit, i);
      #pragma omp critical
      if(challenger->cost < minSol->cost){
        auto tmp = minSol;
        minSol = challenger;
        challenger = tmp;
        minIndex = i;
      }
      if(time(NULL) > timeLimit) continue;
    }
    delete challenger;
  }


  // stop timer
  time_t end = time(NULL);
  int time = end- start;;

  Result result{time, minSol, minIndex, -1 , -1, -1};
  return result;
}


Result solveMultiThreadEndless(void (*solvFunc)(TSPSolution*, const time_t, const int), TSP &tsp, const time_t timeLimit, const HyperParam &param){
  // start clock
  time_t start = time(NULL);

  // try every possible starting point
  TSPSolution *minSol = new TSPSolution(&tsp);
  solvFunc(minSol, timeLimit, param.start);
  int minIndex = 0;

  printf("start %d,", tsp.dim); printTime(start);

  #pragma omp parallel num_threads(8)
  {
    TSPSolution *challenger = new TSPSolution(&tsp);
    while(time(NULL) < timeLimit){
      #pragma omp for
      for(int i = param.start; i < param.end; i++){
        challenger->fill();
        solvFunc(challenger, timeLimit, i);
        #pragma omp critical
        if(challenger->cost < minSol->cost){
          printf("improving from %d to %d\n", minSol->cost,challenger->cost);
          auto tmp = minSol;
          minSol = challenger;
          challenger = tmp;
          minIndex = i;
        }
        if(time(NULL) > timeLimit) continue;
      }
    }
    delete challenger;
  }

  printf("end %d,", tsp.dim); printTime(start);


  // stop timer
  time_t end = time(NULL);
  int time = end- start;;

  Result result{time, minSol, minIndex, -1 , -1, -1};
  return result;
}



void printResult(const Result &result, const bool plot = true){
  printf("Seconds: %d\n", result.time);
  printf("Param1 = %d, ", result.param1);
  if(result.param2 != -1) ("Param2 = %d, ", result.param2);
  if(result.param3 != -1) printf("Param3 = %d, ", result.param3);
  if(result.param4 != -1) printf("Param4 = %d, ", result.param4);

  printf(" cost = %d\n", result.solution->cost);

  if(plot) result.solution->plot();
}

#endif