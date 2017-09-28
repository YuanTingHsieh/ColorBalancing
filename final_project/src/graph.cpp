// **************************************************************************
//  File       [graph.cpp]
//  Author     [Yuan-Ting Hsieh, NTUEE]
//  Author     [En-Yeu Fan, NTUEE]
//  Author     [Yen-Chun Fang, NTUEE]
//  Synopsis   [Implementing graph.h of 2015 CAD contest problem 5]
//  Modify     [2015/06/18 Yuan-Ting Hsieh]
// **************************************************************************

#include "graph.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <cmath> 
#include <limits>
#include <bitset>
#include <stdlib.h>
#include <time.h>
#include "../GA/ga.h"
#include "tm_usage.h"

#define _DEBUG

using namespace std;
bool compare (pair <int,int> i,pair <int,int> j)
		{ return (i.second<j.second); }
bool grpcompare (pair <Group*,int> i,pair <Group*,int> j)
		{ return (i.second<j.second); }
bool wincompare (Window* i,Window* j)
		{ return (i->wgroups.size()<j->wgroups.size()); }
Edge::Edge(Node *a, Node *b)
{
	if ( a->id <= b->id ) { node[0] = a; node[1] = b; }
	else { node[0] = b; node[1] = a; }
}

bool Edge::operator < (const Edge& rhs) const{

	int id1a = node[0]->id;
	int id1b = node[1]->id;

	int id2a = rhs.node[0]->id;
	int id2b = rhs.node[1]->id;


	if ( id1a == id2a ) return id1b < id2b;
	if ( id1a == id2b ) return id1b < id2a;
	if ( id1b == id2a ) return id1a < id2b;
	if ( id1b == id2b ) return id1a < id2a;
    return true;
}
Node * Edge::getNeighbor(Node *n)
{
	if ( node[0] == n ) return node[1];
	if ( node[1] == n ) return node[0];

	return 0;	
}


Node::Node(const int& i,const int& l_x,const int& l_y,const int& r_x,const int& r_y)
{
	id = i;
	traveled = false;
    color = -1;// not colored
	prev = 0;
	lb_x=l_x;
	lb_y=l_y;
	ru_x=r_x;
	ru_y=r_y;

}

void Node::addEdge(Edge *e)
{
	edge.push_back(e);
}

bool edgeComp( const Edge* A, const Edge* B ){
	if ( (*A) < (*B) ) return true;
	return false;
}


void Node::sortEdge()
{
    sort(edge.begin(), edge.end(), edgeComp);
}

bool Node::overlap(Node* n,const int & alpha,const int & beta)// from left to right 
{
	int check_lb_y=lb_y-beta;
	int check_ru_x=ru_x+alpha;
	int check_ru_y=ru_y+beta;
	//cout<<id<<" calling overlap on "<<n->id<<endl;

	if ((n->lb_x>=lb_x)&&(n->lb_x<=check_ru_x))
	{
		//cout<<"Node "<<id<<" and node "<<n->id<<" enter  "<<endl;
	   if (((n->lb_y>=check_lb_y)&&(n->lb_y<=check_ru_y))//left bottom inside
		   ||((n->ru_y>=check_lb_y)&&(n->ru_y<=check_ru_y)))//left upper inside
	   {
	   	  //cout<<"overlaps "<<endl;
		   if ((n->lb_x>=ru_x)&&((n->lb_y>=ru_y)||(n->ru_y<=lb_y)))
		   {
		   		//cout<<"invalid !!"<<endl;
				return false;
		   }

		   return true;
	   }
	   else if ((n->lb_y<=check_lb_y)&&(n->ru_y>=check_ru_y))// be included
	   	return true;
	}

	return false;

}

int Node::areaCalc()
{
	return (ru_x-lb_x)*(ru_y-lb_y);
}


void Graph::constructAll()
{
	//TODO not group yet!
	//TODO not removed redundant edges yet!!
	///first construct conflict edge
	// first along with x-dir, equal with id because the contest input format
	//sortNodesByID();
	sortNodesByX();

	unsigned int j=0;
	for (unsigned int i=0;i<nodes.size();++i)
	{
		j=i+1;
		
		for (;j<nodes.size();++j)
		{
			//cout<<i<<" calling "<<j<<endl;
			if((nodes[j]->lb_x)>nodes[i]->ru_x+alpha)
			{
				break;
			}
			if (nodes[i]->overlap(nodes[j],alpha,beta))
		    {
			   Edge *e = new Edge(nodes[i], nodes[j]);
	           edges.push_back(e);

	           nodes[i]->edge.push_back(e);
	           nodes[j]->edge.push_back(e);
	        }
	        // else if ((nodes[j]->lb_x)>nodes[i]->ru_x+alpha)
	        // 	break;
		}
	}

}

