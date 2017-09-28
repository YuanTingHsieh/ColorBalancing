// **************************************************************************
//  File       [ga.h]
//  Author     [Yuan-Ting Hsieh, NTUEE]
//  Synopsis   [The genetic algorithm header of 2015 CAD contest problem 5]
//  Ref        [Adaptive Genetic Algorithm...][K.Y. Szeto and Zhang Jian]
//  Modify     [2015/06/18 Yuan-Ting Hsieh]
// **************************************************************************
#ifndef GA_H
#define GA_H
#include <vector>
#include <map>

using namespace std;
class Row {
public:
	Row(const int &id=0, const double &score=0):row_id(id),row_score(score){};
	~Row();

	int row_id;
	double row_score;
};

class Matrix {
public:
	//m by n matrix
	Matrix(const int & m, const int & n);
	~Matrix();

	void updateProb();
	void setRowscore(const int & row_num,const float & score);

	void print();

	//bool myComp(pair<double,int*> a, pair<double,int*> b);
	void mogaR();
	void mogaC();
	void sortRowByScore();


	int** data;
	double* mutate_column;//probability of column to change to one!!
	double* b_column;
	vector<pair<double,int> > b_board;
	vector<Row* > rowsVec;
	map<int,Row* > rowsMap;
	int rows,columns;
};
#endif