#include "ga.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

double calcu(int* qq, const int & column)
{
	double score=0.0;
	for(int j=0;j<column;++j)
	{
		score+=static_cast<double> (qq[j])/7;
	}
	return score;
}

void compScore( Matrix* MM)
{
	for (int i = 0; i < MM->rows; ++i)
	{
		MM->setRowscore(i,calcu(MM->data[i],MM->columns));
	}
}

int main(){

	srand(time(NULL));
	Matrix* M_ptr=new Matrix(4,5);

	compScore(M_ptr);
	
	M_ptr->print();

	M_ptr->mogaR();

	M_ptr->print();
}