void Graph::printall()//just for debug
{
	cout << endl << "Shape area :" << endl << endl;
	map<int, Node *>::iterator itN;
	for ( itN = nodesMap.begin() ; itN != nodesMap.end() ; itN++ ) {
		 cout << (*itN).second -> id << ": " << (*itN).second -> areaCalc() << endl;
	}

	cout << endl << "Edges :" << endl << endl;
	for (unsigned int i = 0; i < edges.size(); ++i) {
		cout << "edge " << i << " is " << edges[i] -> node[0] -> id << " -- " << edges[i] -> node[1] -> id << endl;
	}

	cout << endl << "Groups of the graph: " << endl << endl;
	for(unsigned int i = 0; i < groups.size(); ++i) {
		if(groups[i] != NULL) { groups[i] -> printGroup(); }
	}
}

Graph::Graph(const string& n,const int& alp,const int& be,const int& ome)
{
	name = n;
	alpha = alp;
	beta = be;
	omega = ome;
	colored_graph=0;
	B_top=numeric_limits<int>::min();
	B_bottom=numeric_limits<int>::max();
	B_right=numeric_limits<int>::min();
	B_left=numeric_limits<int>::max();
}

Graph::~Graph()
{
	vector<Edge *>::iterator itE;
	for ( itE = edges.begin() ; itE != edges.end() ; itE++ )
	{
		if (*itE!=NULL)
		{
			delete (*itE);
		    (*itE) = 0;
		}
	}
	
	map<int, Node *>::iterator itN;
	for ( itN = nodesMap.begin() ; itN != nodesMap.end() ; itN++ )
	{
		if ((*itN).second!=NULL)
		{delete (*itN).second;
		(*itN).second = 0;}
	}

    vector<Node *>::iterator itN2;
	for ( itN2 = nodes.begin() ; itN2 != nodes.end() ; itN2++ )
	{
		(*itN2) = 0;
	}
}

void Graph::addNode( Node* nptr)
{
	nodesMap[nptr->id]=nptr;
	nodes.push_back(nptr);
}
	
void Graph::sortEdgesOfNode()
{
	map<int, Node *>::iterator it;
	for ( it = nodesMap.begin() ; it != nodesMap.end() ; it++ )
	{
		Node *node = (*it).second;
		node->sortEdge();
	}

}

bool NodeCompByD( const Node* A, const Node* B ){
	if ( A->edge.size() > B->edge.size() ) 
        return true;
    else if (A->edge.size() == B->edge.size()) {
        if (A->id < B->id) 
            return true;
        else 
            return false;
    }
    else
        return false;
}

void Graph::sortNodesByDegree()
{
    sort(nodes.begin(), nodes.end(), NodeCompByD);
}


bool NodeCompByID( const Node* A, const Node* B ){
	if ( A->id < B->id ) return true;
    else return false;
}

void Graph::sortNodesByID()
{
    sort(nodes.begin(), nodes.end(), NodeCompByID);
}

bool NodeCompByX(const Node* A, const Node* B ) {
	if(A->lb_x < B->lb_x) return true;
	else return false;
}

void Graph::sortNodesByX()
{
	sort(nodes.begin(), nodes.end(), NodeCompByX);
}

bool NodeCompByY(const Node* A, const Node* B ) {
	if(A->lb_y < B->lb_y) return true;
	else return false;
}

void Graph::sortNodesByY()
{
	sort(nodes.begin(), nodes.end(), NodeCompByY);
}

void Graph::init()
{
	map<int, Node *>::iterator itN;
	for ( itN = nodesMap.begin() ; itN != nodesMap.end() ; itN++ )
	{
		Node *node = (*itN).second;
		node->traveled = false;
		//node->d = DIS_INF;
		node->prev = 0;
        node->color = -1;//node is white
	}
	
}


Node * Graph::getNodeById(const int& id)
{
	return nodesMap[id];
}

