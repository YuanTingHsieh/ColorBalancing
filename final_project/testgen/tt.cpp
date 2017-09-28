#include<iostream>
#include<fstream>
#include<string>

using namespace std;

int main()(int argc, char* argv[])
{
   ifstream in;
   //string str("test.in");

   in.open(argv[1];
   if  (!in.is_open())
      return -1;
   
   ofstream out;
   out.open(argv[2]);

   char ss[256];
   while(in)
   {  in.getline(ss,256);
      string qq(ss);
      size_t g=qq.find_first_not_of(' ');
      if (g!=string::npos)
      {  out<<qq.substr(g);
         out<<endl;
      }
   }

}
