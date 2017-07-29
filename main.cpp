#include<iostream>
#include<fstream>
#include<cstdio>
#include<string>
#include<cstring>
#include<cmath>
#include<cstdlib>
#include<malloc.h>
#include<time.h>
#include<vector>
#include<queue>
#include<string>

using namespace std;

//ignore the zero node

ifstream f_in;
ofstream f_out;

string s;
string notstr=" (),=";

typedef struct Node{
	string name;
	int type; // node type, 1 means input, 2 means output, 3 means wire
	vector<int> out; //fin-out
	int gate; // gate type, 1 means MAJ, 2 means XOR, 3 means AND, 4 means OR, 5 means buffer
	int in[3];
	bool invert[3]; //true means 1, false means invert;
	double vote;
}node;

vector<node> N;
queue<node> Q;
string circuitname;


void BuildNode(string name,int nodetype){ //build node
	node x;
	x.name=name;
	x.type=nodetype;
	x.vote=0;
	x.gate=0;
	N.push_back(x);
}

void InputNode(string statestring,int nodetype){
	string subs;
	string::size_type index;
	getline(f_in,s);
	index=s.find_first_not_of(notstr);
	s=s.substr(index,s.length());
	index=s.find_first_of(notstr);
	subs=s.substr(0,index);
	if(subs==statestring)
	{
		s=s.substr(index+1,s.length());
		while(true)
		{
			index=s.find_first_of(notstr+";");
			subs=s.substr(0,index);
			BuildNode(subs,nodetype);
			//cout<<subs<<endl;
			s=s.substr(index,s.length());
			index=s.find_first_not_of(notstr+";");
			if(index==string::npos)
				break;
			s=s.substr(index,s.length());
		}
		while(s[s.length()-1]!=';')
		{
			getline(f_in,s);
			index=s.find_first_not_of(notstr);
			s=s.substr(index,s.length());
			while(true)
			{
				index=s.find_first_of(notstr+";");
				subs=s.substr(0,index);
				BuildNode(subs,nodetype);
				cout<<subs<<endl;
				s=s.substr(index,s.length());
				index=s.find_first_not_of(notstr+";");
				if(index==string::npos)
				break;
				s=s.substr(index,s.length());	
			}
		}
	}
}

int FindNode(string name){
	int loc=-1;
	for(int i=0;i<N.size();i++)
		if(N[i].name==name)
			{loc=i;break;}
	return loc;
}

void ExpandNode(int f,string s){
	string subs;
	string::size_type index;
	int in=0;
	string input[3];
	int gate=-1;
	index=s.find_first_of(notstr);
	subs=s.substr(0,index);
	s=s.substr(index,s.length());
	index=s.find_first_not_of(notstr);
	if(s[index]==';')
	{
		N[f].gate=5;
		if(subs[0]=='~')
		{
			N[f].invert[0]=false;
			subs=subs.substr(1,subs.length());
		}
		else
			N[f].invert[0]=true;
		if(subs[subs.length()-1]==';')
			subs=subs.substr(0,subs.length()-1);
		int sonloc=FindNode(subs);
		N[f].in[0]=sonloc;
		N[sonloc].out.push_back(f);
		return;
	}
	input[0]=subs;
	in++;
	if(s[index]=='&')
		gate=3;
	else if(s[index]=='|')
		gate=4;
	else if(s[index]=='^')
		gate=2;
	N[f].gate=gate;
	s=s.substr(index+1,s.length());
	index=s.find_first_not_of(notstr);
	s=s.substr(index,s.length());
	index=s.find_first_of(notstr+";");
	subs=s.substr(0,index);
	s=s.substr(index,s.length());
	input[1]=subs;
	index=s.find_first_not_of(notstr);
	in++;
	while(s[index]!=';')
	{
		s=s.substr(index,s.length());
		index=s.find_first_of(notstr+";");
		subs=s.substr(0,index);
		if(subs!="|"&&subs!="&")
		{
			if(subs!=input[0]&&subs!=input[1])
				{input[2]=subs;in++;break;}
		}
		s=s.substr(index,s.length());
		index=s.find_first_not_of(notstr);
	}
	if(in>2)
		N[f].gate=1;
	int sonloc=0;
	for(int i=0;i<in;i++)
	{
		if(input[i][0]=='~')
		{
			N[f].invert[i]=false;
			input[i]=input[i].substr(1,input[i].length());
		}
		else
			N[f].invert[i]=true;
		sonloc=FindNode(input[i]);
		
		N[f].in[i]=sonloc;
		N[sonloc].out.push_back(f);
		
	}
	return;
}