void Graph::dfs()
{
   init();
   int edgenum=0, group_num = 0;
   sortEdgesOfNode();
   sortNodesByID();
   for(unsigned int i=0;i<nodes.size();++i) {
   	   if(!(nodes[i] -> traveled)) {
   	   	   Group* group = new Group;
           dfsvisit(nodes[i], group_num, group);
           groups.push_back(group);
           ++group_num;
       }
   }
}

void Graph::dfsvisit(Node* u, int group_num, Group* group)
{
 	u -> group_id = group_num;
	u->traveled = true;
	group -> groupNodes.push_back(u);

	for(unsigned int i = 0; i < u -> edge.size(); ++i) {
		Node* near = u -> edge[i] -> getNeighbor(u);
		if (near->traveled == false) {
			dfsvisit(near, group_num, group);		
		}
	}
}

Group::~Group() {
	for(unsigned int i = 0; i < groupNodes.size(); ++i) {
		if(groupNodes[i] != NULL) {
			delete groupNodes[i];
			groupNodes[i] = NULL;
		}
	}
}

void
Group::printGroup() {
	for(unsigned int i = 0; i < groupNodes.size(); ++i) {
		cout << "group " << groupNodes[i] -> group_id << ": shape " << groupNodes[i] -> id << endl;
	}
	cout << endl;
}
void
Group::calcgrouparea()
{
	for(size_t i=0;i<groupNodes.size();i++)
	{
		grparea+=groupNodes[i]->area;
	}
}

void Graph::colorgraph()
{
	colored_graph=0;
	init();
for(unsigned int i=0;i<groups.size();i++)
{
   	Node* sourcenode=groups[i]->groupNodes[0];
	queue<Node*> nodesque;
	sourcenode->color=0;
	sortEdgesOfNode();
	nodesque.push(sourcenode);
	//int edgenum=0;
    bool stop=false;
	while (nodesque.empty()==false&&stop==false)
	{
		Node * u=nodesque.front();
		if(0==u->color)
		{
		for(unsigned int k=0;k<u->edge.size();++k)
		{
			Node* near= u->edge[k]->getNeighbor(u);
			if (-1==near->color)
			{
				near->color=1;
				nodesque.push(near);
				//++edgenum;
			}
			else if(0==near->color)
			{
				//cout<<"inininin"<<i<<endl;
				 Group* temp=groups[i];
      			groups[i]=groups[colored_graph];
      			groups[colored_graph]=temp;
      			colored_graph++;
      			stop=true;
      			break;
			}
		}
	    }
	    else if(1==u->color)
	    {
	    	for(unsigned int k=0;k<u->edge.size();++k)
		{
			Node* near= u->edge[k]->getNeighbor(u);
			if (-1==near->color)
			{
				near->color=0;
				nodesque.push(near);
				//++edgenum;
			}
			else if(1==near->color)
			{//cout<<"inininin"<<i<<endl;
				 Group* temp=groups[i];
      			groups[i]=groups[colored_graph];
      			groups[colored_graph]=temp;
      			colored_graph++;
      			stop=true;
      			break;
			}
		}
	    }
		nodesque.pop();
	}
}
   	
   
}

