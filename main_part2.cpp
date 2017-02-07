#include <fstream>   /* ifstream */
#include <iostream> /*std::cout*/
#include <cstdio>    /* sscanf */
#include <string> //Memset
#include <cstring>


#ifdef _MSC_VER
#include "pthread.h"
#include "sched.h"
#include "semaphore.h"

#else 
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#endif

/*
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
*/


//Multiplexing pseudocode:  Identical to the previous code, but with an additional semaphore to limit reading and writing to only 5 cells at a time
/*
	Read Semaphore Wait
	READ BOARD STATE
	Read Semaphore Signal
	Read Barrier
	

	Write Semaphore Wait
	WRITE TO BOARD
	Write Semaphore Signal
	Write Barrier


	Repeat



*/

class Cells;
class ReusableBarrier;

struct threadData {

	int myRow, myCol, iterations;
	Cells* myGrid, *otherGrid;
	ReusableBarrier* r, *w;

};

class Cells
{
public:

	int numRows, numCols;
	bool** cellGrid;
	threadData*** dataGrid;



	Cells(int r, int c) : numRows(r), numCols(c)
	{
		cellGrid = new bool*[r];
		dataGrid = new threadData**[r];
		for (int i = 0; i < r; i++)
		{
			cellGrid[i] = new bool[c];
			dataGrid[i] = new threadData*[c];
			for (int j = 0; j < c; j++)
			{
				cellGrid[i][j] = false;
				dataGrid[i][j] = new threadData;
				dataGrid[i][j]->myRow = i;
				dataGrid[i][j]->myCol = j;
				dataGrid[i][j]->myGrid = this;
				dataGrid[i][j]->r = NULL;
				dataGrid[i][j]->w = NULL;

			}
		}
	}

	~Cells()
	{
		for (int i = 0; i < numRows; i++)
		{
			delete[] cellGrid[i];
			for (int j = 0; j < numCols; j++)
			{
				delete dataGrid[i][j];
			}
			delete[] dataGrid[i];
		}
		delete[] cellGrid;
		delete[] dataGrid;
	}
	//Clls& operator=(const Cells& c);
	//void Update();

};

//Implementing the Barrier class in the Lil' Book of Semaphores in C++

class ReusableBarrier
{
public:
	ReusableBarrier(int n) : n(n), count(0) { pthread_mutex_init(&mutex, NULL);  sem_init(&turnstile0, 0, 0);  sem_init(&turnstile1, 0, 0); }
	~ReusableBarrier() { pthread_mutex_destroy(&mutex);  sem_destroy(&turnstile0); sem_destroy(&turnstile1); }

	void phase1(){
		pthread_mutex_lock(&mutex);
		count++;
		if (count == n)
		{
			//Send n signals to let all n threads through
			for (int i = 0; i < n; i++)
			{
				sem_post(&turnstile0);
			}
		}
		pthread_mutex_unlock(&mutex);
		sem_wait(&turnstile0);
	}

	void phase2()
	{
		pthread_mutex_lock(&mutex);
		count--;
		if (count == 0)
		{
			for (int i = 0; i < n; i++)
			{
				sem_post(&turnstile1);
			}
		}
		pthread_mutex_unlock(&mutex);
		sem_wait(&turnstile1);
	}

	void wait()
	{
		this->phase1();
		this->phase2();
	}
	int n, count;
	pthread_mutex_t mutex;
	sem_t turnstile0, turnstile1;


};



int rows, cols, numThreads;
pthread_mutex_t mutex;// = new 
sem_t readBarrier1, readBarrier2, writeBarrier1, writeBarrier2, multiplexingRead, multiplexingWrite;
//sem_init(&readBarrier1, 0, 0);




int mutexCount=0;


