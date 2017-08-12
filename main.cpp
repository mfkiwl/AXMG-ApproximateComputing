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
#define SimValue 100000
#define TYPE vector<node>
#define ITER vector<vector<node>::iterator>
#define random(x) (rand()%x)
#define si (*x).invert
#define rt (*x).in
#define Pointer vector<node>::iterator
#define MAJ 5
#define AND 1
#define OR 1
#define XOR 3
#define BUF 1
#define COM 0.5
#define UNITVOTE 1 //the vote value of output
#define ERROR 0.01


ifstream f_in;
ofstream f_out;

string s;
string notstr=" (),=";

typedef struct Node{
	string name;
	int type; // node type, 1 means input, 2 means output, 3 means wire
	vector<int> out; //fin-out
	int gate; // gate type, 1 means MAJ, 2 means XOR, 3 means AND, 4 means OR, 5 means buffer
    int innum; //input number
	int in[3];
	bool invert[3]; //true means 1, false means invert;
    bool sim[SimValue];
    //These three can be deleted
    double vote;    //vote value
    vector<double> ipp;  //probability of each input data pattern
    double area;    //deleted area, the original is 5/2
}node;

TYPE N; //Main vector conatin all nodes
ITER Q; //TopologicalSorting Result
string circuitname;
int inputnum=0, outputnum=0;
int MaxT,TemMaxT;
vector<int> QN; //represent the undeleted value

void BuildNode(string name,int nodetype);
void InputNode(string statestring,int nodetype,int& num);
void InputWire(string statestring);
int FindNode(string name);
void ExpandNode(int f,string s);
void BuildGraph(string s);
void OutAssign(int f);
void InputFile(void);
void OutputFile(void);
void TopologicalSorting(void);
void RandomInput(void);
void SimulateNode(void);
void OperateValue(vector<node>::iterator x,bool *sim);
void MeasureInput(bool *invert, bool *sim1, bool *sim2, bool *sim3, vector<double>& ipp);
int FindDeleteNode(void);
double Measure(int n);
double ReduceArea(int l);
void MeasureVote(void);
double SimError(int del,vector<bool[SimValue]>& appsim);
void SimulateApproNode(vector<node>::iterator x,bool *sim, vector<bool[SimValue]>& appsim);
void DeleteNode(int del);
void CheckDeleteInput(int del);
void OutputApproCircuit(void);

int main(int argc,char **argv){
	f_in.open(argv[1]);
	f_out.open("/home/elab/Documents/AXMG/a.v");
	InputFile();
    TopologicalSorting();
    RandomInput();
    SimulateNode();
    vector<bool[SimValue]> appsim(N.size());
    for(int i=0;i<inputnum;i++)
        for(int j=0;j<SimValue;j++)
            appsim[i][j]=N[i].sim[j];
    double error=0;
    int del;
    int i=0;
    while(error<=ERROR)
    //while(i<4)
    {
        del=FindDeleteNode();
        if(del==-1)
            break;
        error=SimError(del,appsim);
        if(error>ERROR)
            break;
        //i++;
        DeleteNode(del);
        //f_out.open("/home/elab/Documents/AXMG/a.v");
        //OutputApproCircuit();
        //f_out.close();
        printf("running time: %ld\n",clock()/1000);
    }
    OutputApproCircuit();
    printf("running time: %ld\n",clock()/1000);
	return 0;
}

void BuildNode(string name,int nodetype){ //build node
    node x;
    x.name=name;
    x.type=nodetype;
    x.vote=0;
    x.gate=0;
    x.invert[0]=true;
    x.invert[1]=true;
    x.invert[2]=true;
    N.push_back(x);
    QN.push_back((int)N.size()-1);
}

