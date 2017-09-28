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
                //cout<<"fuck"<<ss.substr(start,end-start).c_str()<<"fuck ";
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

#ifdef DEBUG
    TGI.printall();
#endif

    TGI.colorgraph();

#ifdef DEBUG
    for(int i=0;i<TGI.groups.size();i++)
    {
      for(int j=0;j<TGI.groups[i]->groupNodes.size();j++)
      {
        cout<<TGI.groups[i]->groupNodes[j]->color<<","<<TGI.groups[i]->groupNodes[j]->id<<endl;
      }
       cout<<endl;
    }
#endif
    //cout<<"pass color graph"<<endl;
    TGI.getboxsize();
   // cout<<"uncolor:"<<TGI.colored_graph<<endl;
    //cout<<"bound:"<<TGI.B_top<<","<<TGI.B_bottom<<","<<TGI.B_right<<","<<TGI.B_left<<endl;
    //cout<<"pass getboxsize"<<endl;
    TGI.mapping();
    //cout<<"pass mapping"<<endl;

    TGI.windowcalc_grparea();//only after this and color can you get correct score


    //TGI.set(1,true);
    //TGI.set(2);
    //TGI.set(3,true); //set 3 will like the contest format
    //TGI.set(4);
    //if (groups.size())

    double score = TGI.getscore();

//for exp
        ofstream output_file;
   output_file.open(argv[2]);
    if (!output_file.is_open())
    {
      cerr<<"Can not open output file "<<argv[2]<<" !!"<<endl;
     return -1;
    }


    output_file<<"Initial score is:"<<score<<endl;


       
    //else
    output_file<<"groups are "<<TGI.groups.size()<<endl;
    float y=TGI.windows.size()*TGI.windows[0].size();
    output_file<<"window  is "<<y<<endl;

    //if (TGI.exhausted())
    //{
      //cout<<"enter exhausted "<<endl;
      //score = TGI.getscore();
      //output_file<<"The exhausted score is:"<<score<<endl;
    //}

    //else
    //{
       TGI.greedycolor();
       TGI.saveState();
       double GDscore = TGI.getscore();
       

       TGI.gacolor();
       double score2 = TGI.getscore();
       output_file<<"The genetic score is:"<<score2<<endl;
       output_file<<"The greedy score is:"<<GDscore<<endl;
       if (GDscore>score2)
         TGI.recallState();
    //}
    output_file<<"final score is "<<TGI.getscore()<<endl;

    //Graph Ano;
    //Ano=TGI;

    //cout<<Ano.getscore()<<endl;

    


#ifdef DEBUG
    tmusg.getPeriodUsage(stat);

    cout <<"period user time: " << stat.uTime / 1000000.0 << "s" << endl; // print period user time in seconds
    cout <<"period system time: " << stat.sTime / 1000000.0 << "s" << endl; // print period systemtime in seconds
    cout <<"period user+system time:" << (stat.uTime + stat.sTime) / 1000000.0 << "s" << endl;    

    //////////// write the output file ///////////
#endif

//    ofstream output_file;
//    output_file.open(argv[2]);
//    if (!output_file.is_open())
//    {
//      cerr<<"Can not open output file "<<argv[2]<<" !!"<<endl;
//     return -1;
//    }
	  TGI.output(output_file);

//#ifdef DEBUG
    tmusg.getTotalUsage(stat);

    output_file <<"user time: " << stat.uTime / 1000000.0 << "s" << endl; // print period user time in seconds
    output_file <<"system time: " << stat.sTime / 1000000.0 << "s" << endl; // print period systemtime in seconds
    output_file <<"user+system time:" << (stat.uTime + stat.sTime) / 1000000.0 << "s" << endl;

    output_file << stat.vmSize / 1024.0 /1024.0<< "GB" << endl; // print current memory
    output_file << stat.vmPeak / 1024.0 /1024.0<< "GB" << endl; // print peak memory 
//#endif
    
    return 0;
}

