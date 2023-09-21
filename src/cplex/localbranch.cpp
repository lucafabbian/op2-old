#ifndef TSPCMATHLOCAL_H
#define TSPCMATHLOCAL_H

#include "../tsp.cpp"
#include <ilcplex/cplex.h>
#include "cutils.cpp"
#include "callbacks.cpp"
#include <memory>


void localBranch(TSPSolution &firstSol, time_t timeLimit, const char name[]) {

  const double MIN_IMPROVEMENT = 0.015;
  double radius = 3; //Array of radious
  int consecutivesSmallImprovements = 0; //Number of small improvements
  const int SMALL_IMPROVEMENTS_TOLERANCE = 3; //Limit of consecutives small improvements tolerated

  TSP &tsp = *firstSol.tsp;
  int status;
  CPXENVptr env = CPXopenCPLEX(&status);       // generate new environment, in err will be saved errors
  CPXLPptr lp = CPXcreateprob(env, &status, name);   // create new empty linear programming problem (no variables, no constraints ...)

  TSPSolution sol {&tsp};
  for(int i = 0; i < tsp.dim; i++) sol.sequence[i] = firstSol.sequence[i];
  sol.cost = firstSol.cost;

  buildModel(*firstSol.tsp, env, lp);
  setCPLEXParams(env, timeLimit - time(NULL));

  // enable user cut
  CPXLONG contextid = CPX_CALLBACKCONTEXT_CANDIDATE | CPX_CALLBACKCONTEXT_RELAXATION;

  status = CPXcallbacksetfunc(env, lp, contextid, SEC_cuts_callback, &tsp);
  if (status) debug("CPXcallbacksetfunc() error returned status %d\n", status);

  const int ncols = tsp.dim * (tsp.dim -1) / 2;

  std::unique_ptr<double[]> xstar(new double[ncols]);
  std::fill_n(xstar.get(), ncols, 0.0);

  for(int i = 0; i < tsp.dim; i++) {
    const int node1 = sol.sequence[i];
    const int node2 = sol.sequence[(i + 1) % tsp.dim];
    xstar[flatPos(node1, node2, tsp.dim)] = 1.0;
  }

  std::unique_ptr<int[]> allIndexes(new int[ncols]);
  for(int i = 0; i < ncols; i++) allIndexes[i] = i;

  std::unique_ptr<double[]> ones(new double[ncols]);
  std::fill_n(ones.get(), ncols, 1.0);

  char greaterOrEqual = 'G';
  int matbeg = 0;
  char names[100];
  char *pNames = &names[0];

  int beg = 0;
  int level = CPX_MIPSTART_NOCHECK;
  CPXaddmipstarts(env, lp, 1,ncols, &beg, allIndexes.get(), xstar.get(), &level, NULL);

  //CPXwriteprob(env, lp, "models/l1.lp", NULL);

  int newcolN = 0;
  std::unique_ptr<int[]> newcolIndexes(new int[ncols]);
  int iter = 0;
  do{
    CPXsetdblparam(env, CPXPARAM_TimeLimit, timeLimit - time(NULL));
    debug("MATHLOCAL: new cycle\n");

    debug("Setting xstars\n");

    for(int col = 0; col < ncols; col++){
      if(xstar[col] > (1.0 - EPS) ){
        newcolIndexes[newcolN++] = col;
      }
    }
    debug("Done setting xstars\n");

    double rhs = tsp.dim- radius;
    debug("Setting name\n");

    sprintf(names, "newconst[%d]", iter++);
    debug("Adding name\n");

    status = CPXaddrows(env, lp, 0, 1, newcolN, &rhs, &greaterOrEqual, &matbeg, newcolIndexes.get(), ones.get(), NULL, &pNames);
    if (status) {
        debug("CPXaddrows localbranch error %d\n", status);
    }
      //CPXwriteprob(env, lp, "models/l2.lp", NULL);

    debug("Optimizing xstars\n");
    status = CPXmipopt(env, lp);
    if (status) {
            debug("CPXstatus localbranch error %d\n", status);
    }

    status = CPXgetx(env, lp, xstar.get(), 0, ncols - 1);
    if (status) { debug("CPXgetx error %d\n", status); }
    debug("Extracting solution\n");
    cplexExtractSolFromXstars(sol, tsp.dim, xstar.get());

    double costImprovement = 1 - sol.cost / firstSol.cost;
    if (costImprovement < MIN_IMPROVEMENT) {
      debug("MATHLOCAL: cost improvement %f < %f\n", costImprovement, MIN_IMPROVEMENT);
      consecutivesSmallImprovements++;
      if(consecutivesSmallImprovements >= SMALL_IMPROVEMENTS_TOLERANCE) {
        debug("improving radius from %f to %f\n", radius, radius + 2);
        radius+=2;

        consecutivesSmallImprovements = 0;
      }
    }else{
      consecutivesSmallImprovements = 0;
    }

    debug("Copying solution\n");
    if(sol.cost < firstSol.cost){
      firstSol.cost = sol.cost;
      memcpy(firstSol.sequence, sol.sequence, tsp.dim * sizeof(int));
      printf("TIMECOST:%ld %d\n", time(NULL), firstSol.cost);
    }

    debug("Removing rows\n");
    int numrows = CPXgetnumrows(env, lp);
    status = CPXdelrows(env, lp, numrows - 1, numrows - 1);
    //CPXwriteprob(env, lp, "models/l3.lp", NULL);
    if (status) { debug("CPXdelrows error code %d\n", status); }

    newcolN = 0;
  }while(time(NULL) < timeLimit);

}



#endif //TSPCMATHLOCAL_H