void BuildGraph(string s){
	string subs;
	string::size_type index;
	index=s.find_first_not_of(notstr);
	s=s.substr(index,s.length());
	index=s.find_first_of(notstr);
	subs=s.substr(0,index);
	if(subs=="assign")
	{
		//cout<<s<<endl;
		s=s.substr(index,s.length());
		index=s.find_first_not_of(notstr);
		s=s.substr(index,s.length());
		index=s.find_first_of(notstr);
		subs=s.substr(0,index);
		s=s.substr(index,s.length());
		index=s.find_first_not_of(notstr);
		s=s.substr(index,s.length());
		int loc=FindNode(subs);
		if(loc>0)
			ExpandNode(loc,s);
	}
}

void OutAssign(int f){
	f_out<<"  assign ";
	f_out<<N[f].name<<" = ";
	if(N[f].gate==5)
	{
		if(!N[f].invert[0])
			f_out<<"~";
		f_out<<N[N[f].in[0]].name<<" ;"<<endl;
		return ;
	}
	if(N[f].gate>1&&N[f].gate<5)
	{
		if(!N[f].invert[0])
			f_out<<"~";
		f_out<<N[N[f].in[0]].name;
		if(N[f].gate==2)
			f_out<<" ^ ";
		else if(N[f].gate==3)
			f_out<<" & ";
		else if(N[f].gate==4)
			f_out<<" | ";
		if(!N[f].invert[1])
			f_out<<"~";
		f_out<<N[N[f].in[1]].name<<" ;"<<endl;
		return ;
	}
	if(N[f].gate==1)
	{
		f_out<<"( ";
		if(!N[f].invert[0])
			f_out<<"~";
		f_out<<N[N[f].in[0]].name;
		f_out<<" & ";
		if(!N[f].invert[1])
			f_out<<"~";
		f_out<<N[N[f].in[1]].name;
		f_out<<" ) | ( ";
		if(!N[f].invert[0])
			f_out<<"~";
		f_out<<N[N[f].in[0]].name;
		f_out<<" & ";
		if(!N[f].invert[2])
			f_out<<"~";
		f_out<<N[N[f].in[2]].name;
		f_out<<" ) | ( ";
		if(!N[f].invert[1])
			f_out<<"~";
		f_out<<N[N[f].in[1]].name;
		f_out<<" & ";
		if(!N[f].invert[2])
			f_out<<"~";
		f_out<<N[N[f].in[2]].name;
		f_out<<" );"<<endl;
	}
	
}

void InputFile(void){  //input the verilog file and construct the graph
	string subs;
	string::size_type index;
	getline(f_in,s);
	index=s.find_first_of(notstr);
	subs=s.substr(0,index);
	if(subs=="module")
	{
		s=s.substr(index+1,s.length());
		index=s.find_first_of(notstr);
		circuitname=s.substr(0,index);
		if(s[s.length()-1]!=';')
		{
			while(getline(f_in,s))
			{
				if(s[s.length()-1]==';')
					break;
			}
		}
		//cout<<"Input:"<<endl;
		InputNode("input",1);
		//cout<<"Output:"<<endl;
		InputNode("output",2);
		//cout<<"Wire:"<<endl;
		InputNode("wire",3);
		getline(f_in,s); //jump assign zero=0
		while(getline(f_in,s))
		{
			if(s=="endmodule")	break;
			BuildGraph(s);
		}
	}
	
}

void OutputFile(void){
	int i=0;
	int innum=0;
	int outnum=0;
	f_out<<"module ";
	f_out<<circuitname<<"( ";
	for(i=0;i<N.size();i++)
	{
		f_out<<N[i].name;
		if(N[i+1].type>2)
			{f_out<<" );"<<endl;break;}
		else
			f_out<<" , ";
	}
	f_out<<"  input ";
	for(i=0;i<N.size();i++)
	{
		f_out<<N[i].name;
		innum++;
		if(N[i+1].type!=1)
			{f_out<<" ;"<<endl;break;}
		else
			f_out<<" , ";
	}
	f_out<<"  output ";
	i++;
	while(i<N.size())
	{
		f_out<<N[i].name;
		outnum++;
		if(N[i+1].type!=2)
			{f_out<<" ;"<<endl;break;}
		else
			f_out<<" , ";
		i++;
	}
	f_out<<"  wire ";
	i++;
	while(i<N.size()-1)
	{
		f_out<<N[i].name;
		f_out<<" , ";
		i++;
	}
	f_out<<N[i].name;
	f_out<<" ;"<<endl;
	f_out<<"  assign zero = 0 ;"<<endl;
	for(i=innum+outnum;i<N.size();i++)
		if(N[i].name!="zero")
			OutAssign(i);
	for(i=innum;i<innum+outnum;i++)
		OutAssign(i);
	f_out<<"endmodule"<<endl;
}

int main(void){
	//locale::global(locale(""));
	f_in.open("/home/elab/Documents/AXMG/expr.v");
	f_out.open("/home/elab/Documents/AXMG/a.v");
	InputFile();
	OutputFile();
	return 0;
}
