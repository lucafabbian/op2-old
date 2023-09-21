#ifndef TSPCUTILS_H
#define TSPCUTILS_H

#include "../tsp.cpp"
#include <ilcplex/cplex.h>


#define EPS 1e-5


static void buildModel(const TSP &tsp, CPXENVptr env, CPXLPptr lp) {
    char xctype = 'B';  // B=binary variable
    char name[200];
    char *namePtr = name;

    for (int i = 0; i < tsp.dim; i++) {
        for (int j = i+1; j < tsp.dim; j++) {

            sprintf(name, "x[%d,%d]", i+1, j+1);
            double obj = tsp.cost(i, j);
            double lb = 0.0;
            double ub = 1.0;

            int status = CPXnewcols(env, lp, 1, &obj, &lb, &ub, &xctype, &namePtr);
            if (status) {
                debug("InsertionError");
                exit(1);
            }
            int numcols = CPXgetnumcols(env, lp);
            if (numcols - 1 != flatPos(i, j, tsp)) {
                debug("InsertionError2");
                exit(1);
            }
        }
    }

    // Adding the constraints
    for (int h = 0; h < tsp.dim; h++) {
        double rhs = 2.0;
        char sense = 'E';
        sprintf(name, "const[%d]", h+1);
        int status = CPXnewrows(env, lp, 1, &rhs, &sense, NULL, &namePtr);
        if (status) {
            debug("CPXnewrows() error code %d\n", status);
        }

        for (int i = 0; i < tsp.dim; i++) {
            if (i == h) continue;

            status = CPXchgcoef(env, lp, h, flatPos(h, i, tsp.dim), 1.0);
            if (status) {
                debug("CPXchgcoef() error code %d\n", status);
            }
        }

    }

}

void setCPLEXParams(CPXENVptr env, double time_limit, int threads = -1, int seed = -1) {
  CPXsetdblparam(env, CPXPARAM_TimeLimit, time_limit);
  if (seed >= 0) CPXsetintparam(env, CPX_PARAM_RANDOMSEED, seed);
  if (threads > 0) CPXsetintparam(env, CPXPARAM_Threads, threads);
  CPXsetdblparam(env, CPX_PARAM_EPINT, 0.0);
	CPXsetdblparam(env, CPX_PARAM_EPGAP, 1e-9);
	CPXsetdblparam(env, CPX_PARAM_EPRHS, 1e-9);

}


struct EdgeIndex {
  int first;
  int second;
};


void cplexExtractSolFromXstars(TSPSolution &sol, const int &dim, const double *xstar){

    EdgeIndex* edges = (EdgeIndex*) malloc(dim * sizeof(EdgeIndex));

    for (int i = 0; i < dim; i++) edges[i].first = -1;

    for(int i = 0; i < dim; i++) {
      for(int j = i + 1; j < dim; j++) {
        if( fabs(xstar[flatPos(i, j, dim)]- 1.0) < EPS) {
          if(edges[i].first == -1) edges[i].first = j;
          else edges[i].second = j;

          if(edges[j].first == -1) edges[j].first = i;
          else edges[j].second = i;
        }
      }
    }

    int currentIndex = 0;
    int comingFrom = edges[currentIndex].first;
    for(int i = 0; i < dim; i++) {
      sol.sequence[i] = currentIndex;
      int nextIndex =  (comingFrom == edges[currentIndex].first)
        ? edges[currentIndex].second
        : edges[currentIndex].first;

      comingFrom = currentIndex;
      currentIndex = nextIndex;
    }

    sol.setCost();

    free(edges);

}


void cplexExtractSolution(TSPSolution &sol, CPXENVptr env, CPXLPptr lp){

    const TSP &tsp = *sol.tsp;
    int status = 0;


    // Use the solution (save it)
    int ncols = CPXgetnumcols(env, lp);
    double *xstar = (double*) malloc(ncols * sizeof(double));
    if ( status = CPXgetx(env, lp, xstar, 0, ncols-1) ) {
      if (status == CPXERR_NO_SOLN)  debug("No Solution exists\n");
      debug("CPXgetx() error code %d\n", status);
      exit(1);
    }

    cplexExtractSolFromXstars(sol, tsp.dim, xstar);



}


int prepare_SEC(const int &dim, int tour, int *comp, char *sense, int *indexes, double *values, double *rhs) {
    int nnz = 0;
    int num_nodes = 0;
    *sense = 'L';

    for (int i = 0; i < dim; i++) {
        if (comp[i] != tour) continue;
        num_nodes++;

        for (int j = i+1; j < dim; j++) {
            if (comp[j] != tour) continue;
            indexes[nnz] = flatPos(i, j, dim);
            values[nnz] = 1.0;
            nnz++;
        }
    }

    *rhs = num_nodes - 1; // |S| - 1

    return nnz;
}




typedef struct {
    int i; // Index of node i
    int j; // Index of node j
} edge;


int countComponents(const int &dim, double* xstar, int* successors, int* comp){ //, edge* close_cycle_edges =NULL, int* num_closed_cycles = NULL) {

    int num_comp = 0;

    for (int i = 0; i < dim; i++ ) {
		if ( comp[i] >= 0 ) continue;

		// a new component is found
		num_comp++;
		int current_node = i;
		int visit_comp = 0; //
    int comp_members = 1;
		while ( !visit_comp ) { // go and visit the current component
			comp[current_node] = num_comp;
			visit_comp = 1; // We set the flag visited to true until we find the successor
			for ( int j = 0; j < dim; j++ ) {
        if (current_node == j || comp[j] >= 0) continue;
				if (fabs(xstar[flatPos(current_node,j,dim)]) >= EPS ) {
					successors[current_node] = j;
					current_node = j;
					visit_comp = 0;
          comp_members++;
					break;
				}
			}
		}
		successors[current_node] = i;  // last arc to close the cycle
    /*if (close_cycle_edges && num_closed_cycles && comp_members > 2) {
        edge e = {i, current_node};
        close_cycle_edges[(*num_closed_cycles)++] = e;
    }*/
	}

    return num_comp;
}



struct TSPCallbackData {
  int ncols;
  TSP *tsp;
};

#endif