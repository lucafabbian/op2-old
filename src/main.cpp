#define IPYCPP
// $$ipycpp_file:   src/main.cpp
// $$ipycpp_build:  mkdir -p bin && g++ -o3 -o bin/main src/main.cpp -fopenmp
// $$ipycpp_run:    bin/main
// $$ipycpp_clevermain: true

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





void generateRandomTSPFile(const char filename[], const int num_cities = 100, const int max_value = 1000){
  FILE *outfile = fopen(filename, "w");
  if (outfile == NULL) {
      printf("Error: cannot open file %s for writing.\n", filename);
      exit(1);
  }
  fprintf(outfile, "NAME: Random-%d\n", num_cities);
  fprintf(outfile, "TYPE: TSP\n");
  fprintf(outfile, "DIMENSION: %d\n", num_cities);
  fprintf(outfile, "EDGE_WEIGHT_TYPE: EUC_2D\n");
  fprintf(outfile, "NODE_COORD_SECTION\n");

  for (int i = 1; i <= num_cities; i++) {
      int x = (int) ((((double) rand()) / RAND_MAX) * max_value); // Random x-coordinate between 0 and 1000
      int y = (int) ((((double) rand()) / RAND_MAX) * max_value); // Random y-coordinate between 0 and 1000
      fprintf(outfile, "%d %d %d\n", i, x, y);
  }

  fprintf(outfile, "EOF\n");
  fclose(outfile);
}




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

auto html = R""""(

<div class="widgetcontainer">
  <div class="widget" v-scope="{ count: 0 }">
    <button @click="count--">-</button>
    {{ count }}
    <button @click="count++">+</button>
  </div>

  <script>
  if(!window.INSTALL_PETITE_VUE){
    let resolve = null;
    window.INSTALL_PETITE_VUE = new Promise(r => resolve = r);
    function loadPetiteVue(){
      var script = document.createElement('script');
      script.src = 'https://unpkg.com/petite-vue';
      script.onload = resolve;
      script.onerror = () => setTimeout(loadPetiteVue, 1000)
      document.head.appendChild(script);
    }
    loadPetiteVue()
  }

  {
    // get current element right now, and mount it as soon as petite-vue is loaded
    let element = document.currentScript.previousElementSibling;
    window.INSTALL_PETITE_VUE.then(() => PetiteVue.createApp().mount(element));
  }

  </script>
</div>
)"""";







## Tabu search

#ifndef IPYCPP_MAIN_METHOD
int main(){ return 0;}
#endif
