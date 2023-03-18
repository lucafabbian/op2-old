
// include guard 
#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "tsp.cpp"


TSPSolution *solveGreedCopilot(const TSP &tsp){
	TSPSolution *sol = new TSPSolution(&tsp);
	bool *visited = (bool *) malloc(tsp.dim * sizeof(bool));
	memset(visited, false, tsp.dim * sizeof(bool));
	int current = 0;
	int next = 0;
	int min = 0;
	int minIndex = 0;
	int cost = 0;
	for(int i = 0; i < tsp.dim; i++){
		sol->sequence[i] = current;
		visited[current] = 1;
		min = 1000000000;
		for(int j = 0; j < tsp.dim; j++){
			if(visited[j] == 0){
				if(tsp.cost(current, j) < min){
					min = tsp.cost(current, j);
					minIndex = j;
				}
			}
		}
		cost += min;
		current = minIndex;
	}
	sol->sequence[tsp.dim] = 0;
	sol->cost = cost;
	return sol;
}

// swap two elements
void swap(int *a, int *b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

// swap two elements inside an array
void swap(int *a, const int i, const int j){
	int temp = a[i];
	a[i] = a[j];
	a[j] = temp;
}


void solveGreed(TSPSolution *sol,const int startingPoint = 0){
	const TSP &tsp = *sol->tsp;
	if(sol->isFilled == false){
		sol->fill();
		// if a starting point is given, put it at the beginning
		if(startingPoint > 0) swap(sol->sequence, 0, startingPoint);
	}else{
		// look for the starting point in the sequence
		for(int i = 0; i < tsp.dim; i++){
			if(sol->sequence[i] == startingPoint){
				// put the starting point at the beginning
				swap(sol->sequence, 0, i);
				break;
			}
		}
	}

	sol->cost = 0;
	int minIndex, minCost = 0;

	for(int i = 0; (i + 1) < tsp.dim ; i++){
		// initialize the minimum cost to the cost of the edge between the current city and the next one
		minIndex = i + 1;
		minCost = tsp.cost(sol->sequence[i], sol->sequence[minIndex]);
		
		// find the minimum cost edge between the current city and the rest of the cities
		for(int j = i + 2; j < tsp.dim; j++){
			const int cost = tsp.cost(sol->sequence[i], sol->sequence[j]);
			if(cost < minCost){
				minIndex = j;
				minCost = cost;
			}
		}

		// swap the city with the minimum cost with the next city
		swap(sol->sequence, i + 1, minIndex);
		sol->cost += minCost;

		// print the selected minimum cost edge
		//printf("min cost edge: %d %d %d\n", sol->sequence[i], sol->sequence[i + 1], minCost);
	}

	sol->cost += tsp.cost(sol->sequence[tsp.dim - 1], sol->sequence[0]);
}


#endif