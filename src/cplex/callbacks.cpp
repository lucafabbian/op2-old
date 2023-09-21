#ifndef TSPCALLBACK_H
#define TSPCALLBACK_H


#include <ilcplex/cplex.h>
#include <memory>
extern "C" {
  #include "../../concorde/concorde.h"
}
#include "cutils.cpp"
#include "../heuristic.cpp"


static int add_SEC_cuts(const int &dim, CPXCALLBACKCONTEXTptr context, int current_tour, int *comp, int *indexes, double *values) {
    double rhs;
    char sense;
    int matbeg = 0; // Contains the index of the beginning column. In this case we add 1 row at a time so no need for an array
    int nnz = prepare_SEC(dim, current_tour, comp, &sense, indexes, values, &rhs);
    return CPXcallbackrejectcandidate(context, 1, nnz, &rhs, &sense, &matbeg, indexes, values);
}

static int add_SEC_cuts_fractional(const int &dim, CPXCALLBACKCONTEXTptr context, int current_tour, int *comp, int *indexes, double *values) {
    double rhs;
    char sense;
    int matbeg = 0; // Contains the index of the beginning column. In this case we add 1 row at a time so no need for an array
    int purgeable = CPX_USECUT_FILTER;
	int local = 0;
    int nnz = prepare_SEC(dim, current_tour, comp, &sense, indexes, values, &rhs);
    return CPXcallbackaddusercuts(context, 1, nnz, &rhs, &sense, &matbeg, indexes, values, &purgeable, &local); // add one violated usercut
}

static int CPXPUBLIC SEC_cuts_callback_candidate(CPXCALLBACKCONTEXTptr context, CPXLONG contextid, TSP &tsp ) {
    int ncols = (tsp.dim *( tsp.dim -1))/2;
    std::unique_ptr<double[]> xstar(new double[ncols]);

    double objval = CPX_INFBOUND;
    int currentnode = -1; CPXcallbackgetinfoint(context, CPXCALLBACKINFO_NODECOUNT, &currentnode);

    int status = CPXcallbackgetcandidatepoint(context, xstar.get(), 0, ncols - 1 , &objval);
    if (status) debug("CPXcallbackgetcandidatepoint() error code %d", status);

    std::unique_ptr<int[]> succ(new int[tsp.dim]);
    std::fill_n(succ.get(), tsp.dim, -1);

    std::unique_ptr<int[]> comp(new int[tsp.dim]);
    std::fill_n(comp.get(), tsp.dim, -1);
    int num_comp = countComponents(tsp.dim, xstar.get(), succ.get(), comp.get());

    debug("Num components candidate: %d\n", num_comp);


    if (num_comp > 1) { // More than one tours found. Violated so add the cuts
        debug("Added SEC cut in node %d\n", currentnode);

        std::unique_ptr<int[]> indexes(new int[ncols]);
        std::unique_ptr<double[]> values(new double[ncols]);
        for (int subtour = 1; subtour <= num_comp; subtour++) {
            // For each subtour we add the constraints in one shot
            status = add_SEC_cuts(tsp.dim, context, subtour, comp.get(), indexes.get(), values.get());
            if (status) debug("Error with add_SEC_cuts. Error code %d", status);
        }

    } else if (num_comp == 1){ // enable 2 opt

      TSPSolution sol {&tsp};

      cplexExtractSolFromXstars(sol, tsp.dim, xstar.get());
      twoOpt(&sol);
      std::fill_n(xstar.get(), tsp.dim, 0.0);
      for(int i = 0; i < tsp.dim; i++) {
        const int node1 = sol.sequence[i];
        const int node2 = sol.sequence[(i + 1) % tsp.dim];
        xstar[flatPos(node1, node2, tsp.dim)] = 1.0;
      }

      std::unique_ptr<int[]> allIndexes(new int[ncols]);
      for(int i = 0; i < ncols; i++) allIndexes[i] = i;

      // there is something weird with this function. Why should I pass indexes if they are clearly a list from 0 to n-1?
      if(status = CPXcallbackpostheursoln(context, ncols, allIndexes.get(), xstar.get(), sol.cost, CPXCALLBACKSOLUTION_NOCHECK))
        debug("An error occured on CPXcallbackpostheursoln");

    }

    return 0;
}

