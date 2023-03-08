#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>


struct Point {
	int x;
	int y;
};

struct Points {
	int dim;
	struct Point *arr;
};


void plot(struct Points *p, bool show = true) {

	int solDim = 5;
	int solution[5] = {1, 2, 3, 4, 5};

	// plot points
	FILE *fptr;

	fptr = fopen("./plot/points.dat","w");

	if(fptr == NULL){
		printf("Error opening!");   
		exit(1);             
	}

	for (int i = 0; i < p->dim; i++){
		fprintf(fptr, "%d %d\n", p->arr[i].x, p->arr[i].y);
	}
	fclose(fptr);


	// plot solution
	fptr = fopen("./plot/solution.dat","w");

	if(fptr == NULL){
		printf("Error opening!");   
		exit(1);             
	}

	for (int i = 0; i < solDim; i++){
		fprintf(fptr, "%d %d\n", p->arr[solution[i]].x, p->arr[solution[i]].y);
	}
	fclose(fptr);

	if(show) system("gnuplot ./plot/command");
	
}


int main(){

		struct Points p;

		// get file name from env
		char *filename = getenv("FILE");


    FILE* ptr = fopen("./data/att532.tsp", "r");
    if (ptr == NULL) {
        printf("no such file.");
        return 0;
    }


    char buf[5000], buf2[5000];

		while (fscanf(ptr, "%s : %s\n", buf, buf2)){
				if(strcmp(buf, "DIMENSION") == 0) break;
		}

		// convert char to int
		p.dim = atoi(buf2);
		printf("dim = %d\n", p.dim);

		// skip to NODE_COORD_SECTION
    while (fscanf(ptr, "%[^\n]\n", buf)){
      if(strcmp(buf, "NODE_COORD_SECTION") == 0) break;
		}

		// create new array of dimension dim
		p.arr = (Point *) malloc(p.dim * sizeof(struct Point));


		// read data
		int b, c;
		
		int i = 0;
		while (fscanf(ptr, "%*d %d %d\n", &b, &c) == 2){
			p.arr[i].x = b;
			p.arr[i].y = c;
			i++;
		}


		// print data
		for (int i = 0; i < p.dim; i++){
			printf("%d %d %d\n", i, p.arr[i].x, p.arr[i].y);
		}

		// close file
		fclose(ptr);


		// plot points
		plot(&p);
				

    return 0;

}












