#include <iostream>
#include <cstring>
#include <ctime>
#include <chrono>
#include <utility>
#include <algorithm>
#include <math.h>
#include <unistd.h>
#include <vector>
#include <assert.h>
using namespace std;
const int init_pos[2][6]={{0,1,2,6,7,12},{23,28,29,33,34,35}};
const int vertical_bound[2][6]={{30,31,32,33,34,35},{0,1,2,3,4,5}};
const int horizontal_bound[2][6]={{5,11,17,23,29,35},{0,6,12,18,24,30}};
const int ply[2][3]={{6,1,7},{-6,-1,-7}};
int game_simu=200;
int step;
struct NODE{
	int ply=0;
	int position[2][6];
	int legal_step[2][18]={{0,1,2,10,11,12,20,21,22,30,31,32,40,41,42,50,51,52},{0,1,2,10,11,12,20,21,22,30,31,32,40,41,42,50,51,52}};
	int legalnumber[2]={18,18};
	int dest[2]={6,6};
	int parent_id=-1;
	int children[18];
	int Nchild=0;
	int depth=0;
	int win_game=0;
	int Ntotal=0;
	vector <int> map;
	float sqrtlogNtotal=0;
	float sqrt_Ntotal=0;
	float winrate=0;
	float UCBscore=0;
};

struct NODE nodes[500000];
void copyarray(struct NODE &nodea,struct NODE &nodeb)
{
	copy(&nodea.position[0][0],&nodea.position[0][0]+2*6,&nodeb.position[0][0]);
	copy(&nodea.legal_step[0][0],&nodea.legal_step[0][0]+2*18,&nodeb.legal_step[0][0]);
	copy(&nodea.legalnumber[0],&nodea.legalnumber[0]+2,&nodeb.legalnumber[0]);
	copy(&nodea.dest[0],&nodea.dest[0]+2,&nodeb.dest[0]);
	nodeb.map.assign(nodea.map.begin(),nodea.map.end());
}

void renew(int position[2][6],int legal_step[2][18],int legalnumber[2],int dest[2],bool turn,int whomove,bool first)
{
	if(position[turn][whomove]==0 || position[turn][whomove]==35)
		dest[turn]=whomove;
	for(int i=0;i<2;i++)//check eat
	{
		for(int j=0;j<6;j++)
		{
			if(i==turn and j==whomove)
				continue;
			if(position[turn][whomove]==position[i][j])
			{
				position[i][j]=36;
				int m=0;
				while(m<legalnumber[i])
				{
					if(legal_step[i][m]/10==j)
						swap(legal_step[i][m],legal_step[i][--legalnumber[i]]);
					else
						m++;
				}
				break;
			}
		}
	}
	for(int i=0;i<6;i++)//check whether meet bound
	{
		if(position[turn][whomove]==vertical_bound[first^turn][i])
		{
			int m=0;
			while(m<legalnumber[turn])
			{
				if(whomove==(legal_step[turn][m]/10) and !(legal_step[turn][m]%2))
					swap(legal_step[turn][m],legal_step[turn][--legalnumber[turn]]);
				else
					m++;
			}
			break;
		}
	}
	for(int i=0;i<6;i++)
	{
		if(position[turn][whomove]==horizontal_bound[first^turn][i])
		{
			int m=0;
			while(m<legalnumber[turn])
			{
				if(whomove==(legal_step[turn][m]/10) and (legal_step[turn][m]%10)>0)
					swap(legal_step[turn][m],legal_step[turn][--legalnumber[turn]]);
				else
					m++;
			}
			break;
		}
	}
}

