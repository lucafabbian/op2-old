#ifndef TSPBENDERS_H
#define TSPBENDERS_H

#include "../tsp.cpp"
#include "cutils.cpp"

#include <ilcplex/cplex.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>



static int add_SEC(const TSP &tsp, CPXENVptr env, CPXLPptr lp, int current_tour, int *comp, int *indexes, double *values, char *names) {
    double rhs;
    char sense;
    int matbeg = 0; // Contains the index of the beginning column. In this case we add 1 row at a time so no need for an array
    int nnz = prepare_SEC(tsp.dim, current_tour, comp, &sense, indexes, values, &rhs);
    debug("Tour %d has %d nodes\n", current_tour, (int) (rhs + 1)); // Since rhs is |S| - 1 we add +1 to get |S|.

    return CPXaddrows(env, lp, 0, 1, nnz, &rhs, &sense, &matbeg, indexes, values, NULL, &names);
}

int benders_loop(const TSP &tsp, time_t timeLimit, CPXENVptr env, CPXLPptr lp) {

    int* successors = (int*) malloc(tsp.dim * sizeof(int));
    int* comp = (int*) malloc(tsp.dim * sizeof(int));

    char names[100];
    int numcomp = 1;
    int rowscount = 0;

    int status = 0;

    do {
      time_t remaining = timeLimit - time(NULL);
      if(remaining < 0) {
        status = CPX_STAT_ABORT_TIME_LIM;
        goto end;
      }
      CPXsetdblparam(env, CPXPARAM_TimeLimit, remaining);

      if(status = CPXmipopt(env, lp)) goto end;


      //Initialization of successors and comp arrays with -1
      memset(successors, -1, tsp.dim * sizeof(int));
      memset(comp, -1, tsp.dim * sizeof(int));

      const int ncols = CPXgetnumcols(env, lp);

      int *indexes = (int*)      malloc(ncols * sizeof(int));
      double *values = (double*) malloc(ncols * sizeof(double));
      double *xstar = (double*)  malloc(ncols * sizeof(double));

      //Count connected components (subtours)
      status = CPXgetx(env, lp, xstar, 0, ncols-1);
      if (status = CPXgetx(env, lp, xstar, 0, ncols-1)) goto end;
      numcomp = countComponents(tsp.dim, xstar, successors, comp);
      debug("NUM COMPONENTS: %d\n", numcomp);


        // Condition numComp > 1 is needed in order to avoid to add the SEC constraints when the TSP's hamiltonian cycle is found
        for (int subtour = 1; subtour <= numcomp && numcomp > 1; subtour++) { // Connected components are numerated from 1
            sprintf(names, "SEC(%d)", ++rowscount);
            // For each subtour we add the SEC constraints in one shot
            if (status = add_SEC(tsp, env, lp, subtour, comp, indexes, values, names)) { ("An error occurred adding SEC. Error code %d", status); }
            //save_lp(env, lp, inst->name);
        }


        free(indexes);
        free(values);
        free(xstar);

    } while (numcomp > 1);

    status = 0;

  end:
    free(successors);
    free(comp);

    return status;
}


/*
int main(){
  auto name = "test1";
  TSP tsp("./data/pr136.tsp");
  TSPSolution sol(&tsp);


  int status;
  CPXENVptr env = CPXopenCPLEX(&status);       // generate new environment, in err will be saved errors
  CPXLPptr lp = CPXcreateprob(env, &status, name);   // create new empty linear programming problem (no variables, no constraints ...)

  buildModel(tsp, env, lp);
  setCPLEXParams(env, 100);

  const time_t timeLimit = time(NULL) + 100;

  if (status = benders_loop(tsp, timeLimit, env, lp)) {
    if (status == CPX_STAT_ABORT_TIME_LIM) {
      debug("Time limit exceeded\n");
    } else {
      debug("Cplex solver encountered an error with error code: %d\n", status);
    }
    exit(1);
  }


  cplexExtractSolution(sol, env, lp);

  sol.plot();
  debug("Solution: %d\n", sol.cost);

  CPXfreeprob(env, &lp);
  CPXcloseCPLEX(&env);

}
*/
#endif