typedef struct {
    CPXCALLBACKCONTEXTptr context;
    int dim;
} relaxation_callback_params;

static int violated_cuts_callback(double cutval, int num_nodes, int* members, void* param) {
    relaxation_callback_params *params = (relaxation_callback_params*) param;
    CPXCALLBACKCONTEXTptr context = params->context;
    debug("A cut with %d nodes has cut value of %f\n", num_nodes, cutval);



    double rhs = num_nodes - 1;
    char sense = 'L';
    int matbeg = 0;
    int num_edges = num_nodes * (num_nodes - 1) / 2;
    //LOG_D("Num edges: %d\n", num_edges);
    debug("mem function violated_cuts_callback%d\n", num_edges);

    std::unique_ptr<double[]> values(new double[num_edges]);


    std::unique_ptr<int[]> edges(new int[num_edges]);

    debug("function violated_cuts_callback\n");
    int k = 0;
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            if (members[i] >= members[j]) { continue; } // undirected graph. If the node in index i is greated than the node in index j, we skip since (i,j) = (j,i)
            edges[k] = flatPos(members[i], members[j], params->dim);
            values[k] = 1.0;
            k++;
            //LOG_D("X(%d,%d)", members[i], members[j]);
        }
    }
    debug("END function violated_cuts_callback\n");

    int purgeable = CPX_USECUT_FILTER;
	  int local = 0;
    int status = CPXcallbackaddusercuts(context, 1, num_edges, &rhs, &sense, &matbeg, edges.get(), values.get(), &purgeable, &local);
    if (status) debug("CPXcallbackaddusercuts() when conn comps = 1. Error code %d", status);
    return 0;
}

static int CPXPUBLIC SEC_cuts_callback_relaxation(CPXCALLBACKCONTEXTptr context, CPXLONG contextid, const TSP &tsp) {
    int ncols = (tsp.dim * (tsp.dim - 1)) / 2;
    int depth = 0;
    int node = -1;
    int threadid = -1;
    double objval = CPX_INFBOUND;
    CPXcallbackgetinfoint(context, CPXCALLBACKINFO_NODEDEPTH, &depth);
    CPXcallbackgetinfoint(context, CPXCALLBACKINFO_NODECOUNT, &node);
    CPXcallbackgetinfoint(context, CPXCALLBACKINFO_THREADID, &threadid);

    //LOG_D("Depth is %d", depth);
    //LOG_D("Current node %d", node);
    //LOG_D("Thread id: %d\n", threadid);

    //if (depth > 5) return 0; // Hyperparameter tuning
    debug("Relaxation cut\n");

    std::unique_ptr<double[]> xstar(new double[ncols]);

    int status = CPXcallbackgetrelaxationpoint(context, xstar.get(), 0, ncols - 1 , &objval);
    if (status) debug("CPXcallbackgetrelaxationpoint() error code %d", status);


    int numcomps = 0;
    std::unique_ptr<int[]>  elist(new int[2*ncols]);
    int *compscount = NULL;
    int *comps = NULL;
    int k = 0;

    int num_edges = 0;
    for (int i = 0; i < tsp.dim; i++) {
        for (int j = i+1; j < tsp.dim; j++) {
            //if (fabs(xstar[x_udir_pos(i, j, inst->num_nodes)]) <= EPS) continue;
            elist[k++] = i;
            elist[k++] = j;
            num_edges++;
        }
    }
    // Checking whether or not the graph is connected with the fractional solution.
    // CONCORDE
    status = CCcut_connect_components(tsp.dim, num_edges, elist.get(), xstar.get(), &numcomps, &compscount, &comps);
    if (status) {
        debug("CCcut_connect_components() error code %d\n", status);
    }

    if (numcomps == 1) {
        debug("Single component\n");

        relaxation_callback_params params { context, tsp.dim };
        // At this point we have a connected graph. This graph could not be a "tsp". We interpret the fractional
        // solution as capacity of a cut. A cut of a graph G is composed by S and T = V - S where V is the nodes set.
        // The capacity of the cut is the sum of all ingoing and outgoing edges of the cut. Since we have a TSP,
        // we want that for each cut, we have a capacity of 2 (i.e. one ingoing edge and one outgoing edge).
        // So we want to seek the cuts which don't have a capacity of 2. The cuts with capacity < 2 violates the
        // constraints and we are going to add SEC to them.
        // NB: We use cutoff as 2.0 - EPS for numerical stability due the fractional values we obtain in the solution.
        status = CCcut_violated_cuts(tsp.dim, ncols, elist.get(), xstar.get(), 2.0 - EPS, violated_cuts_callback, &params);
        if (status) {
            debug("CCcut_violated_cuts() error code %d", status);
        }

        debug("END single component violation\n");
    } else if (numcomps > 1) {
        debug("Num components fractional: %d\n", numcomps);

        int startindex = 0;

        std::unique_ptr<int[]> components(new int[tsp.dim]);
        // Transforming the concorde's component format into our component format in order to use our addSEC function
        for (int subtour = 0; subtour < numcomps; subtour++) {

            for (int i = startindex; i < startindex + compscount[subtour]; i++) {
                int index = comps[i];
                components[index] = subtour + 1;
            }

            startindex += compscount[subtour];

        }

        std::unique_ptr<int[]> indexes(new int[ncols]);
        std::unique_ptr<double[]> values(new double[ncols]);
        for (int subtour = 1; subtour <= numcomps; subtour++) {
            // For each subtour we add the constraints in one shot
            status = add_SEC_cuts_fractional(tsp.dim, context, subtour, components.get(), indexes.get(), values.get());
            if (status) {
                debug("Error with add_SEC_cuts. Error code %d\n", status);
            }
            debug("Added SEC cuts to tour %d\n", subtour);

        }

    }
    debug("should print\n");
    return 0;
}