//choose(recalculate gamept and wr here)
int choose(bool one)
{
	int current_id=0;
	while(nodes[current_id].Nchild>0)
	{
		float tmp=-500;
		int tmp_id;
		nodes[current_id].sqrtlogNtotal=sqrt(log10(double(nodes[current_id].Ntotal)));
		for(int i=0;i<nodes[current_id].Nchild;i++)
		{
			int child=nodes[current_id].children[i];
			nodes[child].sqrt_Ntotal=sqrt(nodes[child].Ntotal);
			nodes[child].winrate=float(nodes[child].win_game)/float(nodes[child].Ntotal);
			nodes[child].UCBscore=nodes[child].winrate+1.18*nodes[current_id].sqrtlogNtotal/nodes[child].sqrt_Ntotal;
			if(nodes[child].UCBscore>tmp)
			{
				tmp=nodes[child].UCBscore;
				tmp_id=child;
			}
		}
		current_id=tmp_id;
		if(one) return current_id;
	}
	return current_id;
}
//expand
void expand(int id,int &node_number,bool first)
{
	bool turn=nodes[id].depth%2;
	bool eatmove=false;
	for(int i=0;i<nodes[id].legalnumber[turn];i++)
	{
		int whomove=nodes[id].legal_step[turn][i]/10;
		int step=nodes[id].legal_step[turn][i]-whomove*10;
		int dest=nodes[id].position[turn][whomove]+ply[first^turn][step];
		if(nodes[id].map[dest]==(1-turn))
		{
			eatmove=1;
			break;
		}
	}
	for(int i=0;i<nodes[id].legalnumber[turn];i++)
	{
		bool goodmove=1;
		if(eatmove)
		{
			int whomove=nodes[id].legal_step[turn][i]/10;
			int step=nodes[id].legal_step[turn][i]-whomove*10;
			int dest=nodes[id].position[turn][whomove]+ply[first^turn][step];
			if(nodes[id].map[dest]!=(1-turn))
				goodmove=0;
		}
		if(goodmove)
		{
			int whomove=nodes[id].legal_step[turn][i]/10;
			int step=nodes[id].legal_step[turn][i]-whomove*10;
			copyarray(nodes[id],nodes[node_number]);//copy parent board
			nodes[node_number].ply=nodes[id].legal_step[turn][i];
			nodes[node_number].parent_id=id;//write parent id
			nodes[node_number].map[nodes[node_number].position[turn][whomove]]=-1;
			nodes[node_number].position[turn][whomove]+=ply[first^turn][step];//child board renew
			nodes[node_number].map[nodes[node_number].position[turn][whomove]]=turn;
			nodes[id].children[nodes[id].Nchild++]=node_number;//write in parent's child
			nodes[node_number].depth=nodes[id].depth+1;
			renew(nodes[node_number].position,nodes[node_number].legal_step,nodes[node_number].legalnumber,nodes[node_number].dest,turn,whomove,first);
			node_number++;
		}
	}
}

bool endgame(int dest[2],int position[2][6],int &score)
{
	for(int i=0;i<2;i++)
	{
		bool allgg=true;
		for(int j=0;j<6;j++)
		{
			if(position[i][j]!=36)
			{
				allgg=0;
				break;
			}
		}
		if(allgg)
		{
			score=(i)?1000:-1000;
			return true;
		}
	}
	if(dest[0]!=6 and dest[1]!=6)
	{
		if(step<30)
		{
			int tmp=0;
			for(int i=0;i<2;i++)
			{
				int pm=(i)?-2:+2;
				for(int j=0;j<6;j++)
				{
					if(position[i][j]!=36)
						tmp+=(pm);
				}
			}
			score+=tmp;
		}
		if(position[0][0]!=36)
			score+=5;
		else
			score-=5;
		if(position[1][0]!=36)
			score-=5;
		else
			score+=5;
		if(dest[0]>dest[1])
			score-=1;
		else if(dest[0]==dest[1])
			score+=0;
		else
			score+=1;
		return true;
	}
	return false;
}
//simulate
int simulate(int id,bool first)
{
	struct NODE simunode;
	int total=0;
	bool turn;
	for(int j=0;j<game_simu;j++)
	{
		copyarray(nodes[id],simunode);
		turn=nodes[id].depth%2;
		int score=0;
		while(!endgame(simunode.dest,simunode.position,score))
		{
			if(simunode.legalnumber[turn]!=0)
			{
				int whomove,step;
				int ran=simunode.legal_step[turn][rand()%simunode.legalnumber[turn]];
				whomove=ran/10;
				step=ran%10;
				simunode.position[turn][whomove]+=ply[turn^first][step];
				renew(simunode.position,simunode.legal_step,simunode.legalnumber,simunode.dest,turn,whomove,first);
			}
			turn=1-turn;
		}
		total+=score;
	}
	return total;
}
//add result (Ntotal,win_game) for all parent until parentID==-1
void addresult(int id,int score)
{
	while(id!=-1)
	{
		int plusminus=(nodes[id].depth%2)?1:-1;
		nodes[id].win_game+=int(plusminus*score);
		nodes[id].Ntotal+=int(game_simu);
		id=nodes[id].parent_id;
	}
}

