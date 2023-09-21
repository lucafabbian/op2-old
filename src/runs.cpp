#ifndef TSPRUNS_H
#define TSPRUNS_H

#include "utils.cpp"
#include "tsp.cpp"
#include "metaheuristic.cpp"
#include "cplex/hardfixing.cpp"
#include "cplex/localbranch.cpp"





#ifdef RUN_VNS
int main(int argc, char **argv){
  printf("Running vns %s, time=%ld\n", argv[1], time(NULL));

  time_t timeLimit = time(NULL) + DEFAULT_TIMELIMIT;
  TSP tsp(argv[1]);
  TSPSolution sol(&tsp);
  nearest(&sol, timeLimit); // 10 minutes
  twoOpt(&sol);
  vns(&sol, timeLimit);
  printf("Solution cost: %d\n", sol.cost);
  sol.plot();
}

#endif

#ifdef RUN_TABULINEAR
int main(int argc, char **argv){
  printf("Running tabulinear %s, time=%ld\n", argv[1], time(NULL));

  time_t timeLimit = time(NULL) + DEFAULT_TIMELIMIT;
  TSP tsp(argv[1]);
  TSPSolution sol(&tsp);
  nearest(&sol, timeLimit); // 10 minutes
  twoOpt(&sol);
  tabu(&sol, timeLimit, linearTabu);
  printf("Solution cost: %d\n", sol.cost);
  sol.plot();
}
#endif

#ifdef RUN_TABUSTEP
int main(int argc, char **argv){
  printf("Running tabustep %s, time=%ld\n", argv[1], time(NULL));

  time_t timeLimit = time(NULL) + DEFAULT_TIMELIMIT;
  TSP tsp(argv[1]);
  TSPSolution sol(&tsp);
  nearest(&sol, timeLimit); // 10 minutes
  twoOpt(&sol);
  tabu(&sol, timeLimit, stepTabu);
  printf("Solution cost: %d\n", sol.cost);
  sol.plot();
}
#endif

#ifdef RUN_TABURANDOM
int main(int argc, char **argv){
  printf("Running taburandom %s, time=%ld\n", argv[1], time(NULL));

  time_t timeLimit = time(NULL) + DEFAULT_TIMELIMIT;
  TSP tsp(argv[1]);
  TSPSolution sol(&tsp);
  nearest(&sol, timeLimit); // 10 minutes
  twoOpt(&sol);
  tabu(&sol, timeLimit, randomTabu);
  printf("Solution cost: %d\n", sol.cost);
  sol.plot();
}
#endif

#ifdef RUN_ANNEALING
int main(int argc, char **argv){
  printf("Running annealing %s, time=%ld\n", argv[1], time(NULL));

  time_t timeLimit = time(NULL) + DEFAULT_TIMELIMIT;
  TSP tsp(argv[1]);
  TSPSolution sol(&tsp);
  nearest(&sol, timeLimit); // 10 minutes
  twoOpt(&sol);
  annealing(&sol, timeLimit);
  printf("Solution cost: %d\n", sol.cost);
  sol.plot();
}
#endif

#ifdef RUN_GENETIC
int main(int argc, char **argv){
  printf("Running genetic %s, time=%ld\n", argv[1], time(NULL));

  time_t timeLimit = time(NULL) + DEFAULT_TIMELIMIT;
  TSP tsp(argv[1]);
  TSPSolution sol(&tsp);
  nearest(&sol, timeLimit); // 10 minutes
  twoOpt(&sol);
  genetic(&sol, timeLimit);
  printf("Solution cost: %d\n", sol.cost);
  sol.plot();
}
#endif


#ifdef RUN_LOCALBRANCH

int main(int argc, char **argv){
  printf("Running localBranching %s, time=%ld\n", argv[1], time(NULL));
  time_t timeLimit = time(NULL) + DEFAULT_TIMELIMIT;
  TSP tsp(argv[1]);
  TSPSolution sol(&tsp);
  nearest(&sol, timeLimit); // 10 minutes
  twoOpt(&sol);
  localBranch(sol, timeLimit, argv[1]);
  printf("Solution cost: %d\n", sol.cost);
  sol.plot();
}
#endif


#ifdef RUN_HARDFIXING

int main(int argc, char **argv){
  printf("Running hardfixing %s, time=%ld\n", argv[1], time(NULL));

  time_t timeLimit = time(NULL) + DEFAULT_TIMELIMIT;
  TSP tsp(argv[1]);
  TSPSolution sol(&tsp);
  nearest(&sol, timeLimit); // 10 minutes
  twoOpt(&sol);
  hardfix(sol, timeLimit, argv[1]);
  printf("Solution cost: %d\n", sol.cost);
  sol.plot();
}

#endif

#endif