#include <WinSock2.h>  
#include "mysql.h"  
#pragma comment(lib,"wsock32.lib")  
#pragma comment(lib,"libmysql.lib")   
#pragma comment(lib,"wsock32.lib")  
#pragma comment(lib,"libmysql.lib")  
#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <map>
#include <list>
#include <atlstr.h>
using namespace std;
#define ITEMNUM 1285
#define ITEMLEN 4
/***mysql变量***/
MYSQL mysql;  
MYSQL_RES *result;  
MYSQL_ROW row;  
CString strSQL;
int res;

class Data
{
	friend ostream &operator<<(ostream &output, const Data* data);
public:
	Data() {}
	~Data() {}
	int bv;
	bitset<ITEMNUM> tidlist;
	double sup;
};
ostream& operator<<( ostream& os,const Data* data)
{
	return os << data->bv << ":" << data->tidlist<< ":" <<data->sup;
}
typedef struct
{
	vector<int*> p;     //记录i项集的所有内容（比如1项集的内容，2项集的内容，或者3项集的内容等等）
	vector<double> sup;    //各个项集的支持计数（比如1项集中的每一条记录的支持度计数为多少，或者2项集中的每一条记录的支持度计数为多少等等）
	int n;        //项集的数目（比如得到的1项集中有多少条记录，或者为2项集中有多少条记录，或者3项集中有多少条记录）
} Large;
void printLar(vector<Large> &Lar)
{
	for(int i=1; i<=Lar.size(); i++)
	{
		for(int j=1; j<=Lar[i-1].n; j++)
		{
			cout<<Lar[i-1].p[j-1]<<":"<<Lar[i-1].sup[j-1]<<endl;
		}
	}
}
string int2str(int n) {

	char t[24];
	int i = 0;

	while (n) {
		t[i++] = (n % 10) + '0';
		n /= 10;
	}
	t[i] = 0;
	string ret=(string)_strrev(t);
	if (ret.length()<1)
	{
		ret="0";
	}
	return ret;
}
bool Comp(Data* first,Data* second)
{
	return first->sup > second->sup;
}
/*栈*/
template<typename T, class C = list<T> >
class Stack
{
public:
	typedef typename C::iterator iterator;
	Stack()
	{
		stacksize=0;
	}
	iterator begin()
	{
		return cc.begin();
	}
	iterator end()
	{
		return cc.end();
	}
	void push(const T& vaule)
	{
		stacksize++;
		cc.push_front(vaule);
	}
	void pop()
	{
		stacksize--;
		cc.pop_front();
	}
	bool empty()
	{
		return stacksize==0;
	}
	T top()
	{
		return cc.front();
	}
	int size()
	{
		return stacksize;
	}
private:
	int stacksize;
	C cc;
};

/*图*/

template<typename VertexType,typename VertexType2,typename VertexType3, typename InfoType>
class ALGraph
{
public:
	static const int MAX_VERTEX_NUM = 20;  //最大顶点个数


	struct ArcNode          //表结点
	{
		int adjvex;        //该弧所指向的顶点的位置
		ArcNode *nextarc;  //指向下一条弧的指针
		InfoType info;     //该弧相关信息的指针
	};
	struct VNode           //头结点
	{
		VertexType name;    //顶点英文字母
		VertexType2 data;    //顶点支持度
		VertexType3 tidlist; //tidlist
		ArcNode *firstarc;  //指向第一条依附于该顶点的弧的指针
	};

	/*VNode AdjList[MAX_VERTEX_NUM];*/
	/* AdjList[MAX_VERTEX_NUM] vertices;*/
	VNode vertices[MAX_VERTEX_NUM];
	int vexnum;             //图的当前顶点数
	int arcnum;             //图的弧数

	ALGraph(int verNum)
		: vexnum(verNum), arcnum(0)
	{
		for (int i = 0; i < MAX_VERTEX_NUM; i++)
			vertices[i].firstarc = NULL;
	}

