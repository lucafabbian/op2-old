// include guard
#ifndef TSP_H
#define TSP_H

// include libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <limits.h>

#include "utils.cpp"


struct Point {
	int x;
	int y;
};

class TSP {
public:
	int dim;
	struct Point *points;
	int * costs;

	TSP(const char * filename) {
		FILE* ptr = fopen(filename, "r");
		if (ptr == NULL) {
				printf("no such file.");
				exit(1);
		}


		char buf[5000], buf2[5000];


    int in = 0;
		while (fscanf(ptr, "%s : %s\n", buf, buf2)){
				if(strcmp(buf, "DIMENSION") == 0){
					// convert char to int
					dim = atoi(buf2);
					break;
				}

        if(in++ > 20){
          printf("No dimension found in tsp file!\n");
          dim = 10;
          exit(1);
        }
		}



		// skip to NODE_COORD_SECTION
		while (fscanf(ptr, "%[^\n]\n", buf)){
			if(strcmp(buf, "NODE_COORD_SECTION") == 0) break;
		}

		// create new array of dimension dim
		points = (Point *) malloc(dim * sizeof(struct Point));
		costs = (int *) malloc(dim * dim * sizeof(int));


		// read data
		int b, c;

		int i = 0;
		while (fscanf(ptr, "%*d %d %d\n", &b, &c) == 2){
			points[i].x = b;
			points[i].y = c;
			i++;
		}

		// compute costs
		for (int i = 0; i < dim; i++){
			for (int j = 0; j < dim; j++){
				costs[i * dim + j] = sqrt(pow(points[i].x - points[j].x, 2) + pow(points[i].y - points[j].y, 2))  + 0.499999999;
			}
		}

		// close file
		fclose(ptr);
	}

	int cost(const int i, const int j) const {
		return costs[i * dim + j];
	}

	void print() const {
		// print data
		for (int i = 0; i < dim; i++){
			printf("%d %d %d\n", i, points[i].x, points[i].y);
		}
	}

	~TSP(){
		free(points);
		free(costs);
	}


	TSP(TSP& t){ printf("error: this should not be called\n");	}


};


class TSPSolution {
public:
	TSP * const tsp;

	bool isFilled = false;
	bool isAuxFilled = false;

	int *sequence;
	int *auxSequence;
	int cost;

	TSPSolution(TSP * const tsp):tsp(tsp){
		sequence = (int *) malloc(tsp->dim * sizeof(int));
		auxSequence = (int *) malloc(tsp->dim * sizeof(int));
		cost = INT_MAX;
	}

	~TSPSolution(){
		free(sequence);
	}


	TSPSolution(TSPSolution& t):tsp(t.tsp){
		printf("errorrrrr");
	}

	void fill(){
		for(int i = 0; i < tsp->dim; i++) sequence[i] = i;
		isFilled = true;
	}

  void setCost(){
    cost = 0;
    for(int i = 0; i < tsp->dim - 1; i++) cost += tsp->cost(sequence[i], sequence[(i + 1)]);
    cost += tsp->cost(sequence[tsp->dim-1], sequence[0]);
  }

  void shuffle(){
    shuffleArray(sequence, tsp->dim);
  }

  void auxFill(){
    for(int i = 0; i < tsp->dim; i++) auxSequence[i] = i;
    isAuxFilled = true;
  }

	bool isValid(bool printValidity = false){
		// check if all cities are visited
		int *visited = (int *) malloc(tsp->dim * sizeof(int));
		for (int i = 0; i < tsp->dim; i++){
			visited[i] = 0;
		}

		for (int i = 0; i < tsp->dim; i++){
			visited[sequence[i]]++;
		}

    if(printValidity){
      for (int i = 0; i < tsp->dim; i++){
        printf("%d %d\n", sequence[i], visited[sequence[i]]);
		  }
    }

		for (int i = 0; i < tsp->dim; i++){
			if(visited[i] != 1) return false;
		}

		return true;
	}

  static bool compare(const TSPSolution* a1, const TSPSolution* a2) {
      return a1->cost < a2->cost;
  }


  void print() {
    for (int i = 0; i < tsp->dim; i++){
      printf("%d %d\n", tsp->points[sequence[i]].x, tsp->points[sequence[i]].y);
		}
  }


	void plot(const bool show = false, const int partial = -1) {
		const int maximum = partial == -1 ? tsp->dim : partial;
    system("mkdir -p ./plot/gnu");

    std::string uuid = generate_uuid();

    std::string filename = "./plot/gnu/points" + uuid + ".dat";
    std::string solution = "./plot/gnu/solution" + uuid + ".dat";

		// plot points
		FILE *fptr;
		fptr = fopen(filename.c_str(),"w");
		if(fptr == NULL){
			printf("Error opening!");
			exit(1);
		}

		for (int i = 0; i < tsp->dim; i++){
			fprintf(fptr, "%d %d\n", tsp->points[i].x, tsp->points[i].y);
		}
		fclose(fptr);


		// plot solution
		fptr = fopen(solution.c_str(),"w");

		if(fptr == NULL){
			printf("Error opening!");
			exit(1);
		}


		for (int i = 0; i < maximum; i++){
			fprintf(fptr, "%d %d\n", tsp->points[sequence[i]].x, tsp->points[sequence[i]].y);
		}
		// close the graph
		fprintf(fptr, "%d %d\n", tsp->points[sequence[0]].x, tsp->points[sequence[0]].y);


		fclose(fptr);
    std::string command = "gnuplot ./plot/gnu/command" + uuid;
    std::string command_file = "./plot/gnu/command" + uuid;
    std::string png = "./plot/gnu/plot" + uuid + ".png";

    fptr = fopen(command_file.c_str(),"w");

    #ifdef IPYCPP

      fprintf(fptr, "set terminal png size 1600,900\n");
      fprintf(fptr, "set output '%s'\n", png.c_str());
      fprintf(fptr, "plot '%s' with points, '%s' with linespoint\n", filename.c_str(), solution.c_str());
      fclose(fptr);

			system(command.c_str());
			printf("$$$ipycppr_file$$$%s\n", png.c_str());

		#else
      fprintf(fptr, "set terminal qt size 500,500\n");
      fprintf(fptr, "plot '%s' with points, '%s' with linespoint\n", filename.c_str(), solution.c_str());
      fprintf(fptr, "pause -1\n");
      fclose(fptr);

      printf("Solution plot: %s\n", command.c_str());
			if(show) system(command.c_str());
		#endif
	}
};


int flatPos(const int &i, const int &j, const int &num_nodes) {
    if (i == j) {
        printf("Flatpos i == j");
        exit(1);
    }
    if (i > num_nodes - 1 || j > num_nodes -1 ) {
        printf("Flatpos out of bounds");
        exit(1);

    }
    if (i > j) return flatPos(j, i, num_nodes);
    return i * num_nodes + j - ((i + 1) * (i + 2)) / 2;

}

int flatPos(const int &i, const int &j, const TSP& tsp) {
  return flatPos(i, j, tsp.dim);
}


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


#endif