void Graph::getboxsize()
{
  for(unsigned int i=colored_graph;i<groups.size();i++)
{
   for(unsigned int j=0;j<groups[i]->groupNodes.size();j++)
   {
     //lb_x,lb_y,ru_x,ru_y
     if(groups[i]->groupNodes[j]->ru_y>B_top)
     	B_top=groups[i]->groupNodes[j]->ru_y;
     if(groups[i]->groupNodes[j]->lb_y<B_bottom)
      B_bottom=groups[i]->groupNodes[j]->lb_y;
    if(groups[i]->groupNodes[j]->ru_x>B_right)
      B_right=groups[i]->groupNodes[j]->ru_x;
    if(groups[i]->groupNodes[j]->lb_x<B_left)
      B_left=groups[i]->groupNodes[j]->lb_x;
   }
}
}
void Graph::windowcalc_grparea() 
{
	for(unsigned int i=0;i<windows.size();i++)
  {
  	for (unsigned int j = 0; j < windows[i].size(); ++j)
  	{
  		vector< pair<int,int> > v;
  		//vector<int> v;
  		//cout<<"windowNodes:";
  		for(unsigned int k=0;k<windows[i][j]->windowNodes.size();k++)
  		{	
  			//cout<<windows[i][j]->windowNodes[k]->id;
  			pair <int,int> group(k,windows[i][j]->windowNodes[k]->group_id);
            v.push_back(group);
  			//v.push_back(windows[i][j]->windowNodes[k]->group_id);
  		}
  		//cout<<endl;
  		if(v.size()>0)
  		{
  			sort(v.begin(),v.end(),compare);
  			windows[i][j]->wgroups.push_back(v[0].second);
  			int color=(0==windows[i][j]->windowNodes[v[0].first]->color) ?1:-1;
  			windows[i][j]->wgroupsarea.push_back(windows[i][j]->area[v[0].first]*color);
        for(unsigned int k=1;k<v.size();k++)
  			{
  				if(v[k].second!=v[k-1].second)
  				{
  				windows[i][j]->wgroups.push_back(v[k].second);
  				windows[i][j]->wgroupsarea.push_back(0);
  			    }
  			    int color=(0==windows[i][j]->windowNodes[v[k].first]->color) ?1:-1;
  			    windows[i][j]->wgroupsarea[windows[i][j]->wgroupsarea.size()-1]+=windows[i][j]->area[v[k].first]*color;
  			}
  		}

  	}
  }
}
void Graph::greedywindow()
{
	vector<Window*> sortbygrp;
  for(unsigned int i=0;i<windows.size();i++)
  {
  	for (unsigned int j = 0; j < windows[i].size(); ++j)
  	{
  		sortbygrp.push_back(windows[i][j]);
  	}
  }
  sort(sortbygrp.begin(),sortbygrp.end(),wincompare);
  for(unsigned int i=0;i<sortbygrp.size();i++)
  {
  	ptaveragecolor(sortbygrp[i]);
  }
}
void Graph::greedycolor()
{
	
  for(unsigned int i=0;i<windows.size();i++)
  {
  	for (unsigned int j = 0; j < windows[i].size(); ++j)
  	{
  		averagecolor(i,j);
  	}
  }
  
}
void Graph::ptaveragecolor(Window* win)
{
	vector<pair <int,int> > v;
	int avgcolor=0;
    for(unsigned int i=0;i<win->wgroups.size();i++)
    {
    	pair <int,int> area (i,win->wgroupsarea[i]);
    	if(groups[win->wgroups[i]]->locked)
    	{
    		int inv= (!groups[win->wgroups[i]]->invert) ?1:-1;
       		avgcolor+=win->wgroupsarea[i]*inv;
       	}
        v.push_back(area);
    }
    sort(v.begin(),v.end(),compare);
	
	for(int i=v.size()-1;i>=0;i--)
       {
       	
       	int id=win->wgroups[v[i].first];
         if(!groups[id]->locked)
         {
         	
          	if(avgcolor<=0)
           	{

          		if(win->wgroupsarea[v[i].first]<0)
           			groups[id]->invert=true;
           		avgcolor+=win->wgroupsarea[v[i].first];
           	}
           else
           {
           		if(win->wgroupsarea[v[i].first]>=0)
           	groups[id]->invert=true;
           	avgcolor-=win->wgroupsarea[v[i].first];
           }
          groups[id]->locked=true;
         }         
       }
}
void Graph::averagecolor(int x,int y)
{
	vector<pair <int,int> > v;
	int avgcolor=0;
    for(unsigned int i=0;i<windows[x][y]->wgroups.size();i++)
    {
    	pair <int,int> area (i,windows[x][y]->wgroupsarea[i]);
    	if(groups[windows[x][y]->wgroups[i]]->locked)
    	{
    		int inv= (!groups[windows[x][y]->wgroups[i]]->invert) ?1:-1;
       		avgcolor+=windows[x][y]->wgroupsarea[i]*inv;
       	}
        v.push_back(area);
    }
    sort(v.begin(),v.end(),compare);
	
	for(int i=v.size()-1;i>=0;i--)
       {
       	
       	int id=windows[x][y]->wgroups[v[i].first];
         if(!groups[id]->locked)
         {
         	
          	if(avgcolor<=0)
          	{
          		if(windows[x][y]->wgroupsarea[v[i].first]<0)
           			groups[id]->invert=true;
           		avgcolor+=windows[x][y]->wgroupsarea[v[i].first];	
            }
           else
           {
           	if(windows[x][y]->wgroupsarea[v[i].first]>=0)
           	groups[id]->invert=true;
           	avgcolor-=windows[x][y]->wgroupsarea[v[i].first];
           }
          groups[id]->locked=true;
         }         
       }
}

