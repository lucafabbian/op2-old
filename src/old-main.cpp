#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <memory.h>
#include <omp.h>
#include <mutex>


#include "tsp.cpp"
#include "heuristic.cpp"

void allPossibleStartsSingleThread(const TSP &tsp){

	// start clock
	clock_t start = clock();

	// try every possible starting point
	TSPSolution *minSol = new TSPSolution(&tsp);
	solveGreed(minSol);
	int minIndex = 0;

	TSPSolution *challenger = new TSPSolution(&tsp);

	for(int i = 1; i < tsp.dim; i++){
	solveGreed(challenger, i);
		if(challenger->cost < minSol->cost){
			auto tmp = minSol;
			minSol = challenger;
			challenger = tmp;
			minIndex = i;
		}
	}
	

	// stop timer
	clock_t end = clock();
	double time = (double)(end - start) / CLOCKS_PER_SEC;

	// print solution cost
	printf("starting point = %d, cost = %d\n", minIndex, minSol->cost);
	//print time
	printf("time = %f\n", time);

	// plot points
	minSol->plot(true);

}

void allPossibleStartsParallel(const TSP &tsp){
	std::mutex mtx;

	// start clock
	clock_t start = clock();

	// try every possible starting point
	TSPSolution *minSol = new TSPSolution(&tsp);
	solveGreed(minSol);
	int minIndex = 0;


	#pragma omp parallel num_threads(8)
	{
		printf("New thread\n");
		TSPSolution *challenger = new TSPSolution(&tsp);
		#pragma omp for
		for(int i = 1; i < tsp.dim; i++){
			challenger->fill();
			solveGreed(challenger, i);
			#pragma omp critical
			if(challenger->cost < minSol->cost){
				auto tmp = minSol;
				minSol = challenger;
				challenger = tmp;
				minIndex = i;
			}
		}

	}


	// stop timer
	clock_t end = clock();
	double time = (double)(end - start) / CLOCKS_PER_SEC;

	// print solution cost
	printf("starting point = %d, cost = %d\n", minIndex, minSol->cost);
	//print time
	printf("time = %f\n", time);

	// plot points
	minSol->plot(true);

}


int main(){

	// get file from env var
	char *filename = getenv("TSP_FILE");
	if(filename == NULL){
		printf("Error: TSP_FILE env var not set\n");
		return 1;
	}

	TSP tsp(filename);

	// initialize random seed
	srand(2306654);


	allPossibleStartsParallel(tsp);

	return 0;
}
