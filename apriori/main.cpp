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
#define ITEMNUM 1280
#define ITEMLEN 3
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
void initData(Data *d,int items[ITEMNUM][ITEMLEN])
{
	double count=0;
	for(int j=1; j<=ITEMNUM; j++)
	{
		for(int k=1;k<=ITEMLEN;k++)
		{
			if(items[j-1][k-1]==d->bv)
			{
				count++;
				d->tidlist[ITEMNUM-j]=1;//最高位为第一个items
			}
		}
	}
	d->sup=count/ITEMNUM;
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
						temp[0]=vi->bv;
						temp[1]=vj->bv;
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
		ArcNode* p;
		int* visited=new int [vexnum+1]();
		visited[0] = 1;
		s.push(0);
		while (!s.empty())
		{
			p=vertices[s.top()].firstarc;
			while(p)
			{
				if(!visited[p->adjvex])
				{
					visited[p->adjvex]=1;
					if(s.size()>=2)
					{
						Stack<int>::iterator ii= s.begin();						
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
								}
							}
							s.push(p->adjvex);
						}
					}
					else
					{
						s.push(p->adjvex);
					}
					break;
				}
				else
					p=p->nextarc;
			}
			if (p == NULL)
			{
				s.pop();
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

};
/*置信度*/

void Rules(vector<Large> &L,int l[ITEMNUM][ITEMLEN] )
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
				for(int jj=0;jj<L[tp1-1].n;jj++)
				{	
					bool eq=true;
					for(int k=0;k<=tp1-1;k++)
					{
						//cout<<L[tp1-1].p[jj][k]<<" ";
						if(L[tp1-1].p[jj][k]!=temp1[tp1-1-k])
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
				if((double)(L[i-1].sup[j]*ITEMNUM/counts>=0.01))//检查是否大于等于最小置信度阈值 0.2=min_conf
				{
					//if(temp2=="5"||temp2=="6"||temp2=="7")
					//{
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
					//}
				}
			}
		}
	}
}

int main()
{
	int items[ITEMNUM][ITEMLEN];
	MYSQL mysql;  
	MYSQL_RES *result;  
	MYSQL_ROW row;  
	int res;
	mysql_init(&mysql);  
	mysql_real_connect(&mysql, "localhost", "root", "root", "apriori", 3306, NULL, 0); 
	CString strSQL;
	strSQL.Format("SELECT * FROM seq");
	res=mysql_query(&mysql, strSQL);
	if (!res)
	{
		result = mysql_store_result(&mysql);
		if (result)
		{
			while (row = mysql_fetch_row(result))//获取具体的数据
			{
				items[atoi(row[0])-1][0]=atoi(row[1]);
				items[atoi(row[0])-1][1]=atoi(row[2]);
				items[atoi(row[0])-1][2]=atoi(row[3]);
			}
		}
	}
	else
	{
		cout << "query sql failed!" << endl;
	}
	//1 8-18 2 18-8 3 guohy 4lvtx 5xz 6outlook 7 liulan
	/*items[0][0]=1;
	items[0][1]=3;
	items[0][2]=5;
	items[1][0]=2;
	items[1][1]=4;
	items[1][2]=5;
	items[2][0]=1;
	items[2][1]=4;
	items[2][2]=6;
	items[3][0]=2;
	items[3][1]=3;
	items[3][2]=5;
	items[4][0]=1;
	items[4][1]=3;
	items[4][2]=7;
	items[5][0]=1;
	items[5][1]=4;
	items[5][2]=7;
	items[6][0]=2;
	items[6][1]=3;
	items[6][2]=7;
	items[7][0]=1;
	items[7][1]=4;
	items[7][2]=5;
	items[8][0]=2;
	items[8][1]=4;
	items[8][2]=5;
	items[9][0]=1;
	items[9][1]=4;
	items[9][2]=7;*/
	map<int,int>itemmap;
	int id=1;
	for(int i=1; i<=ITEMNUM; i++)
	{
		for(int j=1; j<=ITEMLEN; j++)
		{
			int word=items[i-1][j-1];
			map<int,int>::iterator it;
			it=itemmap.find(word);
			if(it==itemmap.end())
			{
				itemmap[word]=id;
				id++;
			}
		}
	}
	int num=itemmap.size();
	vector<Data*> dataarray;
	Data **d=new Data*[num];
	for(int i=1; i<=num; i++)
	{
		d[i-1]=new Data();
	}
	map<int,int>::iterator iter;
	for( iter=itemmap.begin(); iter!=itemmap.end(); iter++)
	{
		int a= iter->first;
		int p = iter->second;
		d[p-1]->bv=a;
	}
	for(int i=1; i<=num; i++)
	{
		initData(d[i-1],items);
		dataarray.push_back(d[i-1]);
	}
	sort(dataarray.begin(),dataarray.end(),Comp);
	map<int,double>minsupmap;
	for(int i=1;i<50;i++)
		minsupmap[i]=0.1;
	//minsupmap[1]=0.2;
	//minsupmap[2]=0.1;
	//minsupmap[3]=0.09;
	//minsupmap[4]=0.2;
	//minsupmap[5]=0.1;
	//minsupmap[6]=0.3;
	//minsupmap[7]=0.3;
	vector<Data*>::iterator dataiter;
	for(dataiter=dataarray.begin(); dataiter!=dataarray.end();)
	{

		if((*dataiter)->sup<minsupmap[(*dataiter)->bv])
		{
			dataiter=dataarray.erase(dataiter);    //删除元素，返回值指向已删除元素的下一个位置
		}
		else
		{
			++dataiter;    //指向下一个位置
		}
	}
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

	while(!dgGraph.isempty())
	{
		//dgGraph.displayGraph();
		dgGraph.DFSsearch(dataarray,minsupmap,Lar);
		dgGraph.deleteVex();
		dataarray.erase(dataarray.begin());
	}
	//for (int i=0;i<Lar.size();i++)
	//{
	//	cout<<"L"<<i+1<<endl;
	//	for(int j=0;j<Lar[i].n;j++)
	//	{	
	//		for(int k=0;k<=i;k++)
	//		{
	//			cout<<Lar[i].p[j][k]<<" ";
	//		}
	//		cout<<endl;
	//	}
	//}
	Rules(Lar,items);
}