void InputNode(string statestring,int nodetype,int& num){
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
            num++;
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
                num++;
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

void InputWire(string statestring){
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
            if(subs!="zero")
                BuildNode(subs,3);
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
                BuildNode(subs,3);
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
        N[f].innum=1;
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
    N[f].innum=2;
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
        {N[f].gate=1;N[f].innum=3;}
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
        InputNode("input",1,inputnum);
        //cout<<"Output:"<<endl;
        InputNode("output",2,outputnum);
        //cout<<"Wire:"<<endl;
        InputWire("wire");
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
    //f_out<<"  assign zero = 0 ;"<<endl;
    for(i=innum+outnum;i<N.size();i++)
        OutAssign(i);
    for(i=innum;i<innum+outnum;i++)
        OutAssign(i);
    f_out<<"endmodule"<<endl;
}

void OutputApproCircuit(void){
    int i=0;
    f_out<<"module ";
    f_out<<circuitname<<"( ";
    for(i=0;i<inputnum+outputnum;i++)
    {
        if(N[i].type<=2)
        {
            f_out<<N[i].name;
            if(i<inputnum+outputnum-1)
                f_out<<" , ";
            else
                f_out<<" );"<<endl;
        }
        else
            break;
    }
    f_out<<"  input ";
    for(i=0;i<inputnum;i++)
    {

        if(N[i].type==1)
        {
            f_out<<N[i].name;
            if(i<inputnum-1)
                f_out<<" , ";
            else
                f_out<<" ;"<<endl;
        }
        else
            break;
    }
    f_out<<"  output ";
    for(;i<inputnum+outputnum;i++)
    {
        if(N[i].type==2)
        {
            f_out<<N[i].name;
            if(i<inputnum+outputnum-1)
                f_out<<" , ";
            else
                f_out<<" ;"<<endl;
        }
        else
            break;
    }
    f_out<<"  wire  ";
    for(i=0;i<Q.size();i++)
    {
        if((*(Q[i])).type==3)
        {
            f_out<<(*(Q[i])).name;
            break;
        }
    }
    i++;
    for(;i<Q.size();i++)
    {
        if((*(Q[i])).type==3)
        {
            f_out<<" , ";
            f_out<<(*(Q[i])).name;
        }
    }
    f_out<<" ;"<<endl;
    for(i=0;i<Q.size();i++)
        OutAssign((int)(Q[i]-N.begin()));
    f_out<<"endmodule"<<endl;
}

void TopologicalSorting(void) {
    queue<vector<node>::iterator> L;
    int i;
    for(i=0;i<inputnum;i++)
        L.push(N.begin()+i);
    int Nin[N.size()];
    for(i=0;i<N.size();i++)
        Nin[i]=N[i].innum;
    vector<node>::iterator x;
    int index=0;
    while(!L.empty())
    {
        x=L.front();
        L.pop();
        for(i=0;i<(*x).out.size();i++)
        {
            index=(*x).out[i];
            Nin[index]--;
            if(Nin[index]==0)
            {
                L.push(N.begin()+index);
                Q.push_back(N.begin()+index);
            }
        }
    }
}

void RandomInput(void){
    srand((int)time(0));
    int i,j;
    for(i=0;i<inputnum;i++)
        for(j=0;j<SimValue;j++)
            N[i].sim[j]=random(2);
}

void OperateValue(vector<node>::iterator x,bool *sim){
    int j;
    if((*x).gate==1)
        for(j=0;j<SimValue;j++)
            sim[j]=(si[0]?N[rt[0]].sim[j]:~N[rt[0]].sim[j])&&(si[1]?N[rt[1]].sim[j]:~N[rt[1]].sim[j])||\
(si[0]?N[rt[0]].sim[j]:~N[rt[0]].sim[j])&&(si[2]?N[rt[2]].sim[j]:~N[rt[2]].sim[j])||\
(si[1]?N[rt[1]].sim[j]:~N[rt[1]].sim[j])&&(si[2]?N[rt[2]].sim[j]:~N[rt[2]].sim[j]);
    else if((*x).gate==2)
        for(j=0;j<SimValue;j++)
            sim[j]=(si[0]?N[rt[0]].sim[j]:~N[rt[0]].sim[j])^(si[1]?N[rt[1]].sim[j]:~N[rt[1]].sim[j]);
    else if((*x).gate==3)
        for(j=0;j<SimValue;j++)
            sim[j]=(si[0]?N[rt[0]].sim[j]:~N[rt[0]].sim[j])&&(si[1]?N[rt[1]].sim[j]:~N[rt[1]].sim[j]);
    else if((*x).gate==4)
        for(j=0;j<SimValue;j++)
            sim[j]=(si[0]?N[rt[0]].sim[j]:~N[rt[0]].sim[j])||(si[1]?N[rt[1]].sim[j]:~N[rt[1]].sim[j]);
    else if((*x).gate==5)
        for(j=0;j<SimValue;j++)
            sim[j]=si[0]?N[rt[0]].sim[j]:~N[rt[0]].sim[j];
}

void SimulateNode(void){
    int i;
    for(i=0;i<Q.size();i++)
    {
        OperateValue(Q[i],(*Q[i]).sim);
    }
}

void MeasureInput(bool *invert, bool *sim1, bool *sim2, bool *sim3, vector<double>& ipp){
    int p[8]={0,0,0,0,0,0,0,0};
    int t=0,i,inv=0;
    for(i=0;i<3;i++)
        inv=(invert[0]?4:0)+(invert[1]?2:0)+(invert[2]?1:0);
    if(inv==0)
    {
        for(i=0;i<SimValue;i++)
        {t=((!sim1[i])?4:0)+((!sim2[i])?2:0)+((!sim3[i])?1:0);p[t]++;}
    }
    else if(inv==1)
    {
        for(i=0;i<SimValue;i++)
        {t=((!sim1[i])?4:0)+((!sim2[i])?2:0)+((sim3[i])?1:0);p[t]++;}
    }
    else if(inv==2)
    {
        for(i=0;i<SimValue;i++)
        {t=((!sim1[i])?4:0)+((sim2[i])?2:0)+((!sim3[i])?1:0);p[t]++;}
    }
    else if(inv==3)
    {
        for(i=0;i<SimValue;i++)
        {t=((!sim1[i])?4:0)+((sim2[i])?2:0)+((sim3[i])?1:0);p[t]++;}
    }
    else if(inv==4)
    {
        for(i=0;i<SimValue;i++)
        {t=((sim1[i])?4:0)+((!sim2[i])?2:0)+((!sim3[i])?1:0);p[t]++;}
    }
    else if(inv==5)
    {
        for(i=0;i<SimValue;i++)
        {t=((sim1[i])?4:0)+((!sim2[i])?2:0)+((sim3[i])?1:0);p[t]++;}
    }
    else if(inv==6)
    {
        for(i=0;i<SimValue;i++)
        {t=((sim1[i])?4:0)+((sim2[i])?2:0)+((!sim3[i])?1:0);p[t]++;}
    }
    else if(inv==7)
    {
        for(i=0;i<SimValue;i++)
        {t=((sim1[i])?4:0)+((sim2[i])?2:0)+((sim3[i])?1:0);p[t]++;}
    }
    for(i=0;i<8;i++)
        ipp.push_back((double)p[i]/SimValue);
}

double ReduceArea(int l){
    double area=0;
    if(N[l].out.size()==1&&N[l].type!=1)
    {
        if(N[l].gate==1)
        {
            area+=MAJ;
            area+=(N[l].invert[0]?0:COM)+(N[l].invert[1]?0:COM)+(N[l].invert[2]?0:COM);
        }
        else if(N[l].gate==2)
        {
            area+=XOR;
        }
        else if(N[l].gate==3)
        {
            area+=AND;
            area+=(N[l].invert[0]?0:COM)+(N[l].invert[1]?0:COM);
        }
        else if(N[l].gate==4)
        {
            area+=OR;
            area+=(N[l].invert[0]?0:COM)+(N[l].invert[1]?0:COM);
        }
        else if(N[l].gate==5)
        {
            area+=BUF;
            area+=(N[l].invert[0]?0:COM);
        }
        for(int i=0;i<N[l].innum;i++)
        {
            ReduceArea(N[l].in[i]);
        }
    }
    return area;
}

void MeasureVote(void){
    Pointer p;
    int i,j;
    for(i=(int)Q.size()-1;i>=0;i--)
    {
        p=Q[i];
        if((*p).type==2)
            (*p).vote=UNITVOTE;
        else if((*p).type==3)
        {
            for(j=0;j<(*p).out.size();j++)
            {
                if(N[(*p).out[j]].gate==2)
                    (*p).vote+=N[(*p).out[j]].vote/2;
                else if(N[(*p).out[j]].gate==5)
                    (*p).vote+=N[(*p).out[j]].vote;
                else
                    (*p).vote+=N[(*p).out[j]].vote/3;
            }
        }
    }
    return;
}

double Measure(int l){
    double P=0,temP=0;
    double area[3]={MAJ,MAJ,MAJ};
    MeasureInput(N[l].invert,N[N[l].in[0]].sim,N[N[l].in[1]].sim,N[N[l].in[2]].sim,N[l].ipp);
    for(int i=0;i<3;i++)
    {
        area[i]+=ReduceArea(N[l].in[i]);
        if(!N[l].invert[i])
            for(int j=0;j<3;j++)
                if(i!=j)
                    area[j]+=COM;
    }
    for(int i=0;i<3;i++)
    {
        temP=area[i]/(N[l].ipp[3-i]+N[l].ipp[4+i]);
        if(P<temP)
        {
            P=temP;
            TemMaxT=i;
            N[l].area=area[i];
        }
    }
    P=P/N[l].vote;
    return P;
}

int FindDeleteNode(void){
    int del=-1;
    int i=0;
    double MaxP=0;
    double P=0;
    MaxT=0;
    MeasureVote();
    for(i=inputnum;i<QN.size();i++)
    {
        if(N[QN[i]].gate==1)
        {
            del=QN[i];
            break;
        }
    }
    for(;i<QN.size();i++)
    {
        if(N[QN[i]].gate==1)
        {
            P=Measure(QN[i]);
            if(P>=MaxP)
            {MaxP=P;MaxT=TemMaxT;del=QN[i];}
        }
    }
    return del;
}

void SimulateApproNode(vector<node>::iterator x,bool *sim,vector<bool[SimValue]>& appsim){
    int j;
    if((*x).gate==1)
        for(j=0;j<SimValue;j++)
            sim[j]=(si[0]?appsim[rt[0]][j]:~appsim[rt[0]][j])&&(si[1]?appsim[rt[1]][j]:~appsim[rt[1]][j])||\
(si[0]?appsim[rt[0]][j]:~appsim[rt[0]][j])&&(si[2]?appsim[rt[2]][j]:~appsim[rt[2]][j])||\
(si[1]?appsim[rt[1]][j]:~appsim[rt[1]][j])&&(si[2]?appsim[rt[2]][j]:~appsim[rt[2]][j]);
    else if((*x).gate==2)
        for(j=0;j<SimValue;j++)
            sim[j]=(si[0]?appsim[rt[0]][j]:~appsim[rt[0]][j])^(si[1]?appsim[rt[1]][j]:~appsim[rt[1]][j]);
    else if((*x).gate==3)
        for(j=0;j<SimValue;j++)
            sim[j]=(si[0]?appsim[rt[0]][j]:~appsim[rt[0]][j])&&(si[1]?appsim[rt[1]][j]:~appsim[rt[1]][j]);
    else if((*x).gate==4)
        for(j=0;j<SimValue;j++)
            sim[j]=(si[0]?appsim[rt[0]][j]:~appsim[rt[0]][j])||(si[1]?appsim[rt[1]][j]:~appsim[rt[1]][j]);
    else if((*x).gate==5)
        for(j=0;j<SimValue;j++)
            sim[j]=si[0]?appsim[rt[0]][j]:~appsim[rt[0]][j];
}

double SimError(int del,vector<bool[SimValue]>& appsim){
    double error=0;
    int errornum=0;
    int store_in;
    bool store_invert;
    store_in=N[del].in[0];
    store_invert=N[del].invert[0];
    N[del].gate=5;
    N[del].innum=1;
    N[del].in[0]=N[del].in[MaxT];
    N[del].invert[0]=N[del].invert[MaxT];
    for(int i=0;i<Q.size();i++)
    {
        SimulateApproNode(Q[i],appsim[Q[i]-N.begin()],appsim);
    }
    for(int i=0;i<SimValue;i++)
    {
        for(int j=inputnum;j<inputnum+outputnum;j++)
            if(appsim[j][i]^N[j].sim[i])
            {
                errornum++;
                break;
            }
    }
    error=(double)errornum/SimValue;
    N[del].gate=1;
    N[del].innum=3;
    N[del].in[0]=store_in;
    N[del].invert[0]=store_invert;
    return error;
}

void CheckDeleteInput(int del){
    for(vector<int>::iterator i=N[del].out.begin();i!=N[del].out.end();i++)
        if((*i)==del)
            N[del].out.erase(i);
    if(N[del].out.size()==0)
    {
        for(ITER::iterator i=Q.begin();i!=Q.end();i++)
            if((*i)==(N.begin()+del))
            {Q.erase(i);break;}
        for(vector<int>::iterator i=QN.begin();i!=QN.end();i++)
            if((*i)==del)
            {QN.erase(i);break;}
        for(int i=0;i<N[del].innum;i++)
            CheckDeleteInput(N[del].in[i]);
    }
    return;
}

void DeleteNode(int del){
    if(N[del].type==2)
    {
        N[del].gate=5;
        for(int i=0;i<N[del].innum;i++)
            if(i!=MaxT)
                CheckDeleteInput(N[del].in[i]);
        N[del].innum=1;
        N[del].in[0]=N[del].in[MaxT];
        N[del].invert[0]=N[del].invert[MaxT];
    }
    else if(N[del].type==3)
    {
        int MT=N[del].in[MaxT];
        bool invert=N[del].invert[MaxT];
        for(vector<int>::iterator i=N[MT].out.begin();i!=N[MT].out.end();i++)
            if((*i)==del)
            {N[MT].out.erase(i);break;}
        for(int i=0;i<N[del].out.size();i++)
        {
            N[MT].out.push_back(N[del].out[i]);
            for(int j=0;j<N[N[del].out[i]].innum;j++)
            {
                if(N[N[del].out[i]].in[j]==del)
                {
                    N[N[del].out[i]].in[j]=MT;
                    N[N[del].out[i]].invert[j]=!(N[N[del].out[i]].invert[j]^invert);
                    j=3;
                }
            }
        }
        for(ITER::iterator i=Q.begin();i!=Q.end();i++)
            if((*i)==(N.begin()+del))
            {Q.erase(i);break;}
        for(vector<int>::iterator i=QN.begin();i!=QN.end();i++)
            if((*i)==del)
            {QN.erase(i);break;}
        for(int i=0;i<N[del].innum;i++)
            if(i!=MaxT)
                CheckDeleteInput(N[del].in[i]);
    }
    return;
}