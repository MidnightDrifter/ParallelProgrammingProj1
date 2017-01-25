#pragma once
class Cells
{
public:

	Cells(int r, int c);
	~Cells();

	void Update();

	int numRows, numCols;
	bool[][] cellGrid;
};

