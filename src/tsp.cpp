// include guard
#ifndef TSP_H
#define TSP_H

// include libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


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

		while (fscanf(ptr, "%s : %s\n", buf, buf2)){
				if(strcmp(buf, "DIMENSION") == 0){
					// convert char to int
					dim = atoi(buf2);
					printf("dim = %d\n", dim);
					break;
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


	TSP(TSP& t){
		printf("errorrrrr");
	}


};


class TSPSolution {
public:
	const TSP * const tsp;

	bool isFilled = false;

	int *sequence;
	int cost;

	TSPSolution(const TSP * const tsp):tsp(tsp){
		sequence = (int *) malloc(tsp->dim * sizeof(int));
		cost = 0;
	}

	~TSPSolution(){
		free(sequence);
		printf("Freeing memory\n");
	}


	TSPSolution(TSPSolution& t):tsp(t.tsp){
		printf("errorrrrr");
	}

	void fill(){
		for(int i = 0; i < tsp->dim; i++) sequence[i] = i;
		isFilled = true;
	}

	bool isValid(){
		// check if all cities are visited
		int *visited = (int *) malloc(tsp->dim * sizeof(int));
		for (int i = 0; i < tsp->dim; i++){
			visited[i] = 0;
		}

		for (int i = 0; i < tsp->dim; i++){
			visited[sequence[i]]++;
		}

		for (int i = 0; i < tsp->dim; i++){
			if(visited[i] != 1) return false;
		}
		
		return true;
	}


	void plot(const bool show = false, const int partial = -1) {
		const int maximum = partial == -1 ? tsp->dim : partial;

		// plot points
		FILE *fptr;
		fptr = fopen("./plot/points.dat","w");
		if(fptr == NULL){
			printf("Error opening!");   
			exit(1);             
		}

		for (int i = 0; i < tsp->dim; i++){
			fprintf(fptr, "%d %d\n", tsp->points[i].x, tsp->points[i].y);
		}
		fclose(fptr);


		// plot solution
		fptr = fopen("./plot/solution.dat","w");

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

		if(show) system("gnuplot ./plot/command");
	
	}


	


};

#endif

