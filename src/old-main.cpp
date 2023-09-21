/*


void nearestPlusTwoOpt(TSPSolution *sol, const time_t timeLimit, const int startingPoint = 0){
  nearest(sol, timeLimit, startingPoint);
  twoOpt(sol);
}

int main () {
  TSP tsp("./data/pr1002.tsp");
  time_t timeLimit = time(NULL) + (2 *60 );
  printResult(solveMultiThread(nearestPlusTwoOpt, tsp, timeLimit, {0, tsp.dim, 1}));
}




void repeatGrasp (TSPSolution *sol, const time_t timeLimit, const int startingPoint = 0){
  nearestWithGrasp(sol, timeLimit, startingPoint, 50); // 100 is 10.0%
  twoOpt(sol);
}

int main(){

  srand(1234543454);
  TSP tsp("./data/pr1002.tsp");
  time_t timeLimit = time(NULL) + ( 5 * 60 );
  printResult(solveMultiThreadEndless(repeatGrasp, tsp, timeLimit, {0, tsp.dim, 1}));
}



int main() {
  TSP tsp("./data/pr1002.tsp");
  TSPSolution sol(&tsp);

  time_t timeLimit = time(NULL) + (2 );
  solveGreed(&sol, timeLimit);
  twoOpt(&sol);

  timeLimit = time(NULL) + (20 * 60 );
  genetic(&sol, timeLimit);

  //sol.plot();
  //printf("%d %s\n", sol.cost, sol.isValid()? "VALID" : "INVALID");
}




void extraMileageWithMaximum(TSPSolution *sol, const time_t timeLimit, const int startingPoint = 0){
  extraMileage(sol, timeLimit, startingPoint, -1);
}

void extraMileageWithMaximumAndTwoOpt(TSPSolution *sol, const time_t timeLimit, const int startingPoint = 0){
  extraMileage(sol, timeLimit, startingPoint, -1);
  twoOpt(sol);
}

int main(){
  TSP tsp("./data/pr136.tsp");
  time_t timeLimit = time(NULL) + (0.5 );
  printResult(solveSingleThread(extraMileageWithMaximum, tsp, timeLimit, {0, tsp.dim, 1}));
  printResult(solveSingleThread(extraMileageWithMaximumAndTwoOpt, tsp, timeLimit, {0, tsp.dim, 1}));
}







*/


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
	time_t start = time(NULL);

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
	time_t end = time(NULL);
	int time = end- start;;

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
	time_t start = time(NULL);

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
	time_t end = time(NULL);
	int time = end- start;;

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
