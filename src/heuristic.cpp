
// include guard
#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "utils.cpp"
#include "tsp.cpp"
#include <limits.h>




void nearest(TSPSolution *sol, const time_t timeLimit,  const int startingPoint){
	TSP &tsp = *sol->tsp;
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

void nearest(TSPSolution *sol, const time_t timeLimit){
  nearest(sol, timeLimit, 0);
}

void nearestWithGrasp(TSPSolution *sol, const time_t timeLimit, const int startingPoint = 0, int graspFactor = 0){
	TSP &tsp = *sol->tsp;
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
	int secondMinIndex, secondMinCost = 0;


	for(int i = 0; (i + 1) < tsp.dim ; i++){
		// initialize te minimum cost to the cost of the edge between the current city and the next one
		minIndex = -1;
		minCost = INT_MAX;

    secondMinIndex = -1;
		secondMinCost = INT_MAX;

		// find the minimum cost edge between the current city and the rest of the cities
		for(int j = i + 1; j < tsp.dim; j++){
			const int cost = tsp.cost(sol->sequence[i], sol->sequence[j]);
			if(cost < secondMinCost){
        if(cost < minCost){
          secondMinCost = minCost;
          secondMinIndex = minIndex;
          minCost = cost;
          minIndex = j;
        }else {
          secondMinCost = cost;
          secondMinIndex = j;
        }
			}
		}


		// swap the city with the minimum cost with the next city
    int randomInt = rand() % 1000;
    if(secondMinIndex != -1 && randomInt < graspFactor){
      swap(sol->sequence, i + 1, secondMinIndex);
		  sol->cost += secondMinCost;
    }else{
      swap(sol->sequence, i + 1, minIndex);
		  sol->cost += minCost;
    }

	}

	sol->cost += tsp.cost(sol->sequence[tsp.dim - 1], sol->sequence[0]);
}


void nearestWithGrasp(TSPSolution *sol, const time_t timeLimit, const int startingPoint = 0){
  nearestWithGrasp(sol, timeLimit, startingPoint, 100);
}


void extraMileageStep(TSPSolution *sol, const time_t timeLimit, const int startingPoint, int remaining){
	TSP &tsp = *sol->tsp;
  while(remaining != 0){

    int minIndex, minRemaining, minCost = INT_MAX;
    int index = startingPoint;
    do{
      int nextIndex = sol->auxSequence[index];
      for(int i = 0; i < remaining; i++){
        int costIncrease = tsp.cost(index, sol->sequence[i]) + tsp.cost(sol->sequence[i], nextIndex) - tsp.cost(index, nextIndex);
        if(costIncrease < minCost){
          minCost = costIncrease;
          minIndex = index;
          minRemaining = i;
        }
      }
      index = nextIndex;
    }while(index != startingPoint);
    int minNextIndex = sol->auxSequence[minIndex];
    int removingIndex = sol->sequence[minRemaining];
    sol->auxSequence[minIndex] = removingIndex;
    sol->auxSequence[removingIndex] = minNextIndex;

    // remove node from remaining pool
    remaining-= 1;
    sol->sequence[minRemaining] = sol->sequence[remaining];

  }
}

void extraMileage(TSPSolution *sol, const time_t timeLimit, const int startingPoint = 0, int secondPoint = -1){
	TSP &tsp = *sol->tsp;
	if(sol->isAuxFilled == false){
		sol->auxFill();
	}




  if(secondPoint < 0){
    int maxDistance = 0;
    int maxDistancePoint = -1;
    for(int i = 0; i < tsp.dim; i++){
      if(i != startingPoint){
        int cost = tsp.cost(startingPoint, i);
        if(cost > maxDistance){
          maxDistance = cost;
          maxDistancePoint = i;
        }
      }
    }
    secondPoint = maxDistancePoint;
  }

  sol->auxSequence[startingPoint] = secondPoint;
  sol->auxSequence[secondPoint]  = startingPoint;

  int remaining = tsp.dim  - 2; // -2 is for the two points
	sol->fill();
  sol->sequence[startingPoint] = tsp.dim - 1;
  sol->sequence[secondPoint] = tsp.dim - 2;

  extraMileageStep(sol, timeLimit, startingPoint, remaining);


  int index = startingPoint;
  for(int i = 0; i < tsp.dim; i++){
    sol->sequence[i] = index;
    index = sol->auxSequence[index];
  }

  sol->setCost();
}


void twoOptSwap(TSPSolution *sol, const int i, const int j) {
  int ki = i + 1;
  int kj = j;
  while(ki < kj){
    swap(sol->sequence, ki, kj);
    ki++;
    kj--;
  }

}

void twoOpt(TSPSolution *sol){
  TSP &tsp = *sol->tsp;
  bool improving = true;
  while(improving){
    improving = false;

    for(int i = 0; i < tsp.dim - 1; i++){
      for(int j = i + 2; j < tsp.dim; j++){
        int cost1 = tsp.cost(sol->sequence[i], sol->sequence[i+1]) + tsp.cost(sol->sequence[j], sol->sequence[(j+1) % tsp.dim]);
        int cost2 = tsp.cost(sol->sequence[i], sol->sequence[j]) + tsp.cost(sol->sequence[i+1], sol->sequence[(j+1) % tsp.dim]);

        if(cost1 > cost2){
          sol->cost += cost2 - cost1;
          improving = true;
          twoOptSwap(sol, i, j);
        }
      }
    }
  }
}


void nearestPlusTwoOpt(TSPSolution *sol, const time_t timeLimit, const int startingPoint = 0){
  nearest(sol, timeLimit, startingPoint);
  twoOpt(sol);
}

void nearestWithGraspPlusTwoOpt(TSPSolution *sol, const time_t timeLimit, const int startingPoint = 0){
  nearestWithGrasp(sol, timeLimit, startingPoint, 100);
  twoOpt(sol);
}



#endif