void Graph::largegreedy()
{
	for(size_t i=0;i<scoreMap.size();i++)
	{
		for(size_t j=0;j<scoreMap[0].size();j++)
		{
			if(scoreMap[i][j]!=0)
			{
				groups[j]->grpwin++;
			}
		}
	}

	 vector<pair <Group*,int> > largegrps; 
	for(size_t i=0;i<groups.size();i++)
	{
		groups[i]->calcgrouparea();
		pair <Group*,int> area (groups[i],groups[i]->grpwin);
		largegrps.push_back(area);
	}
    sort(largegrps.begin(),largegrps.end(),grpcompare);
    for(int i=largegrps.size()-1;i>=0;i--)
    {
    	largecolor(largegrps[i].first);
    }

}
void Graph::largecolor(Group* grp)
{
	int score1=0,score2=0;
	for(size_t i=0;i<scoreMap.size();i++)
	{
		if(scoreMap[i][grp->groupNodes[0]->group_id]!=0)
		{
			score1+=windows[i%windows.size()][i/windows.size()]->colorarea;
			score2+=windows[i%windows.size()][i/windows.size()]->colorarea;
		}
		score1+=scoreMap[i][grp->groupNodes[0]->group_id];
		score2-=scoreMap[i][grp->groupNodes[0]->group_id];
	}
	if(abs(score1)<=abs(score2))
	{
		for(size_t i=0;i<scoreMap.size();i++)
		{
			windows[i%windows.size()][i/windows.size()]->colorarea+=scoreMap[i][grp->groupNodes[0]->group_id];
		}
		grp->locked=true;
	}
	else
	{
		grp->invert=true;
		for(size_t i=0;i<scoreMap.size();i++)
		{
			windows[i%windows.size()][i/windows.size()]->colorarea-=scoreMap[i][grp->groupNodes[0]->group_id];
		}
		grp->locked=true;
	}	
}
void Graph::reset()
{
	for(size_t i=0;i<groups.size();i++)
	{
		groups[i]->locked=false;
		groups[i]->invert=false;
	}
}
//bear's code