	//打印邻接链表
	void displayGraph()
	{
		for (int i = 0; i < vexnum; i++)
		{
			cout << "第" << i+1 << "个顶点是：" << vertices[i].name<<"支持度是"<<vertices[i].data
				<< " 邻接表为: ";
			ArcNode *arcNode = vertices[i].firstarc;
			while (arcNode != NULL)
			{
				cout << " -> " << vertices[arcNode->adjvex].name
					<< "(" << arcNode->info << ")";
				arcNode = arcNode->nextarc;
			}
			cout << endl;
		}
	}
	//构造一个有向图
	void CreateDG( vector<Data*> &dataarray, map<int,double>&minsupmap,Large& L2)
	{
		InitVertics(dataarray);
		int n=dataarray.size();
		for(int i=1; i<=n; i++)
		{
			Data* vi=dataarray[i-1];
			for(int j=i+1; j<=n; j++)
			{
				Data* vj=dataarray[j-1];
				double sup1=min(vi->sup,vj->sup);
				double sup2=max(minsupmap[vi->bv],minsupmap[vj->bv]);
				if (sup1>=sup2)
				{
					double sup3=(double)(vi->tidlist&vj->tidlist).count()/ITEMNUM;
					if(sup3>=sup2)
					{
						int *temp=new int[2];
						temp[0]=vj->bv;
						temp[1]=vi->bv;
						L2.p.push_back(temp);
						L2.sup.push_back(sup3);
						L2.n++;
						insertArc(i-1, j-1, 0);
					}
				}
				//
			}
		}
	}
	bool isempty()
	{
		return 0==vexnum;
	}
	void DFSsearch(vector<Data*> &dataarray, map<int,double>&minsupmap,vector<Large> &Lar)
	{
		//深度优先搜索非递归算法
		Stack <int>s;
		Stack <ArcNode*>ps;//stack s push之后 将p也push到ps中保存信息 stack s pop之后 ps也pop
		ArcNode* p;
		//int* visited=new int [vexnum+1]();
		//visited[0] = 1;
		s.push(0);
		p=vertices[s.top()].firstarc;
		while (!s.empty())
		{
			while(p)
			{
				if(s.size()>=2)
				{
		/*			cout<<dataarray[p->adjvex]->bv<<endl;
					Stack<int>::iterator ii= s.begin();		
					while (ii != s.end())
					{
						cout<<vertices[*ii].name<<endl;
						ii++;
					}
					cout<<"************************"<<endl;*/
					bool allbeside=true;
					Stack<int>::iterator itor= s.begin();
					while (itor != s.end())
					{
						int adj=p->adjvex;
						bool beside=false;
						ArcNode* pf=vertices[*itor].firstarc;
						itor++;
						while(pf)
						{
							if(pf->adjvex==adj)
							{
								beside=true;
								break;
							}
							else
							{
								pf=pf->nextarc;
							}
						}
						if(!beside)
						{
							allbeside=false;
							break;
						}
					}
					if(allbeside)//Vk(p->adjvex)是否是其所有的祖先结点的邻接点
					{
						int i=1;//频繁集层数
						double sup1=dataarray[p->adjvex]->sup;
						double sup2=minsupmap[dataarray[p->adjvex]->bv];
						Stack<int>::iterator itor= s.begin();	
						while (itor != s.end())
						{
							i++;
							sup1=min(sup1,vertices[*itor].data);
							sup2=max(sup2,minsupmap[vertices[*itor].name]);
							itor++;
						}

						if(sup1>=sup2)
						{
							bitset<ITEMNUM> bit=dataarray[p->adjvex]->tidlist;
							itor= s.begin();
							while (itor != s.end())
							{
								bit=bit&vertices[*itor].tidlist;
								itor++;
							}
							double sup3=(double)bit.count()/ITEMNUM;
							if(sup3>=sup2)
							{
								if(Lar.size()==i-1)
								{
									Large Li;
									Li.n=0;
									Lar.push_back(Li);
								}
								else if(Lar.size()<i-1)
								{
									cout<<"so wrong!!!"<<endl;
								}
								int *temp=new int[i];
								temp[0]=dataarray[p->adjvex]->bv;
								Stack<int>::iterator itor= s.begin();
								int j=1;
								while (itor != s.end())
								{
									temp[j]=vertices[*itor].name;
									j++;
									itor++;
								}
								Lar[i-1].n++;
								Lar[i-1].p.push_back(temp);
								Lar[i-1].sup.push_back(sup3);
								s.push(p->adjvex);
								ps.push(p);
								p=vertices[s.top()].firstarc;
							}
							else
								p=p->nextarc;
						}
						else
							p=p->nextarc;	
					}
					else
						p=p->nextarc;
				}
				else
				{
					s.push(p->adjvex);
					ps.push(p);
					p=vertices[s.top()].firstarc;
				}
			}
			if (p == NULL)
			{
				s.pop();
				if(!ps.empty())//s最后会比ps多一个
				{
					p=ps.top()->nextarc;
					ps.pop();
				}
			}
		}

	}
	void deleteVex()
	{
		//初始条件：图G存在，v是G中某个顶点。操作结果：删除G中顶点v及其相关弧
		int i,j=0;
		ArcNode *p,*q;
		p=vertices[j].firstarc;//删除以v为初读的弧或边
		while(p)
		{
			q=p;
			p=p->nextarc;
			free (q);
			arcnum--;
		}
		vexnum--;
		for(i=j; i<vexnum; i++) //顶点v后面的顶点前移
			vertices[i]=vertices[i+1];
		for(i=0; i<vexnum; i++) //删除以v为入度的弧或边且必要时修改表结点的顶点位置值
		{
			p=vertices[i].firstarc;
			while(p)
			{
				if(p->adjvex==j)
				{
					if(p==vertices[i].firstarc)//待删除的结点是第一个结点
					{
						vertices[i].firstarc=p->nextarc;
						free(p);
						p=vertices[i].firstarc;
						arcnum--;
					}
					else
					{
						q->nextarc=p->nextarc;
						free(p);
						p=q->nextarc;
						arcnum--;
					}
				}
				else
				{
					if(p->adjvex>j)
						p->adjvex--;//修改表结点的顶点位置值(序号)
					q=p;
					p=p->nextarc;
				}

			}
		}
	}
private:
	//初始化邻接链表的表头数组
	void InitVertics( vector<Data*> &dataarray)
	{
		for (int i = 0; i < vexnum; i++)
		{
			vertices[i].name=dataarray[i]->bv;
			vertices[i].data =dataarray[i]->sup;
			vertices[i].tidlist =dataarray[i]->tidlist;
		}
	}

