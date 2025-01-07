//Parallel Algorithms and Programming Assignment 
// Issa Jawabreh - OAFE3F

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>


int numOfLines(FILE* Matrixfile) {  // no const here BTW !!
	int c, count;
	count = 0;
	for (;; ) {
		c = fgetc(Matrixfile);
		if (c == EOF)
			break;          // enf of file => we quit

		if (c == '\n')
			++count;        // end of line => increment line counter
	}
	rewind(Matrixfile);

	return count;
}

int main()
{
	//--------------------------------------------------------------------
	//------------------------Generate-Data-------------------------------
	//--------------------------------------------------------------------

	printf("\Generate random data? (Y / N): \t");

	char randomcheck;
	scanf_s("%c", &randomcheck);
	if (randomcheck == 'Y' || randomcheck == 'y')
	{
		int n = 0;
		printf("For an NxN matrix, input N\n");
		scanf_s("%d", &n);
		printf("Generated data written into data.txt\n");
		srand(time(0));
		int upperN = 2000;
		int lowerN = -2000;
		
		double** Matrix;
		Matrix = (double**)calloc(n, sizeof(double*));
		for (int i = 0; i < n; i++)
		{
			Matrix[i] = (double*)calloc(n, sizeof(double));
		}

		for (int row = 0; row < n; row++)
		{
			for (int col = 0; col < n; col++)
			{
				Matrix[row][col] = ((lowerN + (rand() % (upperN - lowerN + 1)))) / 1.7;
			}
		}
		for (int row = 0; row < n; row++)
		{
			for (int col = 0; col < n; col++)
			{
				//printf("%lf\t", Matrix[row][col]);
				//if (col == n - 1)
				//	printf("\n");
			}
		}


		//==========================================
		//write a file
		//==========================================

		char datafile[100];
		printf("Save file as : ");
		scanf("%s", &datafile);
		FILE* fptr;
		fptr = fopen(datafile, "w");

		if (fptr == NULL)
		{
			printf("Error!");
			exit(1);
		}

		for (int row = 0; row < n; row++)
		{
			for (int col = 0; col < n; col++)
			{
				fprintf(fptr, "%lf\t", Matrix[row][col]);
				if (col == n - 1)
					fprintf(fptr, "\n");
			}
		}
		//deallocate memory
		for (int i = 0; i < n; i++)
			free(Matrix[i]);
		free(Matrix);
		fclose(fptr);
	}


	//==========================================
	//Read File
	//==========================================

	 // Opening the Matrix File

	char filename[100];
	printf("Please enter data source file name (with .extension) to analyse: ");
	scanf("%s", &filename);

	int NT = 1;
	printf("Please specify number of threads:\t");
	scanf_s("%d", &NT);


	// Opening the Matrix File
	FILE* Matrixfile;
	Matrixfile = fopen(&filename, "r");
	//Matrixfile = fopen("dataread.txt", "r");
	if (Matrixfile == NULL)
		return 1;

	int Msize = numOfLines(Matrixfile);

	//printf("%d\n\n", Msize);


	// Reading text file into 2D array
	int i, j;


	//Allocating array memory
	double** Matrix2;
	Matrix2 = (double**)calloc(Msize, sizeof(double*));
	for (int i = 0; i < Msize; i++)
		Matrix2[i] = (double*)calloc(Msize, sizeof(double));


	//==========================================
	//Parising Data
	//==========================================

	//omp_set_num_threads(NT);
//#pragma omp parallel for private (j)			// Attempting to parallelize the data parsing code increased the execution time substantially with higher thread count
	for (i = 0; i < Msize; i++) {
		for (j = 0; j < Msize; j++) {
			fscanf(Matrixfile, "%lf", &Matrix2[i][j]);
		}

		char eol;
		fscanf(Matrixfile, "%c", &eol);     //read \n character
	}

	fclose(Matrixfile);
	double* Averages;
	Averages = (double*)calloc(Msize, sizeof(double));


	//==========================================
	//Calcualte Averages // exectution time measurement begins from this point
	//==========================================

	clock_t begin = clock();


	omp_set_num_threads(NT);
#pragma omp parallel for private (j)	
	for (i = 0; i < Msize; i++)
	{
		for (j = 0; j < Msize; j++)
		{
			Averages[i] = Averages[i] + Matrix2[j][i];
			if (j == Msize - 1)
			{
				Averages[i] = Averages[i] / Msize;

			}
		}

	}




	//printf("\n\nAverages:\n");
	//for (i = 0; i < Msize; i++)
	//	printf("%lf\t", Averages[i]);
	//look for elements matchng with mean
	//printf("\n\n\n");
	//printf("\nColumn Indices are:\t");


	//==========================================
	//Searching for values matching Task3 criteria
	//==========================================

	int* task3;
	task3 = (int*)calloc(Msize, sizeof(int));
	int z = 0;

	omp_set_num_threads(NT);
#pragma omp parallel for  private (i,j) //reduction(+:z)
	for (i = 0; i < Msize; i++)
	{
		for (j = 0; j < Msize; j++)
		{
			if (Averages[i] == Matrix2[j][i])
			{
				task3[z] = i;
				z++;
			}
		}
	}


	clock_t end = clock();

	//Print indices

	if (z == 0)
		printf("\nNo Columns are matching the Task 3 Criteria\t");
	else
	{
		printf("\nColumn Indices Matching Task 3 Criteria:\t");
		for (i = 0; i < z; i++)
		{
			//if (task3[i]!=0)
			printf("%d, ", task3[i]);
		}
		printf("\n");
	}

	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("\ntime spent: %lf", time_spent);
	printf(" s");
	printf("\tusing %d threads\n", NT);


	printf("\nPlease see Task3_output.txt\n");


	//write output to file
	FILE* fptr3;
	fptr3 = fopen("Task3_output.txt", "w");
	if (fptr3 == NULL)
	{
		printf("Error!");
		exit(1);
	}

	if (z == 0)
	{
		fprintf(fptr3, "No Columns are matching the Task 3 Criteria\t");
	}
	else {

		fprintf(fptr3, "Column Indices Matching Task 3 Criteria are:\t");
		for (i = 0; i < z; i++)
		{

			fprintf(fptr3, "%d ,", task3[i]);

		}
	}


	fprintf(fptr3, "\ntime spent: %lf", time_spent);
	fprintf(fptr3, " s");
	fprintf(fptr3, "\tusing %d threads\n", NT);
	fclose(fptr3);




	//memory deallocations
	for (int i = 0; i < Msize; i++)
		free(Matrix2[i]);
	free(Matrix2);



	return 0;

}