void Graph::mapping(){

	//int omega=omega;
	int B_length=B_right-B_left;
	int B_width=B_top-B_bottom;
	//initialize windows
//debug variable
	//int count=0;
//
	for(int i=0;i<ceil(static_cast<double>(B_length)/omega);i++){
		vector<Window*> a;
		for(int j=0;j<ceil(static_cast<double>(B_width)/omega);j++){
			Window *w=new Window;
			a.push_back(w);	
/*
//debug
			count++;
			cout<<"countL"<<count<<endl;
//debug over
*/
		}	
		windows.push_back(a);

	}

#ifdef DEBUG
	//debug
	for(unsigned int i=0;i<groups.size();i++){
	cout<<"GROUP["<<i<<"]"<<endl;
	cout<<endl;
		for(int j=0;j<groups[i]->groupNodes.size();j++){
			cout<<"node id:"<<groups[i]->groupNodes[j]->id<<endl;
			cout<<"group_id:"<<groups[i]->groupNodes[j]->group_id<<endl;
			cout<<"node color:"<<groups[i]->groupNodes[j]->color<<endl;
			cout<<endl;
		}	
	}

	cout<<"top"<<B_top<<",bottom:"<<B_bottom<<",left"<<B_left<<",right"<<B_right<<endl;	
	cout<<"colored_graph:"<<colored_graph<<endl;
#endif

	//debug over	
	for(unsigned int i=colored_graph;i<groups.size();i++){  
		
		for(unsigned int j=0;j<groups[i]->groupNodes.size();j++){
//debug
//count++;
//cout<<"node pass:"<<count<<endl;
//debug over
			//place shape in given window
			int x1=groups[i]->groupNodes[j]->lb_x;
			int x2=groups[i]->groupNodes[j]->ru_x;
			int y1=groups[i]->groupNodes[j]->lb_y;
			int y2=groups[i]->groupNodes[j]->ru_y;
			
			int x1_sec=(x1-B_left)/omega;//x1,x2,y1,y2 in which DSW
			
			int y1_sec=(y1-B_bottom)/omega;
			
			vector <int> x;			
			vector <int> y;
			//x component
			int k=1;
			x.push_back(x2-x1);//x[0]
			int x_tail=x1;
			while(x2>omega*(k+x1_sec)+B_left){
				x[k-1]=(x1_sec+k)*omega+B_left-x_tail;
				x.push_back(x2-(x1_sec+k)*omega-B_left);
				x_tail=omega*(x1_sec+k)+B_left;
				k++;
			}
			//y component
			k=1;
			y.push_back(y2-y1);//y[0]
			int y_tail=y1;
			while(y2>omega*(k+y1_sec)+B_bottom){
				y[k-1]=(y1_sec+k)*omega+B_bottom-y_tail;
				y.push_back(y2-(y1_sec+k)*omega-B_bottom);
				y_tail=omega*(y1_sec+k)+B_bottom;
				k++;
			}
			//last block overlapping
			if(B_length%omega!=0){
				if(x2>B_right-omega){
					if(x2>((B_right-B_left)/omega)*omega+B_left){//(right-left)/omega= number of x's DW -1
						if(x1<B_right-omega){//see if x1 smaller than boundary
 							x[x.size()-1]=x2+omega-B_right;
						}else{
							x[x.size()-1]=x2-x1;
						}
					}else{
						if(x1<B_right-omega){
							x.push_back(x2+omega-B_right);
						}else{
							x.push_back(x2-x1);
						}
					}
				}
			}
			if(B_width%omega!=0){
				if(y2>B_top-omega){
					if(y2>((B_top-B_bottom)/omega)*omega+B_bottom){//(y2-y1)/omega= number of y's DW -1
						if(y1<B_top-omega){
 							y[y.size()-1]=y2+omega-B_top;
						}else{
							y[y.size()-1]=y2-y1;
						}
					}else{
						if(y1<B_top-omega){
							y.push_back(y2+omega-B_top);
						}else{
							y.push_back(y2-y1);
						}
					}
				}
			}
			// fill in map
//int cc=0;
			for(unsigned int p=0;p<x.size();p++){
				for(unsigned int q=0;q<y.size();q++){
//debug
//cc++;
//cout<<"cc:"<<cc<<endl;
//debug over
				
					windows[x1_sec+p][y1_sec+q]->windowNodes.push_back(groups[i]->groupNodes[j]);//variable check
					windows[x1_sec+p][y1_sec+q]->area.push_back(x[p]*y[q]);
				}			
			}				
		}
	}

//the following use for debug
#ifdef DEBUG
	
	for(int i=0;i<ceil(static_cast<double>(B_length)/omega);i++){//x
		for(int j=0;j<ceil(static_cast<double>(B_width)/omega);j++){//y
			cout<<"Window["<<i<<"]["<<j<<"]"<<endl;
			for(unsigned int k=0;k<windows[i][j]->windowNodes.size();k++){
				cout<<"node id:"<<windows[i][j]->windowNodes[k]->id<<endl;
				cout<<"group_id:"<<windows[i][j]->windowNodes[k]->group_id<<endl;
				cout<<"node color:"<<windows[i][j]->windowNodes[k]->color<<endl;
				cout<<"area:"<<windows[i][j]->area[k]<<endl;
				cout<<endl;
			}			
		}
	}
	cout<<"top"<<B_top<<",bottom:"<<B_bottom<<",left"<<B_left<<",right"<<B_right<<endl;	
	cout<<"colored_graph:"<<colored_graph<<endl;
#endif
//debug over

}