	//插入一个表结点
	void insertArc(int vHead, int vTail, InfoType w)
	{
		//构造一个表结点
		ArcNode *newArcNode = new ArcNode;
		newArcNode->adjvex = vTail;
		newArcNode->nextarc = NULL;
		newArcNode->info = w;

		//arcNode 是vertics[vHead]的邻接表
		ArcNode *arcNode = vertices[vHead].firstarc;
		if (arcNode == NULL)
			vertices[vHead].firstarc = newArcNode;
		else
		{
			while (arcNode->nextarc != NULL)
			{
				arcNode = arcNode->nextarc;
			}
			arcNode->nextarc = newArcNode;
		}
		arcnum++;
	}

	//const数据成员必须在构造函数里初始化

};
/*置信度*/

void Rules(vector<Large> &L )
{
	int lk=L.size();//lk为频繁项目集层数
	int tp1,tp2;

	for(int i=2; i<=lk; i++)              //依次对每个频繁项目集推导关联规则
	{
		cout<<"L"<<i<<':'<<endl;
		for(int j=0; j<L[i-1].n; j++)   //对当前频繁项目集的每一条记录进行推导
		{
			int n=1;
			for(int h=0; h<i; h++)
			{
				n*=2;                 //频繁项集子集总个数（实际总共有(2^i)-2个子集）
			}
			for(int h=1; h<n-1; h++)      //对总数为(2^i)-2个子集进行操作
			{
				int* temp1=new int[ITEMLEN];//申请一维整形数组temp1，存放关联规则前件；整形变量tp1记录关联规则前件包含的项数
				int* temp2=new int[ITEMLEN];//申请一维整形数组temp2，存放关联规则后件；整形变量tp2记录关联规则后件包含的项数
				tp1=0;
				tp2=0;
				int t=1;
				double counts=0;
				for(int m=0; m<i; m++)
				{
					string tt;
					if((h/t)%2==1)
					{
						//						temp1[tp1++]=*(*((*(L+i)).p+j)+m);
						//tt=temp1;
						temp1[tp1++]=((L[i-1].p)[j])[m];
					}
					else
					{
						//						temp2[tp2++]=*(*((*(L+i)).p+j)+m);
						temp2[tp2++]=((L[i-1].p)[j])[m];
					}
					t*=2;
				}
				/*		if(tp1==3)
				{
				for(int io=0;io<tp1;io++)
				{
				cout<<temp1[io]<<endl;
				}
				}*/
				for(int jj=0;jj<L[tp1-1].n;jj++)
				{	
					bool eq=true;
					for(int k=0;k<=tp1-1;k++)
					{
						/*cout<<L[tp1-1].p[jj][k]<<" "<<temp1[k]<<endl;*/
						if(L[tp1-1].p[jj][k]!=temp1[k])
						{
							eq=false;
							break;
						}
					}
					if(eq)
					{
						counts=L[tp1-1].sup[jj]*ITEMNUM;
					}
				}
				//                cout<<temp1<<endl;
				//int len=temp1.length();
				//vector<string>::iterator itit1=find(L[len-1].p.begin(),L[len-1].p.end(),temp1);               //统计关联规则前件在数据库事务中出现的次数
				////                cout<<itit1-L[len-1].p.begin()<<"下标"<<endl;

				// cout<<counts<<"   count"<<endl;
				if((double)(L[i-1].sup[j]*ITEMNUM/counts>=0.04))//检查是否大于等于最小置信度阈值 0.4=min_conf
				{
					for(int jj=0;jj<tp1;jj++)
					{
						cout<<temp1[jj]<<' ';
					}	
					cout<<"==>";
					for(int jj=0;jj<tp2;jj++)
					{
						cout<<temp2[jj]<<' ';
					}	
					cout<<"\t"<<"置信度="<<(double)(L[i-1].sup[j]*ITEMNUM/counts) *100<<"%"<<"\t"<<"\t"<<"支持度="<<(double)(L[i-1].sup[j]) *100<<"%"<<endl;
					switch (tp1)
					{
					case 1:
						switch(tp2)
						{
						case 1:
							strSQL.Format("INSERT INTO `apriori`.`rules` (`firstseq1`, `lastseq1`, `support`, `confidence`)VALUES ('%d', '%d', '%f', '%f');",temp1[0],temp2[0],L[i-1].sup[j],L[i-1].sup[j]*ITEMNUM/counts);
							res=mysql_query(&mysql, strSQL);
							break;
						case 2:
							strSQL.Format("INSERT INTO `apriori`.`rules` (`firstseq1`, `lastseq1`, `lastseq2`, `support`, `confidence`) VALUES ('%d', '%d','%d', '%f', '%f');",temp1[0],temp2[0],temp2[1],L[i-1].sup[j],L[i-1].sup[j]*ITEMNUM/counts);
							mysql_query(&mysql, strSQL);
							break;
						case 3:
							strSQL.Format("INSERT INTO `apriori`.`rules` (`firstseq1`, `lastseq1`, `lastseq2`,`lastseq3`, `support`, `confidence`) VALUES ('%d', '%d','%d','%d', '%f', '%f');",temp1[0],temp2[0],temp2[1],temp2[2],L[i-1].sup[j],L[i-1].sup[j]*ITEMNUM/counts);
							mysql_query(&mysql, strSQL);
							break;
						}
						break;
					case 2:
						switch(tp2)
						{
						case 1:
							strSQL.Format("INSERT INTO `apriori`.`rules`  (`firstseq1`, `firstseq2`, `lastseq1`, `support`, `confidence`)VALUES ('%d', '%d','%d', '%f', '%f');",temp1[0],temp1[1],temp2[0],L[i-1].sup[j],L[i-1].sup[j]*ITEMNUM/counts);
							mysql_query(&mysql, strSQL);
							break;
						case 2:
							strSQL.Format("INSERT INTO `apriori`.`rules`  (`firstseq1`, `firstseq2`, `lastseq1`, `lastseq2`, `support`, `confidence`) VALUES ('%d','%d', '%d','%d', '%f', '%f');",temp1[0],temp1[1],temp2[0],temp2[1],L[i-1].sup[j],L[i-1].sup[j]*ITEMNUM/counts);
							mysql_query(&mysql, strSQL);
							break;
						}
						break;
					case 3:
						strSQL.Format("INSERT INTO `apriori`.`rules`  (`firstseq1`, `firstseq2`, `firstseq3`, `lastseq1`, `support`, `confidence`)VALUES ('%d', '%d','%d','%d', '%f', '%f');",temp1[0],temp1[1],temp1[2],temp2[0],L[i-1].sup[j],L[i-1].sup[j]*ITEMNUM/counts);
						mysql_query(&mysql, strSQL);
						break;
					}

				}
			}
		}
	}
}

