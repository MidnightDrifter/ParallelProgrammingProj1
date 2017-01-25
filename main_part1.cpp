#include <fstream>   /* ifstream */
#include <iostream> /*std::cout*/
#include <cstdio>    /* sscanf */
#include "pthread.h" /* thread stuff */
#include "sched.h"
#include "semaphore.h"
#include "Project1\Cells.h"
int rows, cols;
struct threadData {

	int myRow, myCol;
	Cells* myGrid;

};

void* Update(void* data)
{
	//(int[2]) rowCol = (int[2]) data;
	//threadData* myData;
	threadData* myData = (threadData*)data;

	int count = 0;
	//STOP HERE TOO, everyone only goes when EVERYONE is done writing


	for (int i = myData->myRow - 1; i <= myData->myRow + 1; i++)
	{
		for (int j = myData->myCol - 1; j <= myData->myCol + 1; j++)
		{
			if (!(myData->myRow == i && myData->myCol == j))
			{//GAME OF LIFE RULES HERE
				if (i < 0 || j < 0 || i >= rows || j >= cols)
				{
					//Do nothing, or treat as dead?
				}
				else if (!(myData->myGrid->cellGrid[i][j]))
				{
					count++;
				}
			}
		}
	}

	//Everyone waits here until all threads arrive, THEN they update their grid data
	if (count < 2 || count > 3)
	{
		myData->myGrid->cellGrid[myData->myRow][myData->myCol] = false;
	}
	else if (count == 3)
	{
		myData->myGrid->cellGrid[myData->myRow][myData->myCol] = true;
	}

}


int main( int argc, char ** argv ) 
{
    if ( argc != 4 ) {
        std::cout << "expected 3 parameters 1) init population file to read 2) number  iterations 3) final population file to write" << std::endl;
        return 1;
    }
    
	int num_iter;
	num_iter= 0;
    std::sscanf(argv[2],"%i",&num_iter);

    // input  file argv[1]
    // output file argv[3]

	int rows, cols, a,b;

	char* inputFile;
	inputFile= "init0";
	FILE* fp = fopen(inputFile,"r");
	fscanf(fp, "%i, %i", &rows, &cols);

	Cells grid(rows, cols);

	while (!feof(fp))
	{
		fscanf(fp, "%i, %i", &a, &b);
		grid.cellGrid[a][b] = true;
	}


	int numThreads = rows*cols;


	threadData* data = new threadData;
	data->myGrid = &grid;

	pthread_t **threads = new pthread_t*[rows];
	for (int i = 0; i < rows; i++)
	{
		data->myRow = i;
		threads[i] = new pthread_t[cols];
		for (int j = 0; j < cols; j++)
		{
			data->myCol = j;
			pthread_create(&threads[i][j], NULL, Update, (void*)data);
		}
	}



}
