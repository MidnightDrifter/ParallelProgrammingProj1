#include <fstream>   /* ifstream */
#include <iostream> /*std::cout*/
#include <cstdio>    /* sscanf */
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>



class Cells
{
public:

	Cells(int r, int c)
	{
		cellGrid = new bool*[numRows];
		for (int i = 0; i < numRows; i++)
		{
			cellGrid[i] = new bool[numCols];
			for (int j = 0; j < numCols; j++)
			{
				cellGrid[i][j] = false;
			}
		}
	}

	~Cells()
	{
		for (int i = 0; i < numRows; i++)
		{
			delete[] cellGrid[i];
		}
		delete[] cellGrid;
	}
	//Clls& operator=(const Cells& c);
	//void Update();

	int numRows, numCols;
	bool** cellGrid;
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
sem_t readBarrier1, readBarrier2, writeBarrier1, writeBarrier2;
//sem_init(&readBarrier1, 0, 0);




int mutexCount=0;
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
	//Rendezvous point 1 ?
	//pthread_mutex_lock(&mutex);
	//mutexCount++;

	//if (mutexCount == numThreads)
	//{
	//	
	//}


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
	return 0;
}


int main( int argc, char ** argv ) 
{
    if ( argc != 4 ) {
        std::cout << "expected 3 parameters 1) init population file to read 2) number  iterations 3) final population file to write" << std::endl;
        return 1;
    }
    
	int num_iter;
	std::string outFilename, inputFile;
	num_iter= 0;
	std::sscanf(argv[1], "%s", &inputFile);
    std::sscanf(argv[2],"%i",&num_iter);
	std::sscanf(argv[3], "%s", &outFilename);
    // input  file argv[1]
    // output file argv[3]

	int rows, cols, a,b;

	//char* inputFile;
	//inputFile= "init0";
	FILE* fp = fopen(inputFile.c_str(),"r");
	fscanf(fp, "%i, %i", &rows, &cols);

	Cells grid(rows, cols);

	while (!feof(fp))
	{
		fscanf(fp, "%i, %i", &a, &b);
		grid.cellGrid[a][b] = true;
	}

	fclose(fp);
	 numThreads = rows*cols;
	mutexCount = numThreads;

	threadData* data = new threadData;
	data->myGrid = &grid;

	pthread_t **threads = new pthread_t*[rows];

	for (int iterations = 0; iterations < num_iter; iterations++)
	{
		for (int i = 0; i < rows; i++)
		{
			data->myRow = i;
			if (iterations == 0)
			{
				threads[i] = new pthread_t[cols];
			}
			for (int j = 0; j < cols; j++)
			{
				data->myCol = j;
				pthread_create(&threads[i][j], NULL, Update, (void*)data);
			}
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
	out.open(outFilename.c_str());
	

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (grid.cellGrid[i][j])
			{
				out << i << ", " << j << std::endl;
			}
		}
	}


}