void Graph::output(ofstream &fout ){
	
	int count=0;
	int B_length=B_right-B_left;
	int B_width=B_top-B_bottom;
	
	//for En Yeu debug cout
	//fout<<"BOX coord ="<<B_left<<","<<B_bottom<<","<<B_right<<","<<B_top<<endl;
	
	for(int j=0;j<ceil(static_cast<double>(B_width)/omega);j++){//x

		for(int i=0;i<ceil(static_cast<double>(B_length)/omega);i++){//y
			int red_area=0;
			int blue_area=0;
			for(unsigned int k=0;k<windows[i][j]->windowNodes.size();k++){
				if(windows[i][j]->windowNodes[k]->color==1){
					if(groups[windows[i][j]->windowNodes[k]->group_id]->invert==0){
						red_area+=windows[i][j]->area[k];
					}else{//invert:red -> blue
						blue_area+=windows[i][j]->area[k];
					}
				}else if(windows[i][j]->windowNodes[k]->color==0){
					if(groups[windows[i][j]->windowNodes[k]->group_id]->invert==0){
						blue_area+=windows[i][j]->area[k];
					}else{//invert:blue -> red
						red_area+=windows[i][j]->area[k];
					}
				}
						
			}
			

			int D_left=B_left+i*omega;
			int D_right=D_left+omega;
			int D_down=B_bottom+j*omega;
			int D_up=D_down+omega;
			//x most right
			if(D_right>B_right){	
				D_right=B_right;
				D_left=B_right-omega;
			}	
			//y most up
			if(D_up>B_top){
				D_up=B_top;
				D_down=B_top-omega;
			}
			count++;
			fout<<"WIN["<<count<<"]="<<D_left<<","<<D_down<<","<<D_right<<","<<D_up<<"("<<float(red_area*100)/float(omega*omega)<<" "<<float(blue_area*100)/float(omega*omega)<<")"<<endl;
		}
	}
	// not colored graph
	for(int i=0;i<colored_graph;i++){
		fout<<"GROUP"<<endl;
		for(unsigned int j=0;j<groups[i]->groupNodes.size();j++){
			fout<<"NO["<<j+1<<"]="<<groups[i]->groupNodes[j]->lb_x<<","<<groups[i]->groupNodes[j]->lb_y<<","<<groups[i]->groupNodes[j]->ru_x<<","<<groups[i]->groupNodes[j]->ru_y<<endl;
		}
	}
	//colored graph
	for(unsigned int i=colored_graph;i<groups.size();i++){
		fout<<"GROUP"<<endl;
		vector <int> red;
		vector <int> blue;
		for(unsigned int j=0;j<groups[i]->groupNodes.size();j++){
			if (groups[i]->invert==false)
			{
			   if(groups[i]->groupNodes[j]->color==1){ 
				   red.push_back(j);
			   }
			   else{
				blue.push_back(j);
			   }
			}
			else{
				if(groups[i]->groupNodes[j]->color==1){ 
				   blue.push_back(j);
			   }
			   else{
				red.push_back(j);
			   }
			}		
		}
		for(unsigned int j=0;j<red.size();j++){
			fout<<"CA["<<j+1<<"]="<<groups[i]->groupNodes[red[j]]->lb_x<<","<<groups[i]->groupNodes[red[j]]->lb_y<<","<<groups[i]->groupNodes[red[j]]->ru_x<<","<<groups[i]->groupNodes[red[j]]->ru_y<<endl;
		}
		for(unsigned int j=0;j<blue.size();j++){
			fout<<"CB["<<j+1<<"]="<<groups[i]->groupNodes[blue[j]]->lb_x<<","<<groups[i]->groupNodes[blue[j]]->lb_y<<","<<groups[i]->groupNodes[blue[j]]->ru_x<<","<<groups[i]->groupNodes[blue[j]]->ru_y<<endl;
		}
	}
	
}

float Graph::getscore()
{
	float score=0;
	float y=windows.size()*windows[0].size();
	float x=70/y;	
	for(unsigned int i=0;i<windows.size();i++)
	{
		for(unsigned int j=0;j<windows[i].size();j++)
		{
			float winscore=0;
			for(unsigned int k=0;k<windows[i][j]->wgroupsarea.size();k++)
			{
				int inv= (groups[windows[i][j]->wgroups[k]]->invert)?-1:1;
			  winscore+=windows[i][j]->wgroupsarea[k]*inv;

			}
			winscore=fabs(winscore);
			 //cout<<winscore/5/omega/omega*100<<endl;
            if(x-winscore/5/omega/omega*100>0)
			score+=x-winscore/5/omega/omega*100;
			 // *100 is to become percent
            
		}
	}
	return score;
}

void Graph::saveState()
{	lastState.clear();
	for (unsigned int j=colored_graph;j<groups.size();++j)
	{
		if (groups[j]->invert)
			lastState.push_back(true);
		else
			lastState.push_back(false);
	}
}

void Graph::recallState()
{
	for (unsigned int j=colored_graph;j<groups.size();++j)
	{
		if (lastState[j]==true)
			set(j,true);
		else
			set(j,false);
	}
}