void* Update(void* data)
{
	//(int[2]) rowCol = (int[2]) data;
	//threadData* myData;
	threadData* myData = static_cast<threadData*>(data);
	
	int count;
	for(int x=0;x<myData->iterations;x++)
   { count = 0;
	
	//STOP HERE TOO, everyone only goes when EVERYONE is done writing
	//Rendezvous point 1 ?
	//pthread_mutex_lock(&mutex);
	//mutexCount++;

	//if (mutexCount == numThreads)
	//{
	//	
	//}
	sem_wait(&multiplexingRead);

	//pthread_mutex_unlock(&mutex);



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
				else if ((myData->myGrid->cellGrid[i][j]))
				{
					count++;
				}
			}
		}
	}
	sem_post(&multiplexingRead);
	myData->r->wait();
	//barrier


	sem_wait(&multiplexingWrite);
	//Everyone waits here until all threads arrive, THEN they update their grid data


if (myData->myGrid->cellGrid[myData->myRow][myData->myCol])
{
	if (count < 2 || count>3)
	{
		myData->myGrid->cellGrid[myData->myRow][myData->myCol] = false;
	}
	else
	{
		myData->myGrid->cellGrid[myData->myRow][myData->myCol] = true;
	}
}

else
{
	if (count == 3)
	{
		myData->myGrid->cellGrid[myData->myRow][myData->myCol] = true;
	}
}



	sem_post(&multiplexingWrite);
	myData->w->wait();
	}
	return 0;
}


int main( int argc, char ** argv ) 
{
    if ( argc != 4 ) {
        std::cout << "expected 3 parameters 1) init population file to read 2) number  iterations 3) final population file to write" << std::endl;
        return 1;
    }
    
	int num_iter, threadsToUse;
	char outFilename[100];
	char inputFile[100];
	memset(inputFile, 0, 100 * sizeof(char));
	memset(outFilename, 0, 100 * sizeof(char));
	num_iter= 0;
	std::sscanf(argv[1], "%i", &threadsToUse);
	std::sscanf(argv[2], "%s", inputFile);
    std::sscanf(argv[3],"%i",&num_iter);
	std::sscanf(argv[4], "%s", outFilename);
    // input  file argv[1]
    // output file argv[3]

	int  a,b;
	sem_init(&multiplexingRead, 0, threadsToUse);
	sem_init(&multiplexingWrite, 0, threadsToUse);
	//char* inputFile;
	//inputFile= "init0";
	int trash;

	FILE* fp = fopen(inputFile,"r");
	trash = fscanf(fp, "%i %i", &rows, &cols);

	Cells grid(rows, cols);
	
	while (!feof(fp))
	{
	trash =	fscanf(fp, "%i %i", &a, &b);
		grid.cellGrid[a][b] = true;
	}
	trash++;
	fclose(fp);
	 numThreads = rows*cols;
	mutexCount = numThreads;
	ReusableBarrier finishedReading(numThreads);
	ReusableBarrier finishedWriting(numThreads);

	threadData* data = new threadData;
	data->myGrid = &grid;
	//int g = 0;

	pthread_t **threads = new pthread_t*[rows];

	data->r = &finishedReading;
	data->w = &finishedWriting;
for (int i = 0; i<rows; i++)
{
	for (int j = 0; j < cols; j++)
	{
		grid.dataGrid[i][j]->r = &finishedReading;
		grid.dataGrid[i][j]->w = &finishedWriting;
		grid.dataGrid[i][j]->iterations = num_iter;
	}
}



	for (int i = 0; i < rows; i++)
	{
		//data->myRow = i;
		
			threads[i] = new pthread_t[cols];
		
		for (int j = 0; j < cols; j++)
		{
			//	data->myCol = j;
			pthread_create(&threads[i][j], NULL, Update, static_cast<void*>(grid.dataGrid[i][j]));

		}
	}


for (int i = 0; i<rows; i++)
{
	for (int j = 0; j < cols; j++)
	{
		pthread_join(threads[i][j], NULL);
	}
}



	std::ofstream out;
	out.open(outFilename, std::fstream::out);
	

	out << rows << " " << cols << std::endl;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (grid.cellGrid[i][j])
			{
				out << i << " " << j << std::endl;
			}
		}
	}
	out.close();
  return 0;
}
