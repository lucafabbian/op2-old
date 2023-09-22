
// include guard
#ifndef METAHEURISTIC_H
#define METAHEURISTIC_H

#include <algorithm>
#include <time.h>
#include <limits.h>

#include "utils.cpp"
#include "tsp.cpp"
#include "heuristic.cpp"

void vns(TSPSolution *sol, time_t timeLimit){
  time_t start = time(NULL);

  TSP &tsp = *sol->tsp;
  TSPSolution currentSolution {sol->tsp};
  TSPSolution * current = &currentSolution;
  TSPSolution tempSolution { sol->tsp};
  TSPSolution * temp = &tempSolution;

  for(int i = 0; i < tsp.dim; i++) {
    current->sequence[i] = sol->sequence[i];
  }
  current->cost = sol->cost;
  debug("best cost: %d ", current->cost); printTime(start);
  timecost(current->cost);


  while(time(NULL) < timeLimit){
    // 3opt kick

    int a, b, c;
    // find random a, b, c different from each other
    a = rand() % tsp.dim;
    do{ b = rand() % tsp.dim;
    }while(b == a);
    do{ c = rand() % tsp.dim;
    }while(c == a || c == b);
    // reorder a, b, c
    int lowest = std::min(std::min(a, b), c);
    int greatest = std::max(std::max(a, b), c);
    int middle = a + b + c - lowest - greatest;

    // calculate cost of restored tour
    temp->cost = current->cost;
    temp->cost -= tsp.cost(current->sequence[lowest], current->sequence[lowest + 1]); // remove old edges
    temp->cost -= tsp.cost(current->sequence[middle], current->sequence[middle + 1]);
    temp->cost -= tsp.cost(current->sequence[greatest], current->sequence[(greatest + 1) % tsp.dim]);
    temp->cost += tsp.cost(current->sequence[lowest], current->sequence[greatest]);   // add new edges
    temp->cost += tsp.cost(current->sequence[middle + 1], current->sequence[lowest + 1]);
    temp->cost += tsp.cost(current->sequence[middle], current->sequence[(greatest + 1) % tsp.dim]);

    // restore the tour
    int tempCount = 0;
    for(int i = 0; i <= lowest; i++) {
      temp->sequence[tempCount] = current->sequence[i];
      tempCount++;
    }


    for(int i = greatest; i >= middle + 1; i--) {
      temp->sequence[tempCount] = current->sequence[i];
      tempCount++;
    }

    for(int i = lowest + 1; i <= middle; i++) {
      temp->sequence[tempCount] = current->sequence[i];
      tempCount++;
    }

    for(int i = greatest + 1; i < tsp.dim ; i++) {
      temp->sequence[tempCount] = current->sequence[i];
      tempCount++;
    }


    twoOpt(temp);


    if(temp->cost < current->cost){
      TSPSolution * tempBest = current;
      current = temp;
      temp = tempBest;
    }

    if(current->cost < sol->cost){
      timecost(current->cost);
      debug("best cost: %d ", current->cost); printTime(start);

      for(int i = 0; i < tsp.dim; i++) {
        sol->sequence[i] = current->sequence[i];
      }
      sol->cost = current->cost;
    }
  }

}



typedef void (*TabuCallback)(int &tabuTenure, const int iteration, const TSPSolution *sol, const TSPSolution *bestSol);

void stepTabu(int &tabuTenure, const int iteration, const TSPSolution *sol, const TSPSolution *bestSol){
  const int changeEvery = 100;
  const int max = sol->tsp->dim * 0.1;
  const int min = 0;
  tabuTenure = (iteration% (changeEvery)) == (iteration% (changeEvery * 2)) ? max : min;
}

void linearTabu(int &tabuTenure, const int iteration, const TSPSolution *sol, const TSPSolution *bestSol){
  const int max = sol->tsp->dim * 0.1;
  const int min = 0;

  const int diff = max - min;
  const int remain = iteration % (diff * 2);
  if(remain < diff) tabuTenure = max - remain;
  else tabuTenure = min + remain - diff;
}


void randomTabu(int &tabuTenure, const int iteration, const TSPSolution *sol, const TSPSolution *bestSol){
  const int changeEvery = 100;
  const int max = sol->tsp->dim * 0.1;
  const int min = 0;
  if(iteration% changeEvery) tabuTenure = rand() % (max - min + 1) + min;
}


