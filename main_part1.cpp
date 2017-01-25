#include <fstream>   /* ifstream */
#include <iostream> /*std::cout*/
#include <cstdio>    /* sscanf */
#include "pthread.h" /* thread stuff */
#include "sched.h"
#include "semaphore.h"
#include "Cells.h"

struct threadData {



};


int main( int argc, char ** argv ) 
{
    if ( argc != 4 ) {
        std::cout << "expected 3 parameters 1) init population file to read 2) number  iterations 3) final population file to write" << std::endl;
        return 1;
    }
    
    int num_iter = 0;
    std::sscanf(argv[2],"%i",&num_iter);

    // input  file argv[1]
    // output file argv[3]

	int rows, cols, a,b;

	char* inputFile = "init0";
	FILE* fp = fopen(inputFile,"r");
	fscanf(inputFile, "%i, %i", &rows, &cols);

	Cells grid(rows, cols);

	while (!feof(fp))
	{
		fscanf(inputFile, "%i, %i", &a, &b);
		grid.cellGrid[a][b] = true;
	}


	int numThreads = rows*cols;

	void* Update(void* data)
	{
		(int[2]) rowCol = (int[2]) data;
		int count = 0;
		for (int i = rowCol[0] - 1; i <= rowCol[0] + 1; i++)
		{
			for (int j = rowCol[1] - 1; j <= rowCol[1] + 1; j++)
			{
				if (!(rowCol[0] == i && rowCol[1] == j))
				{//GAME OF LIFE RULES HERE
					if (i < 0 || j < 0 || i >= rows || j >= cols || )
					{
						//Do nothing, or treat as dead?
					}
					else if (!grid.cellGrid[i][j])
					{
						count++;
					}
				}
			}
		}

		if (count < 2 || count > 3)
		{
			grid.cellGrid(rowCol[0], rowCol[1]) = false;
		}
		else if (count == 3)
		{
			grid.cellGrid(rowCol[0], rowCol[1]) = true;
		}

	}


	int data[2] = { 0,0 };

	pthread_t threads[][] = new pthread_t*[rows];
	for (int i = 0; i < rows; i++)
	{
		data[0] = i;
		threads[i] = new pthread_t[cols];
		for (int j = 0; j < cols; j++)
		{
			data[1] = j;
			pthread_create(&threads[i][j], NULL, Update, (void*)data);
		}
	}



}
