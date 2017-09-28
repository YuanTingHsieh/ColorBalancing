// **************************************************************************
//  File       [main.cpp]
//  Author     [Yuan-Ting Hsieh]
//  Synopsis   [The main program of 2015 CAD contest problem 5]
//  Modify     [2015/05/30 Yuan-Ting Hsieh]
// **************************************************************************

#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "graph.h"
#include "tm_usage.h"

#define _DEBUG

using namespace std;

void help_message() {
    cout << "usage: ./DPT_balance_color <input_file> <output_file>" << endl;
}

int main(int argc, char* argv[])
{
    if(argc != 3) {
       help_message();
       return 0;
    }

    CommonNs::TmUsage tmusg;
    CommonNs::TmStat stat;
    tmusg.totalStart();

    //////////// read the input file /////////////
    ifstream input_file;
    input_file.open(argv[1]);
    if (!input_file.is_open())
    {
      cerr<<"Can not open file "<<argv[1]<<" !!"<<endl;
      return -1;
    }

    int alpha,beta,omega;
    char buf [256];

    //get alpha,beta,omega
    for (int i=0;i<3;++i)
    {
       input_file.getline(buf,256);
       string bb(buf);
       size_t g=bb.find_first_of('=');
    
       if (i==0)
          alpha=atoi(bb.substr(g+1).c_str());
       else if (i==1)
          beta=atoi(bb.substr(g+1).c_str());
       else 
          omega=atoi(bb.substr(g+1).c_str());
    }

#ifdef DEBUG
    cout << endl;
    cout << "alpha = " << alpha << endl
         << "beta  = " << beta  << endl
         << "omega = " << omega << endl;
#endif

    Graph TGI("Hsieh",alpha,beta,omega);

    int shapenum=0;
    int a,b,c,d;
    while(input_file.getline(buf,256))
    {
        
        size_t start=0;
        size_t end=0;
        
        ++shapenum;
        //cout<<"shapenum is "<<shapenum<<endl;
        
        string ss(buf);

        for (int i=0;i<4;++i)
        { 
           start=end;
           end=ss.find_first_of(',',start);
           if (i==0)
            {
                a=atoi(ss.substr(start,end-start).c_str()); 
                
            }
           else if (i==1)
            b=atoi(ss.substr(start,end-start).c_str());
           else if (i==2)
            c=atoi(ss.substr(start,end-start).c_str());
           else
            d=atoi(ss.substr(start,end-start).c_str());
          ++end;
        }

        Node* n=new Node(shapenum,a,b,c,d);
        TGI.addNode(n);
        //cout<<a<<" "<<b<<" "<<c<<" "<<d<<endl;
    }

    //////////// find the solution to pick up the food ////
    tmusg.periodStart();


    TGI.constructAll();
    TGI.dfs();
    TGI.colorgraph();


#ifdef DEBUG
    TGI.printall();
    for(int i=0;i<TGI.groups.size();i++)
    {
      for(int j=0;j<TGI.groups[i]->groupNodes.size();j++)
      {
        cout<<TGI.groups[i]->groupNodes[j]->color<<","<<TGI.groups[i]->groupNodes[j]->id<<endl;
      }
       cout<<endl;
    }
#endif
    
    TGI.getboxsize();
    //cout<<"pass getboxsize"<<endl;
    TGI.mapping();
    //cout<<"pass mapping"<<endl;

    TGI.windowcalc_grparea();//only after this and color can you get correct score


    //double score = TGI.getscore();

    //cout<<"Initial score is:"<<score<<endl; 
    //cout<<"groups are "<<TGI.groups.size()<<endl;
    //float y=TGI.windows.size()*TGI.windows[0].size();
    //cout<<"window  is "<<y<<endl;

    TGI.getScoreMap();

    if (TGI.exhausted())
    {
      //cout<<"enter exhausted "<<endl;
      double score = TGI.getscore();
      cout<<"The exhausted score is:"<<score<<endl;
    }
    else
    {
       TGI.greedycolor();
       double GDscore = TGI.getscore();
        cout<<"oringin:"<<GDscore<<endl;

       TGI.saveState();
       TGI.reset();
       TGI.largegreedy();
       double tempscore = TGI.getscore();
        cout<<"large:"<<tempscore<<endl;
       if(tempscore>GDscore)
       {
        TGI.saveState();
        GDscore=tempscore;
       }
       TGI.reset();
       TGI.greedywindow();
      tempscore = TGI.getscore();
      cout<<"window:"<<tempscore<<endl;

       if(tempscore>GDscore)
       {
        TGI.saveState();
        GDscore=tempscore;
       }
       TGI.recallState();
       TGI.gacolor();
       double score2 = TGI.getscore();
       cout<<"The genetic score is:"<<score2<<endl;
       cout<<"The greedy score is:"<<GDscore<<endl;
       if (GDscore>score2)
         TGI.recallState();
    }
    //cout<<"final score is "<<TGI.getscore()<<endl;

    //////////// write the output file ///////////

    ofstream output_file;
    output_file.open(argv[2]);
    if (!output_file.is_open())
    {
      cerr<<"Can not open output file "<<argv[2]<<" !!"<<endl;
     return -1;
    }
	  TGI.output(output_file);

#ifdef DEBUG
    tmusg.getTotalUsage(stat);

    cout <<"user time: " << stat.uTime / 1000000.0 << "s" << endl; // print period user time in seconds
    cout <<"system time: " << stat.sTime / 1000000.0 << "s" << endl; // print period systemtime in seconds
    cout <<"user+system time:" << (stat.uTime + stat.sTime) / 1000000.0 << "s" << endl;

    cout << stat.vmSize / 1024.0 /1024.0<< "GB" << endl; // print current memory
    cout << stat.vmPeak / 1024.0 /1024.0<< "GB" << endl; // print peak memory 
#endif
    
    return 0;
}