void tabu(TSPSolution *firstSol, time_t timeLimit, TabuCallback tabuCallback = linearTabu){
  TSP &tsp = *firstSol->tsp;

  TSPSolution sol {&tsp};
  for(int i = 0; i < tsp.dim; i++) sol.sequence[i] = firstSol->sequence[i];
  sol.cost = firstSol->cost;


  int* tabuList = (int*) malloc(tsp.dim * sizeof(int));
  for(int i = 0; i < tsp.dim; i++) tabuList[i] = INT_MIN;
  int iteration = 0;
  int tabuTenure = 5;


  while(time(NULL) < timeLimit){
    tabuCallback(tabuTenure, iteration, &sol, firstSol);
    bool improving = true;
    while(improving){
      improving = false;

      for(int i = 0; i < tsp.dim - 1; i++){
        for(int j = i + 2; j < tsp.dim; j++){
          int cost1 = tsp.cost(sol.sequence[i], sol.sequence[i+1]) + tsp.cost(sol.sequence[j], sol.sequence[(j+1) % tsp.dim]);
          int cost2 = tsp.cost(sol.sequence[i], sol.sequence[j]) + tsp.cost(sol.sequence[i+1], sol.sequence[(j+1) % tsp.dim]);

          if((cost1 > cost2) && std::max({
            tabuList[sol.sequence[i]],
            tabuList[sol.sequence[j]],
            tabuList[sol.sequence[i+1]],
            tabuList[sol.sequence[(j+1) % tsp.dim]],
          }) < (iteration - tabuTenure)) {
            sol.cost += cost2 - cost1;
            improving = true;
            twoOptSwap(&sol, i, j);
          }
        }
      }
    }

    //printf("outimproving %d %d\n", sol.cost, tabuTenure);

    // update incumbent
    if(sol.cost < firstSol->cost){
      timecost(sol.cost);
      for(int i = 0; i < tsp.dim; i++) firstSol->sequence[i] = sol.sequence[i];
      firstSol->cost = sol.cost;
    }
    if(time(NULL) > timeLimit) break;


    int i, j;
    i = rand() % tsp.dim;
    do{ j = rand() % tsp.dim;}while( j == i || j == i + 1 || j == i - 1);
    if(j < i) swap(&i, &j);
    const int cost1 = tsp.cost(sol.sequence[i], sol.sequence[i+1]) + tsp.cost(sol.sequence[j], sol.sequence[(j+1) % tsp.dim]);
    const int cost2 = tsp.cost(sol.sequence[i], sol.sequence[j]) + tsp.cost(sol.sequence[i+1], sol.sequence[(j+1) % tsp.dim]);

    const int delta = cost1 - cost2;
    sol.cost -= delta;
    twoOptSwap(&sol, i, j);

    tabuList[sol.sequence[i]] = iteration;
    iteration++;
  }

  free(tabuList);
}



double metropolis(const double temperature, const double delta){
  return exp(-delta / temperature);
}


void annealing(TSPSolution *firstSol, time_t timeLimit){
  const double maxTemperature = 100;
  const double minTemperature = 5;
  const double tempDecrease = 0.95;

  TSP &tsp = *firstSol->tsp;

  TSPSolution sol {&tsp};
  for(int i = 0; i < tsp.dim; i++) sol.sequence[i] = firstSol->sequence[i];
  sol.cost = firstSol->cost;

  int temperature = maxTemperature;

  while(time(NULL) < timeLimit){

    int i, j;
    i = rand() % tsp.dim;
    do{ j = rand() % tsp.dim;}while( j == i );
    if(j < i) swap(&i, &j);
    const int cost1 = tsp.cost(sol.sequence[i], sol.sequence[i+1]) + tsp.cost(sol.sequence[j], sol.sequence[(j+1) % tsp.dim]);
    const int cost2 = tsp.cost(sol.sequence[i], sol.sequence[j]) + tsp.cost(sol.sequence[i+1], sol.sequence[(j+1) % tsp.dim]);

    const int delta = cost1 - cost2;

    if(delta > 0 || metropolis(temperature, delta) > (rand() / (double) RAND_MAX)){
      sol.cost -= delta;
      twoOptSwap(&sol, i, j);
      temperature *= tempDecrease;
    }

    // update incumbent
    if(sol.cost < firstSol->cost){
      timecost(sol.cost);
      for(int i = 0; i < tsp.dim; i++) firstSol->sequence[i] = sol.sequence[i];
      firstSol->cost = sol.cost;
    }
  }

}


