#include "ga.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>       /* fabs */
#include <vector>
#include <algorithm>
#include <functional>
#include <assert.h>

using namespace std;

Matrix::Matrix(const int & m, const int & n)
{
	rows=m;
	columns=n;
	data = new int* [m];

	for ( int i=0;i<m;++i)
	{
		data[i]=new int [n];
		Row* newrow = new Row(i,0.0);
		rowsVec.push_back(newrow);
		rowsMap[i]=newrow;
		//double score=i;
		for (int j=0;j<n;++j)
		{
			data[i][j]=rand()%2;
		}		

	}

	mutate_column = new double [n];
	b_column = new double [n];

	updateProb();
}

Matrix::~Matrix()
{
	if (data!=NULL)
	{
		for ( int i=0;i<rows;++i)
		{
			if (data[i]!=NULL)
			{
				delete [] data[i];
			}
		}
		delete [] data;
	}
	if (mutate_column!=NULL)
		delete mutate_column;
	if (b_column!=NULL)
		delete b_column;
}

void Matrix::updateProb()
{
	b_board.clear();
	int normalize=(1+rows)*rows/2;
	for ( int j=0;j<columns;++j)
	{
		int accum=0;
		for ( int i=0;i<rows;++i)
		{
			accum+=(rows-i)*data[i][j];			
		}
		//pj1 in the paper
		mutate_column[j]=static_cast<double> (accum)/normalize;
		//bj in paper without normalize
		double thenum = 1.0-fabs(1.0-mutate_column[j]-0.5)-fabs(mutate_column[j]-0.5);
		b_column[j]=1.0-fabs(1.0-mutate_column[j]-0.5)-fabs(mutate_column[j]-0.5);
		b_board.push_back(pair<double, int>(thenum,j)); 
	}
}

//score_board contains row num and the row's score 
void Matrix::setRowscore(const int & row_num,const float & score)
{
	rowsMap[row_num]->row_score=score;
}

void Matrix::mogaR()
{
	//sortRowByScore();
	sort(b_board.begin(),b_board.end());


	for ( int i=0;i<floor(4*rows/5);++i)
	{
	   double x=((double) rand() / (RAND_MAX));
	   double ai=static_cast<double> (i)/(rows-1);
	   if (x<0)
	   	cout<<"GG"<<endl;
	 
	   if (x< ai)
	   {
	   	  assert(i!=0);

	   	  int now_row = rowsVec[i]->row_id;
	   	  int k=floor(ai*columns);
	   	  if (k<0)
	   	  	cout<<"error"<<endl;
	   	//  cout<<"is changing"<<k<<" loci"<<endl;
	   	  for (int j=0;j<k;++j)
	   	  {
	   	  	  int now_col = b_board[j].second;
	   	  	  int tochange=0;
	   	  	  if (data[now_row][now_col]==0)
	   	  	  	tochange=1;
	   	  	  else
	   	  	  	tochange=0;
	   	  	  data[now_row][now_col]=tochange;
	   	  }
	   }
	}


	for (int i=floor(4*rows/5);i<rows;++i)
	{
		//cout<<"Row "<<rowsVec[i]->row_id<<"is randoming"<<endl;
		for (int j=0;j<columns;++j)
		{
			data[i][j]=rand()%2;
		}
	}

	updateProb();

}

void Matrix::mogaC()
{
	sortRowByScore();
	sort(b_board.begin(),b_board.end());

	for (int i=0;i<columns;++i)
	{
		double y=((double) rand() / (RAND_MAX));
	    //double ai=static_cast<double> (i)/(score_board.size()-1);
	    if(y<b_column[i])
	    {
	    	int w=floor(b_column[i]*rows);
	    	if (w<0)
	    		cout<<"error"<<endl;
	    	for (int j=rows-w;j<rows;++j)
	    	{
	    		int now_row=rowsVec[j]->row_id;
	    		int tochange=0;
	    		if (data[now_row][i]==0)
	   	  	  	  tochange=1;
	   	  	    else
	   	  	  	  tochange=0;
	   	  	    data[now_row][i]=tochange;
	    	}
	    }
	}

	updateProb();
}


void Matrix::print()
{

	sortRowByScore();

	cout<<endl;
	cout<<endl;

	cout<<"Matrix : "<<endl;
	for (int i=0;i<rows;++i)
	{
		cout<<"row "<<i<<" :";
	    for ( int j=0;j<columns;++j)
	    {
		   cout<<data[i][j]<<" ";
	    }
	    cout<<endl;
	}

	cout<<"mutate_column ";
	for ( int j=0;j<columns;++j)
	{
		cout<<mutate_column[j]<<" ";
	}
	cout<<endl;

	cout<<"b_column ";
	for (int j=0;j<columns;++j)
	{
		cout<<b_column[j]<<" ";
	}
	cout<<endl;


	cout<<"IN rowsVec"<<endl;
	for (unsigned int i=0;i<rowsVec.size();++i)
	{
		cout<<"Row is "<<rowsVec[i]->row_id<<endl;
		cout<<"Score is "<<rowsVec[i]->row_score<<endl;
	    
	}
	cout<<endl;
	
}

bool RowCompByScore(const Row* a,const Row* b)
{
	if (a->row_score > b->row_score)
		return true;
	else
		return false;
}

void Matrix::sortRowByScore()
{
	sort(rowsVec.begin(),rowsVec.end(),RowCompByScore);
}