int CPXPUBLIC SEC_cuts_callback(CPXCALLBACKCONTEXTptr context, CPXLONG contextid, void *userhandle ) {
    TSP &tsp =  *((TSP*) userhandle);
    if (contextid == CPX_CALLBACKCONTEXT_CANDIDATE) {
        return SEC_cuts_callback_candidate(context, contextid, tsp);
    }
    if (contextid == CPX_CALLBACKCONTEXT_RELAXATION) {
        return SEC_cuts_callback_relaxation(context, contextid, tsp);
    }
    return 1;
}


int solveWithCallbacks(){
  bool enableUserCuts = true;
  auto name = "test1";
  TSP tsp("./data/pr439.tsp");
  TSPSolution sol(&tsp);


  int status;
  CPXENVptr env = CPXopenCPLEX(&status);       // generate new environment, in err will be saved errors
  CPXLPptr lp = CPXcreateprob(env, &status, name);   // create new empty linear programming problem (no variables, no constraints ...)

  buildModel(tsp, env, lp);
  setCPLEXParams(env, 600);

  const time_t timeLimit = time(NULL) + 100;


  CPXLONG contextid = CPX_CALLBACKCONTEXT_CANDIDATE;
  if (enableUserCuts) contextid |= CPX_CALLBACKCONTEXT_RELAXATION;

  status = CPXcallbacksetfunc(env, lp, contextid, SEC_cuts_callback, &tsp);
  if (status) debug("CPXcallbacksetfunc() error returned status %d\n", status);

  status = CPXmipopt(env, lp);
  int mipstat =CPXgetstat (env, lp);
  if (status || mipstat != CPXMIP_OPTIMAL) {
    if (status == CPX_STAT_ABORT_TIME_LIM || mipstat == CPXMIP_TIME_LIM_FEAS) {
      debug("Time limit exceeded\n");
    } else {
      debug("Cplex solver encountered an error with error code: %d\n", status);
    }
    exit(1);
  }


  int lpstat = CPXgetstat (env, lp);


  debug("Cplex solver encountered an error with error code: %d, lpstat%d\n", status, lpstat);


  cplexExtractSolution(sol, env, lp);

  sol.plot();
  debug("Solution: %d\n", sol.cost);

  CPXfreeprob(env, &lp);
  CPXcloseCPLEX(&env);

  return 0;
}

#endif