int main()
{
	auto timer=[](bool reset=false)->double{
		static decltype(std::chrono::steady_clock::now()) tick,tock;
		if(reset){
			tick=std::chrono::steady_clock::now();
			return(0);
		}
		tock=std::chrono::steady_clock::now();
		return(std::chrono::duration_cast<std::chrono::duration<double>>(tock-tick).count());
	};
	srand(time(NULL));
	do
	{
		step=0;
		struct NODE node;
		for(int i=0;i<2;++i)
		{
			for(int j=0;j<6;++j)
			{
				int a=getchar()-'0';
				node.position[i][a]=init_pos[i][j];
			}
		}
		bool first=(getchar()=='f')?0:1;
		bool turn=first;
		if(first)
		{
			for(int i=0;i<6;i++)
				swap(node.position[0][i],node.position[1][i]);
		}
		int score=0;
		vector <int> mymap(36,-1);
		for(int i=0;i<2;i++)
		{
			for(int j=0;j<6;j++)
				mymap[node.position[i][j]]=i;
		}
		node.map.assign(mymap.begin(),mymap.end());
		while(!endgame(node.dest,node.position,score))
		{
			step++;
			if(!turn)
			{
				int node_num=1;;
				clock_t start=clock();
				copyarray(node,nodes[0]);
				node_num=1;
				if(node.legalnumber[0]==0)
				{
					cout<<"??"<<flush;
					turn=1-turn;
					continue;
				}
				while(double(clock()-start)/CLOCKS_PER_SEC<9.9)
				{
					int id=choose(0);
					int tmp=node_num;
					if(node_num<499900)
						expand(id,node_num,first);
					if(node_num==tmp)
						id=nodes[id].parent_id;
					for(int i=0;i<nodes[id].Nchild;i++)
					{
						int child=nodes[id].children[i];
						int score=simulate(child,first);
						addresult(child,score);
					}
				}
				int myply=nodes[choose(1)].ply;
				int whomove=myply/10;
				int dir=myply-whomove*10;
				char f='0'+whomove;
				char s='0'+dir;
				cout<<f<<s<<flush;
				node.map[node.position[0][whomove]]=-1;
				node.position[0][whomove]+=ply[0^first][dir];
				node.map[node.position[0][whomove]]=0;
				renew(node.position,node.legal_step,node.legalnumber,node.dest,0,myply/10,first);
				for(int i=0;i<node_num;i++)
				{
					nodes[i].Nchild=0;
					nodes[i].win_game=0;
					nodes[i].Ntotal=0;
				}
			}
			else
			{
				int whomove,dir;
				whomove=getchar()-'0';
				dir=getchar()-'0';
				node.map[node.position[1][whomove]]=-1;
				node.position[1][whomove]+=ply[1^first][dir];
				node.map[node.position[1][whomove]]=1;
				renew(node.position,node.legal_step,node.legalnumber,node.dest,1,whomove,first);
			}
			turn=1-turn;
		}
	}while(getchar()=='y');
}