void genetic(TSPSolution *sol, time_t timeLimit, const int initialPopulation = 10){
  TSP &tsp = *sol->tsp;

  double start = time(NULL);

  const int numberOfGoodIndividuals = initialPopulation* 0.4; // this will be generated with greed instead of random
  const int childrenPerGeneration = initialPopulation* 0.6;
  const int mutationPerGeneration = initialPopulation* 0.1;
  const int totalSize = initialPopulation + childrenPerGeneration + mutationPerGeneration;
  const int mutationSize = tsp.dim /100;
  const int keepBad = initialPopulation *0.1;

  // Initialize the population
  TSPSolution * population[totalSize];
  for(int i = 0; i < totalSize; i++){
    population[i] = new TSPSolution(&tsp);
  }

  for(int i = 0; i < numberOfGoodIndividuals; i++) {
    const int startingIndex = rand() % sol->tsp->dim;
    nearest(population[i], timeLimit, startingIndex);
    twoOpt(population[i]);
  }

  for (int i = numberOfGoodIndividuals; i < initialPopulation; i++) {
      population[i]->fill();
      population[i]->shuffle();
      population[i]->setCost();
      twoOpt(population[i]);
      if(time(NULL) > timeLimit) return;
  }


  while(time(NULL) < timeLimit){
    debug("iter start"); printTime(start);

    // generate the children
    for(int i = initialPopulation; i < initialPopulation + childrenPerGeneration; i++) {
      if(time(NULL) > timeLimit) return;
      auto child = population[i];
      int parent1, parent2;
      parent1 = randomLinearIndex(initialPopulation);
      do{ parent2 = rand() % initialPopulation;}while( parent1 == parent2 );

      int splitPoint = (tsp.dim/4) +  (rand() % (tsp.dim/2));

      for(int j = 0; j < splitPoint; j++) {
        child->sequence[j] = population[parent1]->sequence[j];
      }
      for(int j = splitPoint; j < tsp.dim; j++) {
        child->sequence[j] = population[parent2]->sequence[j];
      }

      // repairing
      child->auxFill();

      const int firstNode = child->sequence[0];
      int attachTo = firstNode;
      child->auxSequence[firstNode] = -1;
      for(int j = 1; j < tsp.dim; j++) {
        const int current = child->sequence[j];
        if(child->auxSequence[current] == current) {
          child->auxSequence[current] = attachTo;
          attachTo =current;
        }
      }
      child->auxSequence[firstNode] = attachTo;

      int unused = 0;
      for(int j = 0; j < tsp.dim; j++) {
        if(child->auxSequence[j] == j) {
          child->sequence[unused] = j;
          unused++;
        }
      }
      extraMileageStep(child, timeLimit, firstNode, unused);

      int index = rand() % tsp.dim;
      for(int i = 0; i < tsp.dim; i++){
        child->sequence[i] = index;
        index = child->auxSequence[index];
      }

      child->setCost();
      twoOpt(child);

    }



    // generate mutations
    for(int i = initialPopulation + childrenPerGeneration; i < totalSize; i++) {
      if(time(NULL) > timeLimit) return;

      auto child = population[i];
      const int parent = randomLinearIndex(initialPopulation);

      for(int j = 0; j < tsp.dim; j++) {
        child->sequence[j] = population[parent]->sequence[j];
      }
      for(int j = 0; j < mutationSize; j++) {
        swap(child->sequence, rand() % tsp.dim, rand() % tsp.dim);
      }
      child->setCost();
    }

    if(time(NULL) > timeLimit) return;

    std::sort(population, population + totalSize, TSPSolution::compare);

    const int bestCost = population[0]->cost;

    if(bestCost < sol->cost){
      timecost(bestCost);


      //printf("best cost: %d ", bestCost); printTime(start);
      for(int i = 0; i < tsp.dim; i++) {
        sol->sequence[i] = population[0]->sequence[i];
      }
      sol->cost = population[0]->cost;
    }


    for(int i = 0; i < keepBad; i++) {
      const int firstIndex = (rand() % (totalSize - 1)) + 1;
      const int secondIndex =  (rand() % (totalSize - 1)) + 1;

      auto temp = population[firstIndex];
      population[firstIndex] = population[secondIndex];
      population[secondIndex] = temp;
    }

  }

  for (int i = 0; i < totalSize; ++i) {
    delete population[i];
  }

}


#endif