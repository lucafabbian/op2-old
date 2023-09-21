#ifndef TSPCMATHHARD_H
#define TSPCMATHHARD_H

#include "../tsp.cpp"
#include <ilcplex/cplex.h>
#include "cutils.cpp"
#include "callbacks.cpp"
#include <memory>



void hardfix(TSPSolution &firstSol, time_t timeLimit, const char name[]) {

  const int pFix = 700; // %1000, i.e. 30%

  TSP &tsp = *firstSol.tsp;
  int status;
  CPXENVptr env = CPXopenCPLEX(&status);       // generate new environment, in err will be saved errors
  CPXLPptr lp = CPXcreateprob(env, &status, name);   // create new empty linear programming problem (no variables, no constraints ...)


  buildModel(*firstSol.tsp, env, lp);
  setCPLEXParams(env, timeLimit - time(NULL));

  // enable user cut
  CPXLONG contextid = CPX_CALLBACKCONTEXT_CANDIDATE | CPX_CALLBACKCONTEXT_RELAXATION;

  status = CPXcallbacksetfunc(env, lp, contextid, SEC_cuts_callback, &tsp);
  if (status) debug("CPXcallbacksetfunc() error returned status %d\n", status);


  const int ncols = tsp.dim * (tsp.dim -1) / 2;
  std::unique_ptr<double[]> ones(new double[ncols]);
  std::fill_n(ones.get(), ncols, 1.0);

  std::unique_ptr<double[]> zeros(new double[ncols]);
  std::fill_n(zeros.get(), ncols, 0.0);

  std::unique_ptr<char[]> lower(new char[ncols]);
  std::fill_n(lower.get(), ncols, 'L');


  int fixedEdgesN = 0;
  std::unique_ptr<int[]> fixedEdgesIndexes(new int[ncols]);

  TSPSolution sol {&tsp};
  for(int i = 0; i < tsp.dim; i++) sol.sequence[i] = firstSol.sequence[i];
  sol.cost = firstSol.cost;
  do{
    CPXsetdblparam(env, CPXPARAM_TimeLimit, timeLimit - time(NULL));
    debug("MATHHARD: new cycle\n");

    //CPXwriteprob(env, lp, "models/beforefix.lp", NULL);

    for(int i = 0; i < tsp.dim; i++){
      const int node1 = sol.sequence[i];
      const int node2 = sol.sequence[(i + 1) % tsp.dim];
      bool shouldFix = rand() % 1000 < pFix;
      //debug("MATHHARD: shouldFix %d %d %d\n", node1 , node2, flatPos(node1, node2, tsp.dim));
      if(shouldFix) fixedEdgesIndexes[fixedEdgesN++] = flatPos(node1, node2, tsp.dim);
    }
    debug("MATHHARD: fixed %d edges\n", fixedEdgesN);


    CPXchgbds(env, lp, fixedEdgesN,  fixedEdgesIndexes.get(), lower.get(), ones.get());


    //CPXwriteprob(env, lp, "models/afterfix.lp", NULL);
    status = CPXmipopt(env, lp);
    int mipstat =CPXgetstat (env, lp);

    if((status && status != CPX_STAT_ABORT_TIME_LIM) || (mipstat != CPXMIP_TIME_LIM_FEAS && mipstat!= CPXMIP_OPTIMAL)){
      debug("MATHHARD: Cplex solver encountered an error with error code: %d\n", status);
      exit(1);
    }

    if(mipstat == CPXMIP_TIME_LIM_FEAS) debug("MATHHARD: not enough time!\n");

    cplexExtractSolution(sol, env, lp);
    if(!status && mipstat == CPXMIP_OPTIMAL){
      debug("MATHHARD: found exact solution\n");

        debug("Costs: %d, new%d\n", firstSol.cost, sol.cost);

      if(sol.cost < firstSol.cost) {
        firstSol.cost = sol.cost;
        memcpy(firstSol.sequence, sol.sequence, tsp.dim * sizeof(int));
        printf("TIMECOST:%ld %d\n", time(NULL), firstSol.cost);
      }

    }




    CPXchgbds(env, lp, fixedEdgesN,  fixedEdgesIndexes.get(), lower.get(), zeros.get());
    fixedEdgesN = 0;

  }while(time(NULL) < timeLimit);

}


#endif //TSPCMATH_H