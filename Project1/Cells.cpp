#include "Cells.h"



Cells::Cells(int r, int c) : numRows(r), numCols(c)
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


Cells::~Cells()
{
	for (int i = 0; i < numRows; i++)
	{
		delete[] cellGrid[i];
	}
	delete[] cellGrid;
}