void Graph::gacolor()
{
	int M_rows=groups.size()*100;
	int max_iter=groups.size()*200;
	float gamma=0.9;
	//float crossRate=0.05;


	CommonNs::TmUsage tmusg;
    CommonNs::TmStat stat;
    tmusg.totalStart();

	srand(time(NULL));

	double z=((double) rand() / (RAND_MAX));

	//cout<<"z is "<<z<<endl;

	Matrix* M_ptr=new Matrix(M_rows,groups.size()-colored_graph);

	//add greedy result and corner cases
	 for (unsigned int i = colored_graph; i <groups.size() ; ++i)
	 {
	 	if (groups[i]->invert)
	 		M_ptr->data[0][i-colored_graph]=1;
	 	else
	 		M_ptr->data[0][i-colored_graph]=0;
	 }

	for (int i = 0; i < M_rows; ++i)
	{
		M_ptr->setRowscore(i,gaRowgetScore(M_ptr->data[i]));
	}

    //cout<<"First time "<<endl;
	//M_ptr->print();

	M_ptr->sortRowByScore();

	while (max_iter!=0)
	{
		--max_iter;
		if (z<gamma)
		   M_ptr->mogaR();
		else
		  M_ptr->mogaC();

		//M_ptr->crossOver(crossRate);

		for (int i = 0; i < M_rows; ++i)
	    {
		   M_ptr->setRowscore(i,gaRowgetScore(M_ptr->data[i]));
	    }

	    M_ptr->sortRowByScore(); 

	    tmusg.getTotalUsage(stat);

	  //  cout<<"time using "<<(stat.uTime + stat.sTime) / 1000000.0<<" s"<<endl;
	  //  cout<<"Highest Score is "<<M_ptr->rowsVec[0]->row_score<<endl;


	    if (((stat.uTime + stat.sTime) / 1000000.0)>3400)
	    	break;
	}

	int theRow = M_ptr->rowsVec[0]->row_id; 
	for (unsigned int j=colored_graph;j<groups.size();++j)
	{
		if (M_ptr->data[theRow][j-colored_graph]==0)
			set(j,false);
		else
			set(j,true);
	}

}

float Graph::gaRowgetScore(int * row_ptr)
{
	for (unsigned int i=colored_graph;i<groups.size();++i)
	{
		if (row_ptr[i-colored_graph]==0)
			set(i,false);
		else
			set(i,true);
	}
	return getscore();
}


bool Graph::exhausted()
{
	double all=pow(2,groups.size()-colored_graph);
	if (all*windows.size()*windows[0].size()>1000000000)
		return false;

	float best_score=0.0;
	string best_assign;
	bitset<256> best;

	for (int i = 0; i < all; ++i)
	{
		bitset<256> qqq(i);
	    string binary = qqq.to_string(); //to binary
        //cout<<binary<<"\n";
        for (int j=qqq.size()-1;j>=0;--j)
        {
        	//cout<<" exhausting "<<endl;
        	if (j>=((static_cast<int>(groups.size())-colored_graph)))
        		{
        			continue;
        		}
        	else {
        	if(qqq[j])
        	{
        		set(j+colored_graph,true);
            }
        	else
        	{       	
        		set(j+colored_graph,false);
        	}
            }
        	
        }
        float now_score=getscore();
        if (now_score>best_score)
        {
        	best_score=now_score;
        	best_assign=binary;
        	best=qqq;
        }
	}

	for (int j=best.size()-1;j>=0;--j)
	{
		if (j>=((static_cast<int>(groups.size())-colored_graph)))
        		continue;
		else{
		if(best[j])      
        	set(j+colored_graph,true);
        else
        	set(j+colored_graph,false);
        }     
	}

	return true;
}

void Graph::getScoreMap(){
	//intialize scoreMap;
	
	for(unsigned int j=0;j<windows[0].size();j++){
		for(unsigned int i=0;i<windows.size();i++){
			vector <int> a;
			for(unsigned int k=0;k<groups.size();k++){
				a.push_back(0);
			}
			scoreMap.push_back(a);
		}
	}
	//intialize over
	for(unsigned int j=0;j<windows[0].size();j++){
		for(unsigned int i=0;i<windows.size();i++){
			for (unsigned k=0;k<windows[i][j]->windowNodes.size();k++){
				if(windows[i][j]->windowNodes[k]->color==0){//color 0 area+=area
					(scoreMap[i+j*windows.size()][windows[i][j]->windowNodes[k]->group_id])+=windows[i][j]->area[k];
				}else if(windows[i][j]->windowNodes[k]->color==1){//color 1 area-=area
					(scoreMap[i+j*windows.size()][windows[i][j]->windowNodes[k]->group_id])-=windows[i][j]->area[k];
				}
			}
		}	
	}
	

}


