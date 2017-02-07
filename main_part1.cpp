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



class Cells;
class ReusableBarrier;

struct threadData {

	int myRow, myCol, iterations;
	Cells* myGrid;
	ReusableBarrier* r, *w;

};
class Cells
{
public:

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

//	Cells(const Cells& other) { std::cout << "Cells copy ctor called, something is wrong." << std::endl; }

	//Clls& operator=(const Cells& c);
	//void Update();

	int numRows, numCols;
	bool** cellGrid;
	threadData*** dataGrid;
};

//Implementing the Barrier class in the Lil' Book of Semaphores in C++

class ReusableBarrier
{
public:
	ReusableBarrier() {}
//	ReusableBarrier(const ReusableBarrier& other) { std::cout << "Reusable Barrier copy ctor called, something is wrong." << std::endl; }
	ReusableBarrier(int n) : n(n), count(0) { pthread_mutex_init(&mutex, NULL);  sem_init(&turnstile0, 0, 0);  sem_init(&turnstile1, 0, 0); }
	~ReusableBarrier() { pthread_mutex_destroy(&mutex);  sem_destroy(&turnstile0); sem_destroy(&turnstile1); }

	void phase1() {
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
sem_t readBarrier1, readBarrier2, writeBarrier1, writeBarrier2;

//sem_init(&readBarrier1, 0, 0);




int mutexCount = 0;


void* Update(void* data)
{
	//(int[2]) rowCol = (int[2]) data;
	//threadData* myData;
	threadData* myData = static_cast<threadData*>(data);
	int liveCount;





	std::cout << "Board state at start of first thread,  " << rows << " by" << cols << " board." << std::endl;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			std::cout << "(" << i << ", " << j << ") = ";
			if (myData->myGrid->cellGrid[i][j] == true)
			{
				std::cout << "LIVE" << std::endl;
			}
			else
			{
				std::cout << "DEAD" << std::endl;
			}
		}
	}





	for (int x = 0; x < myData->iterations; x++)
	{
		liveCount = 0;

		//STOP HERE TOO, everyone only goes when EVERYONE is done writing
		//Rendezvous point 1 ?
		//pthread_mutex_lock(&mutex);
		//mutexCount++;

		//if (mutexCount == numThreads)
		//{
		//	
		//}
	//	std::cout << "Thread at  (" << myData->myRow << ", " << myData->myCol << ") entered." << std::endl;


		//pthread_mutex_unlock(&mutex);


		if (myData->myCol == 2 && myData->myRow == 2)
		{
			int t = 3;
			t++;
		}



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
				
					else if (true == myData->myGrid->cellGrid[i][j])
					{
					liveCount++;
					}
				}
			}
		}

//	std::cout << "Thread at  (" << myData->myRow << ", " << myData->myCol << ") has read, waiting." << std::endl;

	myData->r->wait();
	//Everyone waits here until all threads arrive, THEN they update their grid data
	//if (count < 2 || count > 3)
	//{
	//	myData->myGrid->cellGrid[myData->myRow][myData->myCol] = false;
	//}
	//else if (count == 3)
	//{
	//	myData->myGrid->cellGrid[myData->myRow][myData->myCol] = true;
	//}



	if (true == myData->myGrid->cellGrid[myData->myRow][myData->myCol])
	{
		if (liveCount < 2 || liveCount>3)
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
		if(liveCount==3)
		{
			myData->myGrid->cellGrid[myData->myRow][myData->myCol] = true;
		}
	}





//	std::cout << "Thread at  (" << myData->myRow << ", " << myData->myCol << ") has written, waiting." << std::endl;

	myData->w->wait();

	}
	return 0;
}


int main( int argc, char ** argv ) 
{
	/*
	int** tester = new int*[3];  // = { 0,1,2,3,4,5,6,7,8 };
	for (int i = 0; i < 3; i++)
	{
		tester[i] = new int[3];
		for (int j = 0; j < 3; j++)
		{
			tester[i][j] = i + 3 * j;
		}
	}
	*/


    if ( argc != 4 ) {
        std::cout << "expected 3 parameters 1) init population file to read 2) number  iterations 3) final population file to write" << std::endl;
        return 1;
    }
    
	int num_iter;
	char outFilename[100];
	char inputFile[100];
	memset(inputFile, 0, 100 * sizeof(char));
	memset(outFilename, 0, 100 * sizeof(char));
	num_iter= 0;
	std::sscanf(argv[1], "%s", inputFile);
    std::sscanf(argv[2],"%i",&num_iter);
	std::sscanf(argv[3], "%s", outFilename);
    // input  file argv[1]
    // output file argv[3]

	int  a,b;
	int trash;
	//char* inputFile;
	//inputFile= "init0";
	FILE* fp = fopen(inputFile,"r");
	trash = fscanf(fp, "%i %i", &rows, &cols);

	Cells grid(rows, cols);

	while (!feof(fp))
	{
		trash = fscanf(fp, "%i %i", &a, &b);
		grid.cellGrid[a][b] = true;
	}

	std::cout << "Initial board state, " << rows << " by" << cols<< " board." << std::endl;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			std::cout << "(" << i << ", " << j << ") = ";
			if (grid.cellGrid[i][j] == true)
			{
				std::cout << "LIVE" << std::endl;
			}
			else
			{
				std::cout << "DEAD" << std::endl;
			}
		}
	}


	trash++;
	fclose(fp);
	 numThreads = rows*cols;
	mutexCount = numThreads;
	ReusableBarrier finishedReading = ReusableBarrier(numThreads);
	 ReusableBarrier finishedWriting = ReusableBarrier(numThreads);

	//threadData* data = new threadData;
	//data->myGrid = &grid;

	pthread_t **threads = new pthread_t*[rows];

	//data->r = &finishedReading;
	//data->w = &finishedWriting;


	for(int i=0;i<rows;i++)
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
	

	for(int i=0;i<rows;i++)
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
			if (true == grid.cellGrid[i][j])
			{
				out << i << " " << j << std::endl;
			}
		}
	}
	out.close();
	return 0;
}