int main()
{
	vector<Data*> dataarray;
	int num=0;
	mysql_init(&mysql);  
	mysql_real_connect(&mysql, "localhost", "root", "root", "apriori", 3306, NULL, 0); 
	strSQL.Format("SELECT count(*) FROM apriori.count where sum/(select count(*) from apriori.seq)>0.1;");//直接将第一层筛选交给数据库
	res=mysql_query(&mysql, strSQL);
	if (!res)
	{
		result = mysql_store_result(&mysql);
		if (result)
		{
			row=mysql_fetch_row(result);
			num=atoi(row[0]);
		}
	}
	else
	{
		cout << "query sql failed!" << endl;
	}
	Data **d=new Data*[num];
	for(int i=1; i<=num; i++)
	{
		d[i-1]=new Data();
	}
	strSQL.Format("SELECT pid FROM apriori.count where sum/(select count(*) from apriori.seq)>0.1 order by sum desc;");
	res=mysql_query(&mysql, strSQL);
	int i=0;
	if (!res)
	{
		result = mysql_store_result(&mysql);
		if (result)
		{
			while (row = mysql_fetch_row(result))//获取具体的数据
			{
				d[i++]->bv=atoi(row[0]);
			}
		}
	}
	else
	{
		cout << "query sql failed!" << endl;
	}
	for(int i=1; i<=num; i++)
	{
		/*initData(d[i-1],items);*/
		double count=0;
		strSQL.Format("SELECT id FROM apriori.seq where protocol=%d or process=%d or url=%d or time=%d;",d[i-1]->bv,d[i-1]->bv,d[i-1]->bv,d[i-1]->bv);//填充tidlist
		res=mysql_query(&mysql, strSQL);
		if (!res)
		{
			result = mysql_store_result(&mysql);
			if (result)
			{
				while (row = mysql_fetch_row(result))//获取具体的数据
				{
					d[i-1]->tidlist[ITEMNUM-atoi(row[0])]=1;//最高位为第一个items
					count++;
				}
				d[i-1]->sup=count/ITEMNUM;
			}
		}
		else
		{
			cout << "query sql failed!" << endl;
		}
		dataarray.push_back(d[i-1]);
	}
	map<int,double>minsupmap;
	for(int i=1;i<50;i++)
		minsupmap[i]=0.1;
	vector<Data*>::iterator dataiter;
	vector<Large>Lar;
	Large L1;
	L1.n=dataarray.size();
	for(dataiter=dataarray.begin(); dataiter!=dataarray.end(); dataiter++)
	{
		int *temp=new int[1];
		temp[0]=(*dataiter)->bv;
		L1.p.push_back(temp);
		L1.sup.push_back((*dataiter)->sup);
	}
	Lar.push_back(L1);
	Large L2;
	L2.n=0;
	ALGraph<int,double, bitset<ITEMNUM>,int> dgGraph(L1.n);
	dgGraph.CreateDG(dataarray,minsupmap,L2);
	Lar.push_back(L2);
	//dgGraph.displayGraph();
	while(!dgGraph.isempty())
	{
		/*dgGraph.displayGraph();*/
		dgGraph.DFSsearch(dataarray,minsupmap,Lar);
		dgGraph.deleteVex();
		dataarray.erase(dataarray.begin());
	}
	/*for (int i=0;i<Lar.size();i++)
	{
		cout<<"L"<<i+1<<endl;
		for(int j=0;j<Lar[i].n;j++)
		{	
			for(int k=0;k<=i;k++)
			{
				cout<<Lar[i].p[j][k]<<" ";
			}
			cout<<endl;
			cout<<Lar[i].sup[j]<<endl;
		}
	}*/
	Rules(Lar);
}
