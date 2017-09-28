// **************************************************************************
//  File       [graph.h]
//  Author     [Yuan-Ting Hsieh, NTUEE]
//  Author     [En-Yeu Fan, NTUEE]
//  Synopsis   [The graph header of 2015 CAD contest problem 5]
//  Modify     [2015/06/18 Yuan-Ting Hsieh]
// **************************************************************************

#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <fstream>
//#include "density_window.h"

#define DIS_INF 2147483647

using namespace std;

class Node;

class Edge {

	public:
		Edge(Node *a, Node *b);
		Node* node[2];

		Node *getNeighbor(Node *n);

		bool operator < (const Edge& rhs) const;

};

class Node {

	public:
		//left-bottom x,y and right-upper x,y
		Node(const int& i,const int& lb_x,const int& lb_y,const int& ru_x,const int& ru_y);
		void addEdge(Edge *e);
		void sortEdge();
		bool overlap(Node* n,const int & alpha,const int & beta);
		int  areaCalc();

		int id;
        int color;
        int group_id;
		bool traveled;
		vector<Edge *> edge;

		//int d;
		Node *prev;
		//int heap_idx;
		int lb_x,lb_y,ru_x,ru_y;
		int area;
		//int alpha,beta;
};

class Group {

	public:
		Group(bool c = false) { colored = c;invert=false;locked=false;grparea=0;grpwin=0; }
		~Group();
		void printGroup();
		void calcgrouparea();
		vector<Node *> groupNodes;
		bool		   colored;
		bool		   invert;
		bool           locked;
        int            grparea;
        int            grpwin;
};

class Window {
	public:
		Window() {colorarea=0;};
		~Window();

		vector<Node *> windowNodes;
		vector<int>    area;
		vector<int> wgroups;
		vector<int> wgroupsarea;
		int colorarea;

};

class Graph {

	public:
		Graph(const string& n="",const int& alpha=0,const int& beta=0,const int& omega=0);
		~Graph();
		
		//void addEdge(const int& v1, const int& v2);
		void addNode(Node* nptr);
		void sortEdgesOfNode();
        void sortNodesByDegree();
        void sortNodesByID();
        void sortNodesByY();
        void sortNodesByX();

		void init();
		Node * getNodeById(const int& id);

		void dfs();
		void dfsvisit(Node*, int, Group*);
		//void bfs(int);

		void printall();
		void constructAll();

        void colorgraph();
		void getboxsize();

		void windowcalc_grparea();
		void greedycolor();
		void greedywindow();
		void averagecolor(int x,int y);
		void ptaveragecolor(Window* win);
		void largegreedy();
		void largecolor(Group* grp);

		void saveState();
		void recallState();

		bool exhausted();
		void gacolor();
		float gaRowgetScore(int * p);

		void output(ofstream & );//output file
		void mapping();//build windows[][]
		void getScoreMap();
	    float getscore();
	    
		void set(const int& which,const bool& toset ){groups[which]->invert=toset;};
		void reset();
		map<int, Node *> nodesMap;
        vector<Node *>   nodes;
		vector<Edge *>   edges;
		vector<Group *>  groups;
		vector<vector<Window*> > windows;
		vector<bool> lastState;
		//density_window   densWindow;
		int alpha, beta, omega;
		string name;
		int colored_graph;
		int B_top,B_bottom,B_right,B_left;
		vector<vector<int> > scoreMap;//scoreMap[windows W][group G]=differential area in window W for group G
